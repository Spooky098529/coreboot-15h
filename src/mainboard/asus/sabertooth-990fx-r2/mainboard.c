/*
 * mainboard.c - Mainboard-specific initialization
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <southbridge/amd/common/amd_pci_util.h>
#include <southbridge/amd/cimx/cimx_util.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>
#include <southbridge/amd/cimx/sb800/gpio_oem.h>
#include <southbridge/amd/cimx/sb800/pci_devs.h>
#include <northbridge/amd/agesa/family15/pci_devs.h>

/* SB GPP ASPM - default disabled
   TODO: Connect this with ASPM configuration and test,
         when testing devices behind the SB GPP did
         not seem to work correctly with ASPM */
#define SB_GPP_PORT1_ASPM 0 /* cap=3 active=0 */
#define SB_GPP_PORT2_ASPM 0 /* cap=3 active=0 */
#define SB_GPP_PORT3_ASPM 0 /* cap=3 active=0 */
#define SB_GPP_PORT4_ASPM 0 /* cap=3 active=0 */

/* A-Link / GPP PHY PLL power-down
   TODO: Test if enabling this saves power
         without causing a-link latency */
#define ALINK_PHY_PLL_POWER_DOWN FALSE
#define GPP_PHY_PLL_POWER_DOWN FALSE

/* NB-side A-Link ASPM off, see above TODO */
#define NB_SB_LINK_ASPM 0 /* 0=off, 1=L0s, 2=L1, 3=L0s+L1 */

/*
 * PIC interrupt routing table.
 * FCH_INT_TABLE_SIZE (0x54) byte array.
 * PIRQ values taken from SB PM regs 0x55-0x5C.
 * 0x1F = disabled (no IRQ assigned).
 * Configuration avoids conflicts with IDE IRQs
 * and things like timer IRQs to help with avoiding
 * boot-time operation and legacy PIC operation.
 */
static const u8 mainboard_picr_data[FCH_INT_TABLE_SIZE] = {
	/* PIRQA..H */
	[0x00] = 0x06,
	[0x01] = 0x0B,	/* PIRQB: was IRQ 11 */
	[0x02] = 0x0A,
	[0x03] = 0x03,
	[0x04] = 0x0B,
	[0x05] = 0x0A,	/* PIRQF: IRQ 10 (Realtek NIC INTA uses PIRQF) */
	[0x06] = 0x0B,	/* PIRQG: IRQ 11 */
	[0x07] = 0x0B,
	/* Misc: SCI, SMBus, ASF, HDA, FC, GEC, PerfMon, SD */
	[0x08] = 0x00,
	[0x09] = 0xF0,
	[0x0A] = 0x00,
	[0x0B] = 0x00,
	[0x0C] = 0x1F,
	[0x0D] = 0x1F,
	[0x0E] = 0x1F,
	[0x0F] = 0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, GEC, PerfMon */
	[0x10] = 0x1F,
	[0x11] = 0x1F,
	[0x12] = 0x1F,
	[0x13] = 0x06,
	[0x14] = 0x1F,
	[0x15] = 0x1F,
	[0x16] = 0x1F,
	/* IMC - untested */
	[0x20] = 0x1F,
	[0x21] = 0x1F,
	[0x22] = 0x1F,
	[0x23] = 0x1F,
	[0x24] = 0x1F,
	[0x25] = 0x1F,
	/* USB: OHCI1, EHCI1, OHCI2, EHCI2, OHCI3, EHCI3, OHCI4 */
	[0x30] = 0x0A,
	[0x31] = 0x0B,	/* EHCI1 -> IRQ 11 */
	[0x32] = 0x0B,
	[0x33] = 0x0A,	/* EHCI2 -> IRQ 10 */
	[0x34] = 0x0B,	/* OHCI3 -> IRQ 11 */
	[0x35] = 0x0B,
	[0x36] = 0x0A,
	/* IDE, SATA */
	[0x40] = 0x1F,
	[0x41] = 0x03,
	/* GPP0..GPP3 */
	[0x50] = 0x0B,
	[0x51] = 0x0A,	/* GPP1 (Realtek RTL8168 NIC) -> IRQ 10 */
	[0x52] = 0x0B,	/* GPP2 (ASM1042 USB3) -> IRQ 11 */
	[0x53] = 0x0B,
};

