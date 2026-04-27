/*
 * acpi/routing.asl -- PCI interrupt routing
 *
 * May need further tweaks to ensure all devices avoid polling fallbacks.
 *
 * PR0  = PIC mode  (uses LNKA..LNKH link devices)
 * APR0 = APIC mode (direct GSI numbers, SB IOAPIC)
 */

Scope(\_SB) {

	Name(PR0, Package(){
		/* Bus 0 -- root bus PIC routing */

		/* Bus 0, Dev 0x02 -- GFX port 0 (primary GPU) */
		Package(){0x0002FFFF, 0, INTE, 0 },

		/* Bus 0, Dev 0x03 -- GFX port 1 (x8+x8 split) */
		/* disabled */

		/* Bus 0, Dev 0x04 -- GPP port 0 */
		Package(){0x0004FFFF, 0, INTE, 0 },

		/* Bus 0, Dev 0x05 -- GPP port 1 */
		Package(){0x0005FFFF, 0, INTE, 0 },

		/* Bus 0, Dev 0x06 -- GPP port 2 */
		/* disabled */

		/* Bus 0, Dev 0x07 -- GPP port 3 */
		/* disabled */

		/* Bus 0, Dev 0x09 -- GPP port 4 */
		Package(){0x0009FFFF, 0, INTF, 0 },

		/* Bus 0, Dev 0x0a -- GPP port 5 */
		Package(){0x000AFFFF, 0, INTG, 0 },

		/* Bus 0, Dev 0x0b -- GFX2 port 0 (990FX secondary x16, PCIE16_2) */
		Package(){0x000BFFFF, 0, INTG, 0 },

		/* Bus 0, Dev 0x0c -- GFX2 port 1 (x8+x8 split only; no lanes in AAAA mode) */
		/* disabled */

		/* Bus 0, Dev 0x0d -- GPP2 port 0 (GPP3b core x4, PCIE16_3) */
		Package(){0x000DFFFF, 0, INTG, 0 },

		/* SB devices */
		/* Bus 0, Dev 0x11 -- SATA: INTA->PIRQD */
		Package(){0x0011FFFF, 0, INTD, 0 },

		/* Bus 0, Dev 0x12 -- USB0: INTA->PIRQC INTB->PIRQB */
		Package(){0x0012FFFF, 0, INTC, 0 },
		Package(){0x0012FFFF, 1, INTB, 0 },
		/* Bus 0, Dev 0x13 -- USB1: INTA->PIRQE INTB->PIRQF */
		Package(){0x0013FFFF, 0, INTE, 0 },
		Package(){0x0013FFFF, 1, INTF, 0 },
		/* Bus 0, Dev 0x16 -- USB3: INTA->PIRQG INTB->PIRQH */
		Package(){0x0016FFFF, 0, INTG, 0 },
		Package(){0x0016FFFF, 1, INTH, 0 },

		/* Bus 0, Dev 0x14 -- LPC/SMBus/HDA: INTA->PIRQA INTB->PIRQB */
		Package(){0x0014FFFF, 0, INTA, 0 },
		Package(){0x0014FFFF, 1, INTB, 0 },
		Package(){0x0014FFFF, 2, INTC, 0 },
		Package(){0x0014FFFF, 3, INTD, 0 },

		/* Bus 0, Dev 0x15 -- SB PCIe ports: INTA->PIRQE..INTD->PIRQH */
		Package(){0x0015FFFF, 0, INTE, 0 },
		Package(){0x0015FFFF, 1, INTF, 0 },
		Package(){0x0015FFFF, 2, INTG, 0 },
		Package(){0x0015FFFF, 3, INTH, 0 },
	})

	Name(APR0, Package(){
		/* Bus 0 -- root bus APIC routing (direct GSI numbers) */

		/* Bus 0, Dev 0x02 -- GFX port 0 */
		Package(){0x0002FFFF, 0, 0, 52 },

		/* Bus 0, Dev 0x04 -- GPP port 0 */
		Package(){0x0004FFFF, 0, 0, 52 },

		/* Bus 0, Dev 0x05 -- GPP port 1 */
		Package(){0x0005FFFF, 0, 0, 52 },

		/* Bus 0, Dev 0x09 -- GPP port 4 */
		Package(){0x0009FFFF, 0, 0, 53 },

		/* Bus 0, Dev 0x0a -- GPP port 5 */
		Package(){0x000AFFFF, 0, 0, 54 },

		/* Bus 0, Dev 0x0b -- GFX2 port 0 */
		Package(){0x000BFFFF, 0, 0, 54 },

		/* Bus 0, Dev 0x0d -- GPP2 port 0 */
		Package(){0x000DFFFF, 0, 0, 54 },

		/* SB devices in APIC mode */
		/* SATA dev 0x11: INTA->GSI 19 (PIRQD) */
		Package(){0x0011FFFF, 0, 0, 19 },
		/* USB0 dev 0x12: INTA->GSI 18 (PIRQC), INTB->GSI 17 (PIRQB) */
		Package(){0x0012FFFF, 0, 0, 18 },
		Package(){0x0012FFFF, 1, 0, 17 },
		/* USB1 dev 0x13: INTA->GSI 20 (PIRQE), INTB->GSI 21 (PIRQF) */
		Package(){0x0013FFFF, 0, 0, 20 },
		Package(){0x0013FFFF, 1, 0, 21 },
		/* USB3 dev 0x16: INTA->GSI 22 (PIRQG), INTB->GSI 23 (PIRQH) */
		Package(){0x0016FFFF, 0, 0, 22 },
		Package(){0x0016FFFF, 1, 0, 23 },
		/* SMBus/LPC/HDA dev 0x14: INTA->GSI 16 (PIRQA)..INTD->GSI 19 (PIRQD) */
		Package(){0x0014FFFF, 0, 0, 16 },
		Package(){0x0014FFFF, 1, 0, 17 },
		Package(){0x0014FFFF, 2, 0, 18 },
		Package(){0x0014FFFF, 3, 0, 19 },
		/* SB PCIe ports dev 0x15: INTA->GSI 20 (PIRQE)..INTD->GSI 23 (PIRQH) */
		Package(){0x0015FFFF, 0, 0, 20 },
		Package(){0x0015FFFF, 1, 0, 21 },
		Package(){0x0015FFFF, 2, 0, 22 },
		Package(){0x0015FFFF, 3, 0, 23 },
	})

	/* Downstream bus from NB dev 0x02 */
	Name(PS2, Package(){
		Package(){0x0000FFFF, 0, INTE, 0 },
		Package(){0x0000FFFF, 1, INTF, 0 },
		Package(){0x0000FFFF, 2, INTG, 0 },
		Package(){0x0000FFFF, 3, INTH, 0 },
	})
	Name(APS2, Package(){
		Package(){0x0000FFFF, 0, 0, 20 },
		Package(){0x0000FFFF, 1, 0, 21 },
		Package(){0x0000FFFF, 2, 0, 22 },
		Package(){0x0000FFFF, 3, 0, 23 },
	})

	/* Downstream bus from NB dev 0x04 */
	Name(PS4, Package(){
		Package(){0x0000FFFF, 0, INTE, 0 },
		Package(){0x0000FFFF, 1, INTF, 0 },
		Package(){0x0000FFFF, 2, INTG, 0 },
		Package(){0x0000FFFF, 3, INTH, 0 },
	})
	Name(APS4, Package(){
		Package(){0x0000FFFF, 0, 0, 20 },
		Package(){0x0000FFFF, 1, 0, 21 },
		Package(){0x0000FFFF, 2, 0, 22 },
		Package(){0x0000FFFF, 3, 0, 23 },
	})

	/* Downstream bus from NB dev 0x05 */
	Name(PS5, Package(){
		Package(){0x0000FFFF, 0, INTE, 0 },
		Package(){0x0000FFFF, 1, INTF, 0 },
		Package(){0x0000FFFF, 2, INTG, 0 },
		Package(){0x0000FFFF, 3, INTH, 0 },
	})
	Name(APS5, Package(){
		Package(){0x0000FFFF, 0, 0, 20 },
		Package(){0x0000FFFF, 1, 0, 21 },
		Package(){0x0000FFFF, 2, 0, 22 },
		Package(){0x0000FFFF, 3, 0, 23 },
	})

	/* Downstream bus from NB dev 0x06 */
	Name(PS6, Package(){
		Package(){0x0000FFFF, 0, INTE, 0 },
		Package(){0x0000FFFF, 1, INTF, 0 },
		Package(){0x0000FFFF, 2, INTG, 0 },
		Package(){0x0000FFFF, 3, INTH, 0 },
	})
	Name(APS6, Package(){
		Package(){0x0000FFFF, 0, 0, 20 },
		Package(){0x0000FFFF, 1, 0, 21 },
		Package(){0x0000FFFF, 2, 0, 22 },
		Package(){0x0000FFFF, 3, 0, 23 },
	})

	/* Downstream bus from NB dev 0x07 */
	Name(PS7, Package(){
		Package(){0x0000FFFF, 0, INTE, 0 },
		Package(){0x0000FFFF, 1, INTF, 0 },
		Package(){0x0000FFFF, 2, INTG, 0 },
		Package(){0x0000FFFF, 3, INTH, 0 },
	})
	Name(APS7, Package(){
		Package(){0x0000FFFF, 0, 0, 20 },
		Package(){0x0000FFFF, 1, 0, 21 },
		Package(){0x0000FFFF, 2, 0, 22 },
		Package(){0x0000FFFF, 3, 0, 23 },
	})

	/* Downstream bus from NB dev 0x09 */
	Name(PS9, Package(){
		Package(){0x0000FFFF, 0, INTF, 0 },
		Package(){0x0000FFFF, 1, INTG, 0 },
		Package(){0x0000FFFF, 2, INTH, 0 },
		Package(){0x0000FFFF, 3, INTE, 0 },
	})
	Name(APS9, Package(){
		Package(){0x0000FFFF, 0, 0, 21 },
		Package(){0x0000FFFF, 1, 0, 22 },
		Package(){0x0000FFFF, 2, 0, 23 },
		Package(){0x0000FFFF, 3, 0, 20 },
	})

	/* Downstream bus from NB dev 0x0a */
	Name(PSa, Package(){
		Package(){0x0000FFFF, 0, INTG, 0 },
		Package(){0x0000FFFF, 1, INTH, 0 },
		Package(){0x0000FFFF, 2, INTE, 0 },
		Package(){0x0000FFFF, 3, INTF, 0 },
	})
	Name(APSa, Package(){
		Package(){0x0000FFFF, 0, 0, 22 },
		Package(){0x0000FFFF, 1, 0, 23 },
		Package(){0x0000FFFF, 2, 0, 20 },
		Package(){0x0000FFFF, 3, 0, 21 },
	})

	/* Downstream bus from NB dev 0x0b (GFX2 port 0 / second x16 slot) */
	Name(PSb, Package(){
		Package(){0x0000FFFF, 0, INTG, 0 },
		Package(){0x0000FFFF, 1, INTH, 0 },
		Package(){0x0000FFFF, 2, INTE, 0 },
		Package(){0x0000FFFF, 3, INTF, 0 },
	})
	Name(APSb, Package(){
		Package(){0x0000FFFF, 0, 0, 22 },
		Package(){0x0000FFFF, 1, 0, 23 },
		Package(){0x0000FFFF, 2, 0, 20 },
		Package(){0x0000FFFF, 3, 0, 21 },
	})

	/* Downstream bus from NB dev 0x0c (GFX2 port 1 - no lanes in AAAA mode) */
	Name(PSc, Package(){
		Package(){0x0000FFFF, 0, INTH, 0 },
		Package(){0x0000FFFF, 1, INTE, 0 },
		Package(){0x0000FFFF, 2, INTF, 0 },
		Package(){0x0000FFFF, 3, INTG, 0 },
	})
	Name(APSc, Package(){
		Package(){0x0000FFFF, 0, 0, 23 },
		Package(){0x0000FFFF, 1, 0, 20 },
		Package(){0x0000FFFF, 2, 0, 21 },
		Package(){0x0000FFFF, 3, 0, 22 },
	})

	/* Downstream bus from NB dev 0x0d (GPP3b core, PCIE16_3 x4) */
	Name(PSd, Package(){
		Package(){0x0000FFFF, 0, INTG, 0 },
		Package(){0x0000FFFF, 1, INTH, 0 },
		Package(){0x0000FFFF, 2, INTE, 0 },
		Package(){0x0000FFFF, 3, INTF, 0 },
	})
	Name(APSd, Package(){
		Package(){0x0000FFFF, 0, 0, 22 },
		Package(){0x0000FFFF, 1, 0, 23 },
		Package(){0x0000FFFF, 2, 0, 20 },
		Package(){0x0000FFFF, 3, 0, 21 },
	})

	/* Legacy PCI bridge (dev 0x14, func 4) downstream devices */
	Name(PCIB, Package(){
		Package(){0x0005FFFF, 0, 0, 0x14 },
		Package(){0x0005FFFF, 1, 0, 0x15 },
		Package(){0x0005FFFF, 2, 0, 0x16 },
		Package(){0x0005FFFF, 3, 0, 0x17 },
		Package(){0x0006FFFF, 0, 0, 0x15 },
		Package(){0x0006FFFF, 1, 0, 0x16 },
		Package(){0x0006FFFF, 2, 0, 0x17 },
		Package(){0x0006FFFF, 3, 0, 0x14 },
		Package(){0x0007FFFF, 0, 0, 0x16 },
		Package(){0x0007FFFF, 1, 0, 0x17 },
		Package(){0x0007FFFF, 2, 0, 0x14 },
		Package(){0x0007FFFF, 3, 0, 0x15 },
	})

}  /* Scope(\_SB) */
