/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <device/mmio.h>
#include <device/device.h>
#include <device/pci.h>		/* device_operations */
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <device/azalia_device.h>
#include <bootstate.h>
#include <arch/ioapic.h>
#include <device/smbus.h>	/* smbus_bus_operations */
#include <pc80/mc146818rtc.h>
#include <pc80/i8254.h>
#include <pc80/i8259.h>
#include <console/console.h>	/* printk */
#include <arch/acpi.h>
#include <device/pci_ehci.h>
#include "lpc.h"		/* lpc_read_resources */
#include "SBPLATFORM.h"		/* Platform Specific Definitions */
#include "cfg.h"		/* sb800 Cimx configuration */
#include "chip.h"		/* struct southbridge_amd_cimx_sb800_config */
#include "sb_cimx.h"		/* AMD CIMX wrapper entries */
#include "smbus.h"
#include "fan.h"
#include "pci_devs.h"
#include <southbridge/amd/common/amd_pci_util.h>

static AMDSBCFG sb_late_cfg; //global, init in sb800_cimx_config
static AMDSBCFG *sb_config = &sb_late_cfg;

/**
 * @brief Entry point of Southbridge CIMx callout
 *
 * prototype UINT32 (*SBCIM_HOOK_ENTRY)(UINT32 Param1, UINT32 Param2, void* pConfig)
 *
 * @param[in] func      Southbridge CIMx Function ID.
 * @param[in] data      Southbridge Input Data.
 * @param[in] config    Southbridge configuration structure pointer.
 *
 */
static u32 sb800_callout_entry(u32 func, u32 data, void *config)
{
	u32 ret = 0;
	printk(BIOS_DEBUG, "SB800 - Late.c - %s - Start.\n", __func__);
	switch (func) {
	case CB_SBGPP_RESET_ASSERT:
		break;

	case CB_SBGPP_RESET_DEASSERT:
		break;

	case IMC_FIRMWARE_FAIL:
		break;

	default:
		break;
	}

	printk(BIOS_DEBUG, "SB800 - Late.c - %s - End.\n", __func__);
	return ret;
}

#define HOST_CAP                  0x00 /* host capabilities */
#define HOST_CTL                  0x04 /* global host control */
#define HOST_IRQ_STAT             0x08 /* interrupt status */
#define HOST_PORTS_IMPL           0x0c /* bitmap of implemented ports */

#define HOST_CTL_AHCI_EN          (1 << 31) /* AHCI enabled */
#define HOST_CTL_IE               (1 << 1)  /* global HBA interrupt enable */

#define AHCI_PORT_REG_BASE        0x100     /* first port register block */
#define AHCI_PORT_SIZE            0x80      /* each port block is 0x80 bytes */
#define AHCI_PORT_PxIS            0x10      /* port interrupt status (W1C) */
#define AHCI_PORT_PxSERR          0x30      /* port serial ATA error (W1C) */

static void ahci_raid_init(struct device *dev)
{
	u8 irq = 0;
	void *bar5;
	u32 caps, ports, val;
	int port;

	val = pci_read_config16(dev, PCI_CLASS_DEVICE);
	if (val == PCI_CLASS_STORAGE_SATA) {
		printk(BIOS_DEBUG, "AHCI controller ");
	} else if (val == PCI_CLASS_STORAGE_RAID) {
		printk(BIOS_DEBUG, "RAID controller ");
	} else {
		printk(BIOS_WARNING, "device class:%x, neither in ahci or raid mode\n", val);
		return;
	}

	irq = pci_read_config8(dev, PCI_INTERRUPT_LINE);
	bar5 = (void *)(uintptr_t)(pci_read_config32(dev, PCI_BASE_ADDRESS_5) & ~0xf);
	printk(BIOS_DEBUG, "IOMEM base: %p, IRQ: 0x%X\n", bar5, irq);

	caps = read32(bar5 + HOST_CAP);
	caps = (caps & 0x1F) + 1;
	ports= read32(bar5 + HOST_PORTS_IMPL);
	printk(BIOS_DEBUG, "Number of Ports: 0x%x, Port implemented(bit map): 0x%x\n", caps, ports);

	/* make sure ahci is enabled */
	val = read32(bar5 + HOST_CTL);
	if (!(val & HOST_CTL_AHCI_EN)) {
		write32(bar5 + HOST_CTL, val | HOST_CTL_AHCI_EN);
	}

	/* disable GHC.IE then wipe PxIS/PxSERR; CIMx can leave these set */
	val = read32(bar5 + HOST_CTL);
	write32(bar5 + HOST_CTL, val & ~HOST_CTL_IE);
	for (port = 0; port < 32; port++) {
		if (!(ports & (1u << port)))
			continue;
		void *pbase = bar5 + AHCI_PORT_REG_BASE + port * AHCI_PORT_SIZE;
		write32(pbase + AHCI_PORT_PxIS,   0xFFFFFFFF);
		write32(pbase + AHCI_PORT_PxSERR, 0xFFFFFFFF);
	}
	write32(bar5 + HOST_IRQ_STAT, 0xFFFFFFFF);

	dev->command |= PCI_COMMAND_MASTER;
	pci_write_config8(dev, PCI_COMMAND, dev->command);
	printk(BIOS_DEBUG, "AHCI/RAID controller initialized\n");
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SB800 - Late.c - lpc_init - Start.\n");

	cmos_check_update_date();

	/* Initialize the real time clock.
	 * The 0 argument tells cmos_init not to
	 * update CMOS unless it is invalid.
	 * 1 tells cmos_init to always initialize the CMOS.
	 */
	cmos_init(0);

	setup_i8259(); /* Initialize i8259 pic */
	setup_i8254(); /* Initialize i8254 timers */

	printk(BIOS_DEBUG, "SB800 - Late.c - lpc_init - End.\n");
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* Just a dummy */
	return current;
}

