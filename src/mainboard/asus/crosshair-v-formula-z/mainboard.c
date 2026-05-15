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
#include "ec.h"
#include "vrm_asp1000c.h"
#if CONFIG(BOARD_VRM_ASP0911)
#include "vrm_asp0911.h"
#endif
#if CONFIG(BOARD_VRM_DIAG)
#include "vrm_diag.h"
#endif

static const u8 mainboard_picr_data[FCH_INT_TABLE_SIZE] = {
	/*			PIRQ_A		PIRQ_B		PIRQ_C		PIRQ_D */
	[0x00] =		0x1F,		0x1F,		0x1F,		0x1F,
	/*			PIRQ_E		PIRQ_F		PIRQ_G		PIRQ_H */
	[0x04] =		0x1F,		0x1F,		0x1F,		0x1F,
	/*			PIRQ_MISC	PIRQ_MISC0	PIRQ_MISC1	PIRQ_MISC2 */
	[0x08] =		0xF0,		0xF1,		0x00,		0x00,
	/*			PIRQ_SIRQA	PIRQ_SIRQB	PIRQ_SIRQC	PIRQ_SIRQD */
	[0x0c] =		0x1F,		0x1F,		0x1F,		0x1F,
	/*			PIRQ_SCI	PIRQ_SMBUS	PIRQ_ASF	PIRQ_HDA */
	[0x10] =		0x09,		0x1F,		0x1F,		0x1F,
	/*			PIRQ_FC		PIRQ_GEC	PIRQ_PMON	NA */
	[0x14] =		0x1F,		0x1F,		0x1F,		0x00,
	/*			NA		NA		NA		NA */
	[0x18] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x1c] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_IMC0	PIRQ_IMC1	PIRQ_IMC2	PIRQ_IMC3 */
	[0x20] =		0x1F,		0x1F,		0x1F,		0x1F,
	/*			PIRQ_IMC4	PIRQ_IMC5	NA		NA */
	[0x24] =		0x1F,		0x1F,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x28] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x2c] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_OHCI1	PIRQ_EHCI1	PIRQ_OHCI2	PIRQ_EHCI2 */
	[0x30] =		0x1F,		0x1F,		0x1F,		0x1F,
	/*			PIRQ_OHCI3	PIRQ_EHCI3	PIRQ_OHCI4	NA */
	[0x34] =		0x1F,		0x1F,		0x1F,		0x00,
	/*			NA		NA		NA		NA */
	[0x38] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x3c] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_IDE	PIRQ_SATA	NA		NA */
	[0x40] =		0x1F,		0x1F,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x44] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x48] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x4c] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_GPP0	PIRQ_GPP1	PIRQ_GPP2	PIRQ_GPP3 */
	[0x50] =		0x1F,		0x1F,		0x1F,		0x1F,
};

static const u8 mainboard_intr_data[FCH_INT_TABLE_SIZE] = {
	/*			PIRQ_A		PIRQ_B		PIRQ_C		PIRQ_D */
	[0x00] =		0x10,		0x11,		0x12,		0x13,
	/*			PIRQ_E		PIRQ_F		PIRQ_G		PIRQ_H */
	[0x04] =		0x14,		0x15,		0x16,		0x17,
	/*			PIRQ_MISC	PIRQ_MISC0	PIRQ_MISC1	PIRQ_MISC2 */
	[0x08] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_SIRQA	PIRQ_SIRQB	PIRQ_SIRQC	PIRQ_SIRQD */
	[0x0c] =		0x1F,		0x1F,		0x1F,		0x1F,
	/*			PIRQ_SCI	PIRQ_SMBUS	PIRQ_ASF	PIRQ_HDA */
	[0x10] =		0x09,		0x1F,		0x1F,		0x10,
	/*			PIRQ_FC		PIRQ_GEC	PIRQ_PMON	NA */
	[0x14] =		0x1F,		0x12,		0x1F,		0x00,
	/*			NA		NA		NA		NA */
	[0x18] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x1c] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_IMC0	PIRQ_IMC1	PIRQ_IMC2	PIRQ_IMC3 */
	[0x20] =		0x1F,		0x1F,		0x1F,		0x1F,
	/*			PIRQ_IMC4	PIRQ_IMC5	NA		NA */
	[0x24] =		0x1F,		0x1F,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x28] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x2c] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_OHCI1	PIRQ_EHCI1	PIRQ_OHCI2	PIRQ_EHCI2 */
	[0x30] =		0x12,		0x11,		0x14,		0x15,
	/*			PIRQ_OHCI3	PIRQ_EHCI3	PIRQ_OHCI4	NA */
	[0x34] =		0x16,		0x17,		0x12,		0x00,
	/*			NA		NA		NA		NA */
	[0x38] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x3c] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_IDE	PIRQ_SATA	NA		NA */
	[0x40] =		0x11,		0x13,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x44] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x48] =		0x00,		0x00,		0x00,		0x00,
	/*			NA		NA		NA		NA */
	[0x4c] =		0x00,		0x00,		0x00,		0x00,
	/*			PIRQ_GPP0	PIRQ_GPP1	PIRQ_GPP2	PIRQ_GPP3 */
	[0x50] =		0x14,		0x15,		0x16,		0x17,
};