static const u8 mainboard_intr_data[FCH_INT_TABLE_SIZE] = {
	/* PIRQA..H -> APIC GSI 16..23 */
	[0x00] = 0x10,
	[0x01] = 0x11,
	[0x02] = 0x12,
	[0x03] = 0x13,
	[0x04] = 0x14,
	[0x05] = 0x15,
	[0x06] = 0x16,
	[0x07] = 0x17,
	/* Misc */
	[0x08] = 0x00,
	[0x09] = 0x00,	/* MISC0: 0x00 = disabled */
	[0x0A] = 0x00,
	[0x0B] = 0x00,
	[0x0C] = 0x1F,
	[0x0D] = 0x1F,
	[0x0E] = 0x1F,
	[0x0F] = 0x1F,
	/* SCI, SMBUS0, ASF, HDA, FC, GEC, PerfMon */
	[0x10] = 0x09, /* SCI: IRQ 9 -> GSI 9 */
	[0x11] = 0x1F,
	[0x12] = 0x1F,
	[0x13] = 0x10,
	[0x14] = 0x1F,
	[0x15] = 0x1F,
	[0x16] = 0x1F,
	/* IMC */
	[0x20] = 0x1F,
	[0x21] = 0x1F,
	[0x22] = 0x1F,
	[0x23] = 0x1F,
	[0x24] = 0x1F,
	[0x25] = 0x1F,
	/* USB: OHCI1, EHCI1, OHCI2, EHCI2, OHCI3, EHCI3, OHCI4 */
	[0x30] = 0x12,
	[0x31] = 0x11,
	[0x32] = 0x14,
	[0x33] = 0x15,
	[0x34] = 0x16,
	[0x35] = 0x17,
	[0x36] = 0x12,
	/* IDE, SATA */
	[0x40] = 0x1F,
	[0x41] = 0x13,
	/* GPP0..GPP3 */
	[0x50] = 0x14,
	[0x51] = 0x15,
	[0x52] = 0x16,
	[0x53] = 0x17,
};