static const char *lpc_acpi_name(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	/* DSDT: acpi/lpc.asl */
	case LPC_DEVFN:
		return "LIBR";
	}

	return NULL;
}

static struct device_operations lpc_ops = {
	.read_resources = lpc_read_resources,
	.set_resources = lpc_set_resources,
	.enable_resources = pci_dev_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = acpi_write_hpet,
#endif
	.init = lpc_init,
	.scan_bus = scan_static_bus,
	.ops_pci = &lops_pci,
	.acpi_name = lpc_acpi_name,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_LPC,
};

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = ahci_raid_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver ahci_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SATA_AHCI,
};

static const struct pci_driver raid_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SATA_RAID,
};
static const struct pci_driver raid5_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SATA_RAID5,
};

/* SB900-era EHCI improvements and A12+/A13+ errata; SB800 CIMx predates these */
static void usb_ehci_init(struct device *dev)
{
	u32 bar;
	u8 rev = pci_s_read_config8(PCI_DEV(0, 0x14, 0), 0x08);

	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_0) & ~0x0f;
	if (!bar)
		return;

	printk(BIOS_DEBUG, "SB800 EHCI %s: post-CIMx fixups (BAR=%08x rev=%02x)\n",
	       dev_path(dev), bar, rev);

	/* Enable Dynamic Clock Gating (BIT12) + Global Clock Gating (BIT14).
	 * The SB800 CIMx clears BIT12; the SB900 CIMx sets both. */
	write32((void *)(uintptr_t)(bar + SB_EHCI_BAR_REGBC),
		(read32((void *)(uintptr_t)(bar + SB_EHCI_BAR_REGBC))
		 & ~(BIT12 | BIT14)) | BIT12 | BIT14);

	/* PHY enhancement (SB900 sets BIT5 at BAR+0xB0) */
	write32((void *)(uintptr_t)(bar + SB_EHCI_BAR_REGB0),
		read32((void *)(uintptr_t)(bar + SB_EHCI_BAR_REGB0)) | BIT5);

	/* L1 Early Exit + PING response fix + empty-list detection.
	 * SB800 CIMx only sets BIT0 at PCI 0x54; SB900 uses 0x26b. */
	pci_write_config32(dev, SB_EHCI_REG54,
		(pci_read_config32(dev, SB_EHCI_REG54) & ~0x7ff) | 0x26b);

	/* A12+ errata (SB900 CIMx: RPR 7.25, 7.30) */
	if (rev >= 0x12) {
		/* RPR 7.25: LS wake from S3/S4/S5 when EHCI owns port */
		pci_write_config32(dev, SB_EHCI_REG54,
			pci_read_config32(dev, SB_EHCI_REG54) | BIT4);
		/* RPR 7.30: lMU hang near end of uFrame 7 */
		pci_write_config32(dev, SB_EHCI_REG54,
			pci_read_config32(dev, SB_EHCI_REG54) | BIT11);
	}

	/* A13+ errata (SB900 CIMx) */
	if (rev >= 0x13) {
		/* SB02674 (RPR 7.25): framelist read delay */
		pci_write_config16(dev, SB_EHCI_REG54,
			(pci_read_config16(dev, SB_EHCI_REG54) & 0x5FFF)
			| BIT13 | BIT15);
		/* SB02684 (RPR 7.26): timing compensation */
		pci_write_config16(dev, SB_EHCI_REG50 + 2,
			pci_read_config16(dev, SB_EHCI_REG50 + 2) | BIT3);
		/* SB02687 (RPR 7.26): controller stability */
		pci_write_config16(dev, SB_EHCI_REG54 + 2,
			(pci_read_config16(dev, SB_EHCI_REG54 + 2) & 0xFFFC)
			| BIT0 | BIT1);
		/* SB02700 (RPR 7.28): data transfer enhancement */
		pci_write_config16(dev, SB_EHCI_REG54 + 2,
			(pci_read_config16(dev, SB_EHCI_REG54 + 2) & 0xFFFB)
			| BIT2);
		/* SB02703 (RPR 7.29): frame synchronization */
		pci_write_config16(dev, SB_EHCI_REG54 + 2,
			(pci_read_config16(dev, SB_EHCI_REG54 + 2) & 0xFFF7)
			| BIT3);
	}
}