static const struct pirq_struct mainboard_pirq_data[] = {
	/* RD890 */
	{PCI_DEVFN(0x02,0), {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},		// 2.0 PCIE_X16_1
	{PCI_DEVFN(0x03,0), {PIRQ_E, PIRQ_F, PIRQ_G, PIRQ_H}},		// 3.0 PCIE_X16_1 (bifurcated)
	{PCI_DEVFN(0x04,0), {PIRQ_E, PIRQ_F, PIRQ_G, PIRQ_G}},		// 4.0 ASMedia ASM1061 SATA
	{PCI_DEVFN(0x05,0), {PIRQ_G, PIRQ_H, PIRQ_E, PIRQ_F}},		// 5.0 Intel GBE
	{PCI_DEVFN(0x06,0), {PIRQ_D, PIRQ_A, PIRQ_B, PIRQ_C}},		// 6.0 ASMedia ASM1042A USB3
	{PCI_DEVFN(0x07,0), {PIRQ_C, PIRQ_D, PIRQ_A, PIRQ_B}},		// 7.0 ASMedia ASM1042A USB3
	{PCI_DEVFN(0x09,0), {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},		// 9.0 ASMedia ASM1042A USB3
	{PCI_DEVFN(0x0A,0), {PIRQ_H, PIRQ_E, PIRQ_F, PIRQ_G}},		// a.0 ASMedia ASM1061 SATA
	{PCI_DEVFN(0x0B,0), {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},		// b.0 PCIE_X16/X8_3
	{PCI_DEVFN(0x0C,0), {PIRQ_E, PIRQ_F, PIRQ_G, PIRQ_H}},		// c.0 PCIE_X8_2
	{PCI_DEVFN(0x0D,0), {PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D}},		// d.0 PCIE_x4_4
	/* SB800 */
	{PCI_DEVFN(0x11,0), {PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	// 11.0 SB SATA
	{PCI_DEVFN(0x12,0), {PIRQ_OHCI1, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	// 12.0 SB USB OHCI0
	{PCI_DEVFN(0x12,2), {PIRQ_NC, PIRQ_EHCI1, PIRQ_NC, PIRQ_NC}},	// 12.2 SB USB EHCI
	{PCI_DEVFN(0x13,0), {PIRQ_OHCI2, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	// 13.0 SB USB OHCI0
	{PCI_DEVFN(0x13,2), {PIRQ_NC, PIRQ_EHCI2, PIRQ_NC, PIRQ_NC}},	// 13.2 SB USB EHCI
	{PCI_DEVFN(0x14,2), {PIRQ_HDA, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	// 14.2 SB HDA
	{PCI_DEVFN(0x14,4), {PIRQ_E, PIRQ_F, PIRQ_G, PIRQ_H}},		// 14.4 SB PCI
	{PCI_DEVFN(0x14,5), {PIRQ_NC, PIRQ_NC, PIRQ_OHCI4, PIRQ_NC}},	// 14.5 SB USB OHCI2
	{PCI_DEVFN(0x15,0), {PIRQ_E, PIRQ_F, PIRQ_G, PIRQ_H}},		// 15.0 PCIEX1_1
	{PCI_DEVFN(0x15,1), {PIRQ_F, PIRQ_G, PIRQ_H, PIRQ_E}},		// 15.1 PCIEX1_2
	{PCI_DEVFN(0x16,0), {PIRQ_OHCI3, PIRQ_NC, PIRQ_NC, PIRQ_NC}},	// 16.0 SB USB OHCI0
	{PCI_DEVFN(0x16,2), {PIRQ_NC, PIRQ_EHCI3, PIRQ_NC, PIRQ_NC}},	// 16.2 SB USB EHCI
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
 *     HIGH keeps GPP2 in the default lane config; LOW selects an
 *     alternate topology for PCIEX16_3 (not supported here).
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
	 * Switch GPIO59/61/62 to GPIO function (iomux=2) so FCH_GPIO() reads
	 * are reliable; power-on default iomux=0 leaves them on alternate fns.
	 * GPIO61/62 (2X16_SW) are set HIGH to keep GPP2 in the default lane
	 * config; LOW would select an alternate topology for PCIEX16_3.
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
	 */
	if (!(gpio59 >> 7))
		printk(BIOS_INFO,
		       "sabertooth: GPIO59 2X4_SENSE=L -> PCIEx16_4 (NB port 10, dev 0a) populated\n");
	else
		printk(BIOS_INFO,
		       "sabertooth: GPIO59 2X4_SENSE=H -> PCIEx16_4 (NB port 10, dev 0a) empty\n");

	/*
	 * GPIO61/62: 2X16_SW - HIGH keeps GPP2 in the default lane config.
	 * If LOW at boot, the GPP2 core (PCIEX16_3, NB port 11) may be in
	 * an alternate lane config not accounted for in the devicetree.
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

/* ASUS board-specific SATA PHY calibration for ASM1061 */
static void asm1061_phy_calibrate(struct device *dev)
{
	u32 val;

	val = pci_read_config32(dev, 0xEC);
	pci_write_config32(dev, 0xEC, val | (1 << 2));

	pci_write_config32(dev, 0x2C, 0x84B71043);
	pci_write_config8(dev, 0x0A, 0x06);

	val = pci_read_config32(dev, 0xEC);
	pci_write_config32(dev, 0xEC, val & ~(1 << 2));

	printk(BIOS_INFO, "ASM1061 %s: SATA PHY calibration applied\n",
	       dev_path(dev));
}

/* two-phase indirect PHY register write via PCI config 0xE0/0xF8/0xFC */
static void asm1042_phy_write(struct device *dev, u32 phy_reg, u32 val)
{
	u32 bdf = ((u32)dev->bus->secondary << 8) | dev->path.pci.devfn;

	pci_write_config32(dev, 0xE0, 0x02);
	pci_write_config32(dev, 0xF8, bdf);
	pci_write_config32(dev, 0xFC, phy_reg);
	pci_write_config32(dev, 0xE0, 0x02);

	pci_write_config32(dev, 0xF8, val);
	pci_write_config32(dev, 0xFC, 0x00);
	pci_write_config32(dev, 0xE0, 0x02);
}

/* board-specific xHCI PHY tuning for ASM1042; power-on defaults cause marginal signal integrity */
static void asm1042_phy_tune(struct device *dev)
{
	asm1042_phy_write(dev, 0xF390, 0x1043);
	asm1042_phy_write(dev, 0xF392, 0x8488);

	printk(BIOS_INFO, "ASM1042 %s: xHCI PHY tuning applied\n",
	       dev_path(dev));
}

static void mainboard_final(void *chip_info)
{
	struct device *dev;

	dev = NULL;
	while ((dev = dev_find_device(0x1b21, 0x0612, dev)) != NULL)
		asm1061_phy_calibrate(dev);

	dev = NULL;
	while ((dev = dev_find_device(0x1b21, 0x1042, dev)) != NULL)
		asm1042_phy_tune(dev);
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
	printk(BIOS_INFO, "Mainboard ASUS Crosshair V Formula-Z enabled.\n");

	if(0) {
		sb800_gpio_dump_all();
		sb800_gpio_probe_slot_mux();
	}

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
	outb(0x98, 0x4D0);	/* primary PIC (IRQ0-7): IRQs 3,4,7 = Level-triggered */
	outb(0xCE, 0x4D1);	/* secondary PIC (IRQ8-15): IRQs 9,10,11,14,15 = Level-triggered */

#if CONFIG(BOARD_VRM_REPORT)
	{
		int vcore_mv;
		if (asp1000c_get_vcore_mv(&vcore_mv) == 0)
			printk(BIOS_INFO,
			       "VRM: CPU VCore = %d mV (ramstage)\n", vcore_mv);
	}
#endif

#if CONFIG(BOARD_VRM_ASP0911)
	asp0911_report();
#endif

#if CONFIG(BOARD_VRM_DIAG)
	vrm_pmbus_diag();
#endif
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("ASUS Crosshair V Formula-Z")
	.enable_dev = mainboard_enable,
	.final      = mainboard_final,
};