static const struct pirq_struct mainboard_pirq_data[] = {
	/* GPU bridge */
	{GFX_DEVFN, {PIRQ_A, PIRQ_B, PIRQ_NC, PIRQ_NC}},
	/* SATA */
	{SATA_DEVFN, {PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},
	/* USB OHCI/EHCI pairs */
	{OHCI1_DEVFN, {PIRQ_OHCI1, PIRQ_NC, PIRQ_NC, PIRQ_NC}},
	{EHCI1_DEVFN, {PIRQ_NC, PIRQ_EHCI1, PIRQ_NC, PIRQ_NC}},
	{OHCI2_DEVFN, {PIRQ_OHCI2, PIRQ_NC, PIRQ_NC, PIRQ_NC}},
	{EHCI2_DEVFN, {PIRQ_NC, PIRQ_EHCI2, PIRQ_NC, PIRQ_NC}},
	{OHCI3_DEVFN, {PIRQ_OHCI3, PIRQ_NC, PIRQ_NC, PIRQ_NC}},
	{EHCI3_DEVFN, {PIRQ_NC, PIRQ_EHCI3, PIRQ_NC, PIRQ_NC}},
	{OHCI4_DEVFN, {PIRQ_NC, PIRQ_NC, PIRQ_OHCI4, PIRQ_NC}},
	/* SMBus */
	{SMBUS_DEVFN, {PIRQ_SMBUS, PIRQ_NC, PIRQ_NC, PIRQ_NC}},
	/* IDE */
	{IDE_DEVFN, {PIRQ_NC, PIRQ_IDE, PIRQ_NC, PIRQ_NC}},
	/* HDA */
	{HDA_DEVFN, {PIRQ_HDA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},
	/* PCI bridge */
	{SB_PCI_PORT_DEVFN, {PIRQ_H, PIRQ_E, PIRQ_F, PIRQ_G}},
	/* SB PCIe GPP ports */
	{SB_PCIE_PORT1_DEVFN, {PIRQ_E, PIRQ_F, PIRQ_G, PIRQ_H}},
	{SB_PCIE_PORT2_DEVFN, {PIRQ_F, PIRQ_G, PIRQ_H, PIRQ_E}},
	{SB_PCIE_PORT3_DEVFN, {PIRQ_G, PIRQ_H, PIRQ_E, PIRQ_F}},
	{SB_PCIE_PORT4_DEVFN, {PIRQ_H, PIRQ_E, PIRQ_F, PIRQ_G}},
};

/* Sabertooth PCIe slot mux debugging functions.
 * TODO: Implement proper handling for different configurations,
 *       providing config options for different slot layouts.
 *       These functions are only needed for validating GPIO
 *       values based on different card layouts.
 */

/*
 * sb800_gpio_dump_all() - Dump every SB800 FCH GPIO and IOMUX register.
 *
 * Valid GPIO indices for SB800: 0–67 and 171–198.
 * Format: "GPIOxxx iomux=XX reg=XX level=H/L"
 */
static void sb800_gpio_dump_all(void)
{
	static const struct { u8 lo; u8 hi; } ranges[] = {
		{ 0,   67  },
		{ 171, 198 },
	};
	unsigned int r, n;

	printk(BIOS_DEBUG, "sabertooth: SB800 FCH GPIO dump:\n");
	printk(BIOS_DEBUG, "  idx  iomux  reg  lvl\n");
	for (r = 0; r < ARRAY_SIZE(ranges); r++) {
		for (n = ranges[r].lo; n <= ranges[r].hi; n++) {
			u8 iomux = FCH_IOMUX(n);
			u8 gpio  = FCH_GPIO(n);
			printk(BIOS_DEBUG, "  %3u  0x%02x  0x%02x   %c\n",
			       n, iomux, gpio, (gpio >> 7) ? 'H' : 'L');
		}
	}
}

/*
 * sb800_gpio_probe_slot_mux() - Log PCIe slot mux GPIO states at boot.
 *
 * The Sabertooth 990FX R2.0 uses hardware mux chips to dynamically route
 * RD890 PCIe lanes between physical slots based on card presence.  The mux
 * logic is controlled by three SB800 GPIOs:
 *
 *   GPIO59 (ball AE19, AGPIO59/IR_TX0 alt fn 2): 2X4_SENSE - input.
 *     Pulled LOW by the slot mux hardware when PCIEx16_4 is populated
 *     (active-low PRSNT# logic).  HIGH = empty, LOW = card present.
 *     Observed: L (reg 0x2A) with card, H (reg 0xAA) without.
 *
 *   GPIO61 (ball AB18, AGPIO61/SATA_IS0# alt fn 2): 2X16_SW - output.
 *   GPIO62 (ball AH21, AGPIO62/SATA_IS1# alt fn 2): 2X16_SW - output.
 *     The vendor BIOS drives both HIGH (reg 0xAA) in all configurations.
 *     (single GPU, 2-GPU, and fully populated).  LOW selects an alternate
 *     GPP2 lane topology for PCIEX16_3; Warns if LOW, shouldn't happen.
 *
 * Physical slot -> NB port mapping:
 *   PCIEX16_1 -> NB port  2 (dev 02.0, GPP1, x16 cap)
 *   PCIEX16_2 -> NB port 13 (dev 0d.0, GPP3b, x4 max)
 *   PCIEX16_3 -> NB port 11 (dev 0b.0, GPP2, x16 cap)
 *   PCIEX16_4 -> NB port 10 (dev 0a.0, GPP3a, x1)
 */
static void sb800_gpio_probe_slot_mux(void)
{
	u8 gpio59, gpio61, gpio62;
	u8 iomux59, iomux61, iomux62;

	/*
	 * Switches GPIO59/61/62 to GPIO function (iomux=2),
         * which allows reading current values.
	 * The power-on default is iomux=0, which makes
	 * FCH_GPIO() read ops unreliable.
         * The vendor BIOS uses iomux=2 for all
	 * three.
	 *
	 * GPIO61/62 (2X16_SW) control the GPP2 core lane mux.
         * They are set high in romstage to avoid SATA_IS0#/IS1# being driven,
	 * but we reconfigure them here make sure the register is set up
         * in the correct way so we can read and log.
	 * Value 0x2A (bits 5,3,1: pull-up enable, input-enable, output-config).
	 *
	 * GPIO59 (2X4_SENSE) is just an input.  Its iomux is switched to 2
	 * so the sense bit is readable; no write to FCH_GPIO(59) is
	 * needed as it's only used to tell which cards are inserted
         * so the correct switching can be performed.
         * TODO: Actually implement the switching.
	 */
	FCH_IOMUX(59) = 2;
	FCH_IOMUX(61) = 2;
	FCH_GPIO(61) = 0x2A;		/* 2X16_SW HIGH: pull-up */
	FCH_IOMUX(62) = 2;
	FCH_GPIO(62) = 0x2A;		/* 2X16_SW HIGH: pull-up */

	gpio59  = FCH_GPIO(59);
	gpio61  = FCH_GPIO(61);
	gpio62  = FCH_GPIO(62);
	iomux59 = FCH_IOMUX(59);
	iomux61 = FCH_IOMUX(61);
	iomux62 = FCH_IOMUX(62);

	printk(BIOS_DEBUG,
	       "sabertooth: PCIe slot mux GPIO state:\n"
	       "  GPIO59 (2X4_SENSE, AE19): iomux=0x%02x reg=0x%02x level=%s\n"
	       "  GPIO61 (2X16_SW,   AB18): iomux=0x%02x reg=0x%02x level=%s\n"
	       "  GPIO62 (2X16_SW,   AH21): iomux=0x%02x reg=0x%02x level=%s\n",
	       iomux59, gpio59, (gpio59 >> 7) ? "H" : "L",
	       iomux61, gpio61, (gpio61 >> 7) ? "H" : "L",
	       iomux62, gpio62, (gpio62 >> 7) ? "H" : "L");

	/*
	 * GPIO59: 2X4_SENSE - LOW means PCIEx16_4 slot is populated.
	 * The vendor BIOS also adjusts PIRQC/PIRQH routing when this is LOW
	 * (PIRQC<->PIRQH swap observed between populated and empty configs).
	 */
	if (!(gpio59 >> 7))
		printk(BIOS_INFO,
		       "sabertooth: GPIO59 2X4_SENSE=L -> PCIEx16_4 (NB port 10, dev 0a) populated\n");
	else
		printk(BIOS_INFO,
		       "sabertooth: GPIO59 2X4_SENSE=H -> PCIEx16_4 (NB port 10, dev 0a) empty\n");

	/*
	 * GPIO61/62: 2X16_SW - vendor BIOS drives both high in all observed
	 * configurations.  If low at coreboot boot, the GPP2 core (PCIEX16_3,
	 * NB port 11, dev 0b) may be in an alternate lane configuration not
	 * accounted for by the static devicetree; log a warning.
	 */
	if (!(gpio61 >> 7) || !(gpio62 >> 7))
		printk(BIOS_WARNING,
		       "sabertooth: 2X16_SW GPIO61=%s GPIO62=%s - UNEXPECTED LOW; "
		       "GPP2/PCIEX16_3 lane topology may differ from devicetree\n",
		       (gpio61 >> 7) ? "H" : "L",
		       (gpio62 >> 7) ? "H" : "L");
	else
		printk(BIOS_DEBUG,
		       "sabertooth: 2X16_SW=H (GPIO61=H GPIO62=H) - "
		       "GPP2 in normal mode, PCIEX16_3 routed to NB port 11\n");
}

static void pirq_setup(void)
{
	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = ARRAY_SIZE(mainboard_pirq_data);
	intr_data_ptr = mainboard_intr_data;
	picr_data_ptr = mainboard_picr_data;
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard ASUSTeK COMPUTER INC. SABERTOOTH 990FX R2.0 enabled.\n");

	sb800_gpio_dump_all();
	sb800_gpio_probe_slot_mux();

	/*
	 * GPP clock enable: write all-1s to SB_MISC_REG00 (ClkCntrl0, 32-bit)
	 * and SB_MISC_REG04 (ClkCntrl1, byte 0).  The CIMX SB power-on init
	 * already restores these to 0xFFFFFFFF/0xFF in romstage, but
	 * mainboard_enable runs before PCIe training (before rd890_enable is
	 * reached during scan_bus).  Writing 0x00 to bytes 2–3 of ClkCntrl0
	 * and to ClkCntrl1 before training disables the clock signal needed by
	 * the secondary x16 slot (GPP2, port 11) and specific GPP3a x1 slot
	 * (port 10) - those ports will end up in LTSSM=0x0 as a result.
         * This keeps all bits set to match the CIMX intent.
         * This is done in the mainboard directory instead of CIMx code as
         * this is potentially not useful on all boards with discrete NBs.
         * TODO: Fold into SB CIMx with compiler ifdefs on the RD890 being
         * used if it ends up applying to more boards.
	 */
	u8 *misc_base = (u8 *)(uintptr_t)(ACPI_MMIO_BASE + MISC_BASE);
	write8(misc_base + 0x00, 0xFF); /* ClkCntrl0 byte 0: GPP_CLK[0-3] */
	write8(misc_base + 0x01, 0xFF); /* ClkCntrl0 byte 1: GPP_CLK[4-7] */
	write8(misc_base + 0x02, 0xFF); /* ClkCntrl0 byte 2: keep enabled */
	write8(misc_base + 0x03, 0xFF); /* ClkCntrl0 byte 3: keep enabled */
	write8(misc_base + 0x04, 0xFF); /* ClkCntrl1 byte 0: keep enabled */

	/* ASF register init (suppress SMBus event storms) */
	pm_iowrite(0x29, 0x80);
	pm_iowrite(0x28, 0x61);

	pirq_setup();

	/*
	 * ELCR (Edge/Level Control Registers) - I/O ports 0x4D0 (primary PIC)
	 * and 0x4D1 (secondary PIC).  Each bit selects level-triggered (1) or
	 * edge-triggered (0) for the corresponding IRQ.
	 *
	 * SeaBIOS does not set the ELCR for AMD chipsets, so it remains at
	 * power-on default (all edge-triggered = 0x00/0x00).  PCI devices
	 * require level-triggered, active-low interrupts.  Without the correct
	 * ELCR, unserviced PCI interrupt lines cause IRQ storms or stuck
	 * assertion state on PIC/APIC lines that disrupt real-mode
         * bootloader INT 13h disk I/O. If IRQ lines end up triggering in level
         * triggered mode, and nothing handles the IRQ, it ties the line high
         * indefinitely, preventing other IRQs (such as edge-triggered or more
         * important level triggered IRQs) from working.
	 *
	 * IRQs 10 and 11 are used for PCI devices (OHCI/EHCI USB, SATA, PCIe).
	 * IRQs 0/1/2/8/13 must remain edge-triggered (timer, keyboard, cascade,
	 * RTC, FPU) so the take priority.  IRQ 7 is reserved as the spurious
         * interrupt vector and must NOT be used for any PCI device. Having a
         * dedicated spurious interrupt vector means unhandled IRQs won't
         * interfere with legitimate devices & their IRQ traffic.
	 */
	outb(0x00, 0x4D0);	/* primary PIC (IRQ0-7): all edge-triggered */
	outb(0x0C, 0x4D1);	/* secondary PIC (IRQ8-15): IRQ10 + IRQ11 level-triggered */
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("ASUS Sabertooth 990FX R2.0")
	.enable_dev = mainboard_enable,
};