/* SB900-era OHCI clock gating and A12+ arbiter errata; SB800 CIMx predates these */
static void usb_ohci_init(struct device *dev)
{
	u8 rev = pci_s_read_config8(PCI_DEV(0, 0x14, 0), 0x08);

	printk(BIOS_DEBUG, "SB800 OHCI %s: post-CIMx fixups (rev=%02x)\n",
	       dev_path(dev), rev);

	/* L1 Early Exit + Enhanced Arbiter + Global Clock Gating.
	 * The SB800 CIMx does not touch register 0x80 at all. */
	pci_write_config32(dev, 0x80,
		(pci_read_config32(dev, 0x80)
		 & ~(BIT0 | BIT4 | BIT5 | BIT6 | BIT7))
		| BIT0 | BIT4 | BIT7);

	/* A12+ (RPR 7.21): enhanced arbiter mode + USB 1.1 false CRC fix */
	if (rev >= 0x12) {
		pci_write_config16(dev, 0x80,
			(pci_read_config16(dev, 0x80) & ~(BIT4 | BIT5 | BIT8))
			| BIT4 | BIT5 | BIT8);
		/* RPR 7.23: USB 1.1 full-speed false CRC errors fix */
		pci_write_config32(dev, 0x80,
			pci_read_config32(dev, 0x80) | BIT10);
	}

	/* SOF Synchronization + Periodic List Advance (PCI 0x52 BIT3|BIT4) */
	pci_write_config8(dev, 0x52,
		pci_read_config8(dev, 0x52) | BIT3 | BIT4);
}

static struct device_operations usb_ehci_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_ehci_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static struct device_operations usb_ohci_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_ohci_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

/*
 * The pci id of usb ctrl 0 and 1 are the same.
 */
static const struct pci_driver usb_ohci123_driver __pci_driver = {
	.ops = &usb_ohci_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_18_0, /* OHCI-USB1, OHCI-USB2, OHCI-USB3 */
};

static const struct pci_driver usb_ehci123_driver __pci_driver = {
	.ops = &usb_ehci_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_18_2, /* EHCI-USB1, EHCI-USB2, EHCI-USB3 */
};

static const struct pci_driver usb_ohci4_driver __pci_driver = {
	.ops = &usb_ohci_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_20_5, /* OHCI-USB4 */
};


static struct device_operations azalia_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	/* SB800 CIMx omitted this; without it the codec verb table is never written */
	.init = azalia_audio_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver azalia_driver __pci_driver = {
	.ops = &azalia_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_HDA,
};


static struct device_operations gec_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = 0,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver gec_driver __pci_driver = {
	.ops = &gec_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_GEC,
};

/* use pci_scan_bridge to prevent pciexp_enable_aspm running on SB GPP ports */
static struct device_operations sb_pcie_ops = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = 0,
	.scan_bus         = pci_scan_bridge,
	.ops_pci          = &lops_pci,
};

