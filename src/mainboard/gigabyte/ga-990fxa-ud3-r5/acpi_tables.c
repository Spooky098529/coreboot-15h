/*
 * acpi_tables.c - MADT filler
 *
 * Timer and SCI IRQ routing (ISA -> IOAPIC GSI):
 *   IRQ 0 (PIT/i8254)  -> GSI 0  (no override; SB800 connects PIT to IOAPIC pin 0)
 *   IRQ 2 (cascade)    -> not routed (consumed by 8259 cascade function)
 *   IRQ 8 (RTC)        -> GSI 8  (identity, edge, active-high)
 *   IRQ 9 (SCI)        -> GSI 9  (MADT override, level, active-low)
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/ioapic.h>
#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ids.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	struct device *dev;
	u32 dword;
	u32 gsi_base = 0;
	u32 apicid_sb;
	u32 apicid_nb;

	/*
	 * AGESA v5 APIC ID assignment:
	 * For systems with >= 16 APICs: IO-APICs at 0..n, local at m..z
	 * For systems with <  16 APICs: local at 0..n, IO-APICs at n+1
	 */
	if (CONFIG_MAX_CPUS >= 16)
		apicid_sb = 0x0;
	else
		apicid_sb = CONFIG_MAX_CPUS + 1;
	apicid_nb = apicid_sb + 1;

	/* Create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* SB I/O APIC (always present at IO_APIC_ADDR = 0xFEC00000) */
	current += acpi_create_madt_ioapic(
		(acpi_madt_ioapic_t *)current,
		apicid_sb,
		IO_APIC_ADDR,
		0 /* GSI base = 0 */
	);

	/* NB I/O APIC - address from NBMISCIND[1] at PCI 00:00.0 */
	gsi_base += IO_APIC_INTERRUPTS; /* SB has 24 IOAPIC entries */
	dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (dev) {
		pci_write_config32(dev, 0xF8, 0x1);
		dword = pci_read_config32(dev, 0xFC) & 0xfffffff0;
		current += acpi_create_madt_ioapic(
			(acpi_madt_ioapic_t *)current,
			apicid_nb,
			dword,
			gsi_base
		);
	}

	/* Interrupt source overrides */
	/*
	 * No IRQ0 override: the SB800 connects the 8254 PIT to IOAPIC pin 0
	 * (GSI 0), which is the default ISA mapping.  An older IRQ0->GSI2
	 * override (from a vendor MADT probe) caused "MP-BIOS bug: 8254 timer
	 * not connected to IO-APIC" under linux, because the kernel checked
         * and found that GSI2 is not actually receiving PIT ticks with that
         * override - instead of doing additional configuration to match the
         * vendor board, we just use default AGESA behaviour, which seems to
         * work OK.
	 */
	current += acpi_create_madt_irqoverride(
		(acpi_madt_irqoverride_t *)current,
		0,     /* bus: ISA */
		9,    /* source: ISA IRQ 9 */
		9,    /* GSI */
		0x000F /* flags */
	);

	/*
	 * LAPIC NMI - vendor MADT has a single entry for processor 0xFF
	 * (all processors), LINT1, active-high edge (flags=0x0005).
         * Much cleaner and requires less per-board configuration to set up
         * NMI using this mask. Untested on dual-socket boards.
	 */
	current += acpi_create_madt_lapic_nmi(
		(acpi_madt_lapic_nmi_t *)current, 0xFF, 5, 1);

	return current;
}
