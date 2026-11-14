/*
 * mainboard.c - Mainboard-specific initialization, Gigabyte GA-990FXA-UD3 R5
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pnp.h>
#include <southbridge/amd/common/amd_pci_util.h>
#include <southbridge/amd/cimx/cimx_util.h>
#include <southbridge/amd/cimx/sb800/SBPLATFORM.h>
#include <southbridge/amd/cimx/sb800/gpio_oem.h>
#include <southbridge/amd/cimx/sb800/pci_devs.h>
#include <northbridge/amd/agesa/family15/pci_devs.h>
#include <superio/ite/it8620e/it8620e.h>
#include "it8620e_board.h"

/* Set to 1 to dump all SB800 GPIO/IOMUX registers at boot (debug). */
#define UD3R5_GPIO_DUMP 0

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
	[0x05] = 0x0A,	/* PIRQF: IRQ 10 */
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
	[0x50] = 0x0B,	/* GPP0 (Realtek RTL8111 LAN on 15.0) -> IRQ 11 */
	[0x51] = 0x0A,	/* GPP1 -> IRQ 10 */
	[0x52] = 0x0B,	/* GPP2 -> IRQ 11 */
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

#if UD3R5_GPIO_DUMP
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

	printk(BIOS_DEBUG, "ud3r5: SB800 FCH GPIO dump:\n");
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

#endif

static void pirq_setup(void)
{
	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = ARRAY_SIZE(mainboard_pirq_data);
	intr_data_ptr = mainboard_intr_data;
	picr_data_ptr = mainboard_picr_data;
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard GIGABYTE GA-990FXA-UD3 R5 enabled.\n");

#if UD3R5_GPIO_DUMP
	sb800_gpio_dump_all();
#endif

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

	/*
	 * LPC decode for the IT8620E EC/HWM at 0x228-0x237 (index/data at
	 * 0x22D/0x22E). The vendor BIOS decodes it through LPC reg 0x44 bit 8
	 * (IO 0x220-0x233; vendor reg 0x44 = 0xFF03FFD5). The SB800 LPC driver
	 * only sets that bit for a child resource at exactly 0x220, and the
	 * "wide IO 0" slot it would use for 0x228 never gets its base written,
	 * so without this the EC (and the fan setup) would be unreachable.
	 * lpc_enable_childrens_resources() only ORs bits in, so this sticks.
	 */
	struct device *lpc = dev_find_slot(0, PCI_DEVFN(0x14, 3));
	if (lpc)
		pci_write_config32(lpc, 0x44,
				   pci_read_config32(lpc, 0x44) | (1 << 8));

	/* GP17 high, as the vendor BIOS does; uses simple I/O at 0x220 (above). */
	it8620e_gp17_high();

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

/*
 * WORKAROUND for a bug in shared code: southbridge/amd/cimx/sb800/late.c
 * (sb800_init(), "A13+ errata" block) sets PMIO 0xC1 bit 2 as the SB900
 * "RPR 2.14: Enable delayed SLP_S3/S5". On SB800/SB950 that bit (PM_Reg C0
 * bit 10) is UserRst2EcEn instead: "Set to 1 to route user reset event to
 * Ec. The bit should be set to 1 when IMC is enabled."
 * (vendorcode/amd/cimx/sb800/SBCMN.c). The IMC is off on this board.
 *
 * With the bit set, the reset button only partly resets the southbridge
 * (its USB controllers and 00:14.6 keep their state from the previous
 * boot), and a reset pressed ~20 min or more after power-on made the
 * IT8620E EC treat the boot as failed and start the DualBIOS recovery,
 * which overwrites coreboot. Clearing it (the vendor BIOS's value, PMIO
 * C1 = 0x0b) gives a full reset; both problems are gone on hardware.
 *
 * Must run after sb800_init(). Drop this once late.c only sets the bit
 * when the IMC is enabled; other SB800/SB950 boards are likely affected too.
 */
#define SB_PMIO_C1	((void *)(uintptr_t)(0xfed80000 + 0x300 + 0xc1))
static void sb950_user_reset_to_sb(void)
{
	u8 before = read8(SB_PMIO_C1);

	write8(SB_PMIO_C1, before & ~(1 << 2));
	printk(BIOS_INFO, "SB950 PMIO C1 (UserRst2EcEn = bit2): %02x -> %02x\n",
	       before, read8(SB_PMIO_C1));
}

/*
 * Runs from dev_finalize_chips(), i.e. after device init. The fan setup has
 * to come after the IT8620E chip driver's ite_ec_init() (device init),
 * which clears EC reg 0x13 and rewrites 0x14/0x50; programming it from
 * mainboard_enable() would be overwritten.
 */
static void mainboard_final(void *chip_info)
{
	sb950_user_reset_to_sb();

	const struct device *ec = dev_find_slot_pnp(0x2e, IT8620E_EC);
	const struct resource *res = ec ? find_resource(ec, PNP_IDX_IO0) : NULL;

	if (ec && ec->enabled && res)
		it8620e_program_vendor_fans(res->base);
	else
		printk(BIOS_ERR, "IT8620E EC (LDN4) not enabled - fans NOT programmed!\n");

	/* DualBIOS: report the ramstage ("DXE") stage, as DualBiosDxe does. */
	it8620e_dualbios_stage(DUALBIOS_STAGE_DXE);

	/* Then turn the watchdog off, or a reset-button press triggers recovery. */
	it8620e_disarm_wdt();
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("GIGABYTE GA-990FXA-UD3 R5")
	.enable_dev = mainboard_enable,
	.final      = mainboard_final,
};
