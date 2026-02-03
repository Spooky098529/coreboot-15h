/*
 * mptable.c - MP specification interrupt routing table
 *
 * ISA timer IRQs (identity-mapped, edge-triggered, active-high):
 *   IRQ 0 -> IOAPIC pin 0  (PIT/i8254 or HPET legacy)
 *   IRQ 8 -> IOAPIC pin 8  (RTC or HPET legacy)
 *
 * SeaBIOS BUILD_MAX_MPTABLE_FSEG is by default currently 600 bytes.
 * The table includes a 16-byte floating pointer + the config table
 * so the config table body must stay under 584 bytes.
 * Contributing to this are:
 *   - 8 processor entries          = 160 bytes (20 each)
 *   - ~10 bus entries              =  80 bytes (8 each)
 *   - 16 ISA interrupt entries     = 128 bytes (8 each, from mpspec.c)
 *   - SB PCI interrupt entries     =  96 bytes (12 x 8)
 *   - 2 IOAPIC + 2 local + header  =  76 bytes
 *   Subtotal                       = 540 bytes (leaves a snug ~44 bytes free)
 *
 * NB PCIe interrupt routing is intentionally omitted from this table.
 * The KGPE-D16 (SR5650, same NB IOAPIC topology) and all other
 * reference boards with dual IOAPICs follow the same pattern: register
 * the NB IOAPIC so the OS discovers it, but avoid writing per-port
 * interrupt source entries.  The OS obtains NB PCIe interrupt routing
 * from ACPI (_PRT methods in the DSDT) rather than the MP table.
 * Including NB PCIe entries here would push the table over the 600-byte
 * SeaBIOS fseg copy limit, causing SeaBIOS to skip the entire table,
 * and it's not necessary for boot IO, se delegate it to ACPI.
 */

#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <arch/io.h>
#include <string.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;
	u32 apicid_nb;
	u32 apicid_sb;
	struct device *dev;
	u32 *dword;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);
	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);
	mptable_write_buses(mc, NULL, &bus_isa);

	/*
	 * AGESA APIC ID assignment rule:
	 * CPU local APICs get IDs 0x10..0x17 (8 cores on Family 15h FX CPUs).
         * This logic may need tuning for dual-socket boards or higher core fam15h.
	 * IO-APICs use IDs below the LAPIC range to avoid conflicts:
	 *   apicid_sb = CONFIG_MAX_CPUS + 1 = 9  (SB800/SB950 IO-APIC, 0xFEC00000)
	 *   apicid_nb = apicid_sb + 1       = 10 (RD890 IO-APIC, read from NBMISCIND[1])
	 * These must match the MADT IO-APIC entries in acpi_tables.c.
	 *
	 * CONFIG_MAX_CPUS must be 8 (actual core count on FX-8350) so that
	 * apicid_sb = 9 = 0x09, which is below the LAPIC ID range 0x10-0x17.
	 * With MAX_CPUS = 16, apicid_sb = 17 conflicts with the LAPIC range and
	 * causes AMD-Vi interrupt remapping failures (timer panic on Ubuntu/Linux).
         * Given 8 cores is the most we can get on a 990FX system, this is OK, but
         * additional logic would be needed if we wanted to use this on G34/C32 boards.
	 */
	apicid_sb = CONFIG_MAX_CPUS + 1;  /* 9 (0x09): matches MADT SB IO-APIC ID */
	apicid_nb = apicid_sb + 1;        /* 10 (0x0A): matches MADT NB IO-APIC ID */

	/* SB IOAPIC - read base address from LPC PCI config reg 0x74 */
	dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	if (dev) {
		/* Set NB IOAPIC address via NBMISCIND[1] */
		struct device *nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
		if (nb_dev) {
			pci_write_config32(nb_dev, 0xF8, 0x1);
			dword = (u32 *)(uintptr_t)(pci_read_config32(nb_dev, 0xFC) & 0xfffffff0);
			smp_write_ioapic(mc, apicid_nb, 0x20, dword);
		}

		/* SB IOAPIC address from LPC config reg 0x74 */
		dword = (u32 *)(uintptr_t)(pci_read_config32(dev, 0x74) & 0xfffffff0);
		smp_write_ioapic(mc, apicid_sb, 0x20, dword);

		/* PCI interrupt entries - level-triggered, active-low */
#define PCI_INT(bus, dev, int_sign, pin) \
		smp_write_intsrc(mc, mp_INT, \
			MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_LOW, \
			(bus), (((dev) << 2) | (int_sign)), apicid_sb, (pin))

		/* HD Audio (dev 0x14, func 2) */
		PCI_INT(0x0, 0x14, 0x2, 0x10);

		/* USB controllers - OHCI INTA->0x10, EHCI INTB->0x11 */
		PCI_INT(0x0, 0x12, 0x0, 0x10); /* OHCI */
		PCI_INT(0x0, 0x12, 0x2, 0x11); /* EHCI */
		PCI_INT(0x0, 0x13, 0x0, 0x10); /* OHCI */
		PCI_INT(0x0, 0x13, 0x2, 0x11); /* EHCI */
		PCI_INT(0x0, 0x14, 0x0, 0x10); /* OHCI */
		PCI_INT(0x0, 0x16, 0x0, 0x10); /* OHCI */
		PCI_INT(0x0, 0x16, 0x2, 0x11); /* EHCI */

		/* SATA (dev 0x11) - INTA->PIRQD (pin 0x13 for GSI 19) */
		PCI_INT(0x0, 0x11, 0x0, 0x13);

		/* SB PCIe ports (dev 0x15) */
		PCI_INT(0x0, 0x15, 0x0, 0x14);
		PCI_INT(0x0, 0x15, 0x1, 0x15);
		PCI_INT(0x0, 0x15, 0x2, 0x16);
		PCI_INT(0x0, 0x15, 0x3, 0x17);

	}

#undef PCI_INT

	/*
	 * ISA interrupts (via SB IOAPIC, bus_isa)
	 *
	 * mptable_add_isa_interrupts() generates all 16 ISA IRQ
	 * entries (IRQ 0-15) as identity-mapped, edge-triggered,
	 * active-high to the SB IOAPIC.  Key timer IRQs:
	 *   IRQ 0 -> IOAPIC pin 0 (PIT timer, important for SeaBIOS)
	 *   IRQ 2 -> IOAPIC pin 2 (cascade, not usable)
	 *   IRQ 8 -> IOAPIC pin 8 (RTC periodic)
	 *
	 * Note: ACPI MADT overrides (IRQ 0->GSI 2 for timer)
	 * take precedence at the OS level over MP table mappings
         * to avoid issues with clock drivers in Linux.
	 */
#define IO_LOCAL_INT(type, intr, apicid, pin) \
	smp_write_lintsrc(mc, (type), \
		MP_IRQ_TRIGGER_EDGE | MP_IRQ_POLARITY_HIGH, \
		bus_isa, (intr), (apicid), (pin))

	mptable_add_isa_interrupts(mc, bus_isa, apicid_sb, 0);

	/* Local INTs */
	IO_LOCAL_INT(mp_ExtINT, 0, MP_APIC_ALL, 0x0);
	IO_LOCAL_INT(mp_NMI,    0, MP_APIC_ALL, 0x1);

	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