static const unsigned short sb_pcie_devs[] = {
	PCI_DEVICE_ID_ATI_SB800_PCIEA,
	PCI_DEVICE_ID_ATI_SB800_PCIEB,
	PCI_DEVICE_ID_ATI_SB800_PCIEC,
	PCI_DEVICE_ID_ATI_SB800_PCIED,
	0
};

static const struct pci_driver sb_pcie_driver __pci_driver = {
	.ops = &sb_pcie_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.devices = sb_pcie_devs,
};

/**
 *  Fill build time defaults.
 */
static void sb800_init(void *chip_info)
{
	printk(BIOS_DEBUG, "SB800: %s\n", __func__);
	sb_config->StdHeader.CALLBACK.CalloutPtr = sb800_callout_entry;
	sb800_cimx_config(sb_config);

	/* Initially enable all GPP ports 0 to 3 */
	abcfg_reg(0xc0, 0x01FF, 0x0F4);

	/* USB power management enhancement from SB900 CIMx (PMIO 0xF0 BIT12) */
	RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF0,
	      AccWidthUint16, 0xFFFF, BIT12);

	/* A13+ errata from SB900 CIMx; same silicon, SB800 CIMx predates these */
	{
		u8 rev = pci_s_read_config8(PCI_DEV(0, 0x14, 0), 0x08);

		if (rev >= 0x13) {
			printk(BIOS_DEBUG, "SB800: applying A13+ errata fixups (rev=%02x)\n", rev);

			/* SB02643: USB FidVid power management */
			RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGB4,
			      AccWidthUint8, ~BIT7, BIT7);

			/* SB02686 (RPR 7.27): USB gating control */
			RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGED,
			      AccWidthUint8, ~BIT2, BIT2);

			/* RPR 2.14: Enable delayed SLP_S3/S5 to the board */
			RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC0 + 1,
			      AccWidthUint8, 0xFF, BIT2);

			/* RPR 2.15: Enable C-State wake-up before warm reset */
			RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGBE,
			      AccWidthUint8, ~BIT0, BIT0);

			/* RPR 3.16: Disable LPC A-Link cycle bypass */
			RWMEM(ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG50,
			      AccWidthUint32, ~BIT19, BIT19);

			/* RPR 3.5: Enable LPCCLK0 power-down (when IMC disabled).
			 * OBS261463: skip if IMC is enabled (fan control). */
			if (!CONFIG(SB800_IMC_FWM)) {
				RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD2,
				      AccWidthUint8, ~BIT3, BIT3);
			}
		}
	}
}

/**
 * South Bridge CIMx ramstage entry point wrapper.
 */
