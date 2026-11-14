/*
 * acpi/gpe.asl -- General Purpose Event handlers
 *
 */

Scope(\_GPE) {

	/* Gevent0 - ExtEvent0 SCI - Legacy PM Event */
	Method(_L08) { }

	/*
	 * Gevent1 (GPE 9, "Temp warning"): no handler, like the vendor DSDT.
	 * Its source stays asserted on this board; with an empty _L09 Linux
	 * enabled it and it fired ~40 times a second. Without a handler Linux
	 * leaves it disabled.
	 */

	/* Gevent3 - USB PME event */
	Method(_L0B) {
		Notify(\_SB.PCI0, 0x02)  /* device wake */
	}

	/* ExtEvent0 SCI */
	Method(_L10) { }

	/* ExtEvent1 SCI */
	Method(_L11) { }

	/* GPIO0/GEvent8 - PCIe wake */
	Method(_L18) {
		Notify(\_SB.PCI0, 0x02)  /* device wake */
	}

	/* Azalia SCI */
	Method(_L1B) {
		Notify(\_SB.PCI0.AZHD, 0x02)  /* device wake */
	}

	#include "usb_oc.asl"

}  /* \GPE */