void sb_Before_Pci_Init(void)
{
	printk(BIOS_DEBUG, "SB800: %s\n", __func__);
	sb_config->StdHeader.Func = SB_BEFORE_PCI_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_After_Pci_Init(void)
{
	printk(BIOS_DEBUG, "SB800: %s\n", __func__);
	sb_config->StdHeader.Func = SB_AFTER_PCI_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_Mid_Post_Init(void)
{
	printk(BIOS_DEBUG, "SB800: %s\n", __func__);
	sb_config->StdHeader.Func = SB_MID_POST_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_Late_Post(void)
{
	printk(BIOS_DEBUG, "SB800: %s\n", __func__);
	sb_config->StdHeader.Func = SB_LATE_POST_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_Before_Pci_Restore_Init(void)
{
	printk(BIOS_DEBUG, "SB800: %s\n", __func__);
	sb_config->StdHeader.Func = SB_BEFORE_PCI_RESTORE_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_After_Pci_Restore_Init(void)
{
	printk(BIOS_DEBUG, "SB800: %s\n", __func__);
	sb_config->StdHeader.Func = SB_AFTER_PCI_RESTORE_INIT;
	AmdSbDispatcher(sb_config);
}

/*
 * Update the PCI devices with a valid IRQ number
 * that is set in the mainboard PCI_IRQ structures.
 */
static void set_pci_irqs(void *unused)
{
	/* Write PCI_INTR regs 0xC00/0xC01 */
	write_pci_int_table();

	/* Write IRQs for all devicetree enabled devices */
	write_pci_cfg_irqs();
}

/*
 * Hook this function into the PCI state machine
 * on entry into BS_DEV_ENABLE.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, set_pci_irqs, NULL);

/**
 * @brief SB Cimx entry point sbBeforePciInit wrapper
 */
static void sb800_enable(struct device *dev)
{
	struct southbridge_amd_cimx_sb800_config *sb_chip =
		(struct southbridge_amd_cimx_sb800_config *)(dev->chip_info);

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0x11, 0): /* 0:11.0  SATA */
		if (dev->enabled) {
			sb_config->SATAMODE.SataMode.SataController = CIMX_OPTION_ENABLED;
			if (sb_chip->boot_switch_sata_ide == 1)
				sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt = 0; //0 -IDE as primary.
			else if (sb_chip->boot_switch_sata_ide == 0)
				sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt = 1; //1 -IDE as secondary.
		} else {
			sb_config->SATAMODE.SataMode.SataController = CIMX_OPTION_DISABLED;
		}
		break;

	case PCI_DEVFN(0x14, 0): /* 0:14:0 SMBUS */
		clear_ioapic(VIO_APIC_VADDR);
		/* Assign the ioapic ID the next available number after the processor core local APIC IDs */
		setup_ioapic(VIO_APIC_VADDR, CONFIG_MAX_CPUS);
		break;

	case PCI_DEVFN(0x14, 1): /* 0:14:1 IDE */
		break;

	case PCI_DEVFN(0x14, 2): /* 0:14:2 HDA */
		if (dev->enabled) {
			if (sb_config->AzaliaController == AZALIA_DISABLE) {
				sb_config->AzaliaController = AZALIA_AUTO;
			}
		} else {
			sb_config->AzaliaController = AZALIA_DISABLE;
		}
		break;


	case PCI_DEVFN(0x14, 3): /* 0:14:3 LPC */
		/* Initialize the fans */
#if CONFIG(SB800_IMC_FAN_CONTROL)
		init_sb800_IMC_fans(dev);
#elif CONFIG(SB800_MANUAL_FAN_CONTROL)
		init_sb800_MANUAL_fans(dev);
#endif
		break;

	case PCI_DEVFN(0x14, 4): /* 0:14:4 PCI */
		/* PcibConfig [PM_Reg: EAh], PCIDisable [Bit0]
		 * 'PCIDisable' set to 0 to enable P2P bridge.
		 * 'PCIDisable' set to 1 to disable P2P bridge and enable PCI interface pins
		 *              to function as GPIO {GPIO 35:0}.
		 */
		if (!sb_chip->disconnect_pcib && dev->enabled)
			RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEA, AccWidthUint8, ~BIT0, 0);
		else
			RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEA, AccWidthUint8, ~BIT0, BIT0);
		break;

	case PCI_DEVFN(0x14, 6): /* 0:14:6 GEC */
		if (dev->enabled) {
			sb_config->GecConfig = 0;
		} else {
			sb_config->GecConfig = 1;
		}
		break;

	case PCI_DEVFN(0x15, 0): /* 0:15:0 PCIe PortA */
		{
			struct device *device;
			for (device = dev; device; device = device->sibling) {
				if ((device->path.pci.devfn & ~3) != PCI_DEVFN(0x15,0)) break;
				sb_config->PORTCONFIG[device->path.pci.devfn & 3].PortCfg.PortPresent = device->enabled;
			}

			/*
			 * GPP_CFGMODE_X4000: PortA Lanes[3:0]
			 * GPP_CFGMODE_X2200: PortA Lanes[1:0], PortB Lanes[3:2]
			 * GPP_CFGMODE_X2110: PortA Lanes[1:0], PortB Lane2, PortC Lane3
			 * GPP_CFGMODE_X1111: PortA Lanes0, PortB Lane1, PortC Lane2, PortD Lane3
			 */
			sb_config->GppLinkConfig = sb_chip->gpp_configuration;
		}
		break;

	case PCI_DEVFN(0x12, 0): /* 0:12:0 OHCI-USB1 */
		sb_config->USBMODE.UsbMode.Ohci1 = dev->enabled;
		break;
	case PCI_DEVFN(0x12, 2): /* 0:12:2 EHCI-USB1 */
		sb_config->USBMODE.UsbMode.Ehci1 = dev->enabled;
		break;
	case PCI_DEVFN(0x13, 0): /* 0:13:0 OHCI-USB2 */
		sb_config->USBMODE.UsbMode.Ohci2 = dev->enabled;
		break;
	case PCI_DEVFN(0x13, 2): /* 0:13:2 EHCI-USB2 */
		sb_config->USBMODE.UsbMode.Ehci2 = dev->enabled;
		break;
	case PCI_DEVFN(0x14, 5): /* 0:14:5 OHCI-USB4 */
		sb_config->USBMODE.UsbMode.Ohci4 = dev->enabled;
		break;
	case PCI_DEVFN(0x16, 0): /* 0:16:0 OHCI-USB3 */
		sb_config->USBMODE.UsbMode.Ohci3 = dev->enabled;
		break;
	case PCI_DEVFN(0x16, 2): /* 0:16:2 EHCI-USB3 */
		sb_config->USBMODE.UsbMode.Ehci3 = dev->enabled;

		/* FIXME: Find better callsites for these.
		 * call the CIMX entry at the last sb800 device,
		 * so make sure the mainboard devicetree is complete
		 */
		if (!acpi_is_wakeup_s3())
			sb_Before_Pci_Init();
		else
			sb_Before_Pci_Restore_Init();
		break;

	default:
		break;
	}
}

/* clear ASPM on GPP ports in gpp_aspm_disable: root+endpoint LnkCtl, LnkCap ASPMS, stale PME_Status */
static void sb800_final(void *chip_info)
{
	struct southbridge_amd_cimx_sb800_config *sb = chip_info;
	static const u8 gpp_devfns[4] = {
		SB_PCIE_PORT1_DEVFN,
		SB_PCIE_PORT2_DEVFN,
		SB_PCIE_PORT3_DEVFN,
		SB_PCIE_PORT4_DEVFN,
	};
	unsigned int i;

	if (!sb || !sb->gpp_aspm_disable)
		return;

	for (i = 0; i < ARRAY_SIZE(gpp_devfns); i++) {
		struct device *rp;
		u16 cap;
		u32 lnkcap;
		u16 lnkctl;

		if (!(sb->gpp_aspm_disable & (1 << i)))
			continue;

		rp = dev_find_slot(0, gpp_devfns[i]);
		if (!rp || !rp->enabled)
			continue;
		cap = pci_find_capability(rp, PCI_CAP_ID_PCIE);
		if (!cap)
			continue;

		lnkcap = pci_read_config32(rp, cap + PCI_EXP_LNKCAP);
		if (lnkcap & PCI_EXP_LNKCAP_ASPMS) {
			pci_write_config32(rp, cap + PCI_EXP_LNKCAP,
					   lnkcap & ~PCI_EXP_LNKCAP_ASPMS);
			printk(BIOS_INFO,
			       "sb800: GPP%u: cleared LnkCap ASPM (was 0x%08x)\n",
			       i + 1, lnkcap);
		}

		lnkctl = pci_read_config16(rp, cap + PCI_EXP_LNKCTL);
		if (lnkctl & (0x3 | PCI_EXP_EN_CLK_PM))
			pci_write_config16(rp, cap + PCI_EXP_LNKCTL,
					   lnkctl & ~(0x3 | PCI_EXP_EN_CLK_PM));

		if (!rp->link_list)
			continue;
		struct device *ep;
		for (ep = rp->link_list->children; ep; ep = ep->sibling) {
			u16 ep_cap = pci_find_capability(ep, PCI_CAP_ID_PCIE);
			if (ep_cap) {
				u16 ep_lnkctl = pci_read_config16(ep, ep_cap + PCI_EXP_LNKCTL);
				if (ep_lnkctl & (0x3 | PCI_EXP_EN_CLK_PM)) {
					pci_write_config16(ep, ep_cap + PCI_EXP_LNKCTL,
						ep_lnkctl & ~(0x3 | PCI_EXP_EN_CLK_PM));
					printk(BIOS_INFO,
					       "sb800: GPP%u: cleared %s LnkCtl ASPM/CLK_PM"
					       " (was 0x%04x)\n",
					       i + 1, dev_path(ep), ep_lnkctl);
				}
			}

			u8 pm_cap = pci_find_capability(ep, PCI_CAP_ID_PM);
			if (pm_cap) {
				u16 pmcsr = pci_read_config16(ep, pm_cap + PCI_PM_CTRL);
				if (pmcsr & PCI_PM_CTRL_PME_STATUS)
					pci_write_config16(ep, pm_cap + PCI_PM_CTRL, pmcsr);
			}
		}
	}
}

struct chip_operations southbridge_amd_cimx_sb800_ops = {
	CHIP_NAME("ATI SB800")
	.init = sb800_init,
	.enable_dev = sb800_enable,
	.final = sb800_final,
};
