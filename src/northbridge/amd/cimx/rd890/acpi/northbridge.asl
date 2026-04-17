/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2026 the coreboot project
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

/*
 * AMD RD890 / 990FX CIMx northbridge PCIe root-port ACPI.
 *
 * Defines the PCIe root-port bridge devices managed by the CIMx RD890
 * northbridge driver.  Include this file inside a Device(PCI0) block in
 * \_SB scope, after the AGESA family15 northbridge include and before the
 * southbridge include.
 *
 * Each bridge references a pair of interrupt-routing packages (PIC / APIC
 * mode) that must be provided by the board's routing.asl.  The package
 * names below are consistent across all known RD890 boards:
 *
 *   PS2 / APS2 - downstream routing for GFX port 0  (dev 0x02, x16)
 *   PS4 / APS4 - downstream routing for GPP port 0  (dev 0x04)
 *   PS5 / APS5 - downstream routing for GPP port 1  (dev 0x05)
 *                AM3+ boards only; G34 boards omit dev 5 connections
 *   PS9 / APS9 - downstream routing for GPP port 4  (dev 0x09)
 *                AM3+ boards only; G34 boards omit dev 9 connections
 *   PSa / APSa - downstream routing for GPP port 5  (dev 0x0a)
 *                AM3+ boards only; G34 boards omit dev a connections
 *   PSb / APSb - downstream routing for GFX2 / PCIe (dev 0x0b)
 *   PSc / APSc - downstream routing for GFX2 / PCIe (dev 0x0c)
 *   PSd / APSd - downstream routing for GPP2 port 0 (dev 0x0d)
 *
 * Optional RD890 ports not connected on all boards (dev 0x03 GFX port 1,
 * dev 0x06-0x07 GPP ports 2-3) are not enumerated here; boards that use
 * them should add the bridge devices and matching routing packages in their
 * board-level ACPI.
 *
 * AM3+-only ports (dev 5, 9, a) require RD890_GPP_PORT_{1,4,5}_ACTIVE
 * and matching PSx/APSx routing packages in the board's routing.asl.
 */

/* Dev 2 - GFX port 0 (primary x16 graphics slot on some NBs) */
Device(PBR2) {
	Name(_ADR, 0x00020000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD) { Return(APS2) }	/* APIC mode */
		Return (PS2)			/* PIC Mode */
	} /* end _PRT */
} /* end PBR2 */

/* Dev 4 - GPP port 0 */
Device(PBR4) {
	Name(_ADR, 0x00040000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD) { Return(APS4) }	/* APIC mode */
		Return (PS4)			/* PIC Mode */
	} /* end _PRT */
} /* end PBR4 */

/* Dev 5 - GPP port 1 (AM3+ boards only; define PS5/APS5 in routing.asl) */
#ifdef RD890_GPP_PORT_1_ACTIVE
Device(PBR5) {
	Name(_ADR, 0x00050000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD) { Return(APS5) }	/* APIC mode */
		Return (PS5)			/* PIC Mode */
	} /* end _PRT */
} /* end PBR5 */
#endif

/* Dev 9 - GPP port 4 (AM3+ boards only; define PS9/APS9 in routing.asl) */
#ifdef RD890_GPP_PORT_4_ACTIVE
Device(PBR9) {
	Name(_ADR, 0x00090000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD) { Return(APS9) }	/* APIC mode */
		Return (PS9)			/* PIC Mode */
	} /* end _PRT */
} /* end PBR9 */
#endif

/* Dev a - GPP port 5 (AM3+ boards only; define PSa/APSa in routing.asl) */
#ifdef RD890_GPP_PORT_5_ACTIVE
Device(PBRa) {
	Name(_ADR, 0x000A0000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD) { Return(APSa) }	/* APIC mode */
		Return (PSa)			/* PIC Mode */
	} /* end _PRT */
} /* end PBRa */
#endif

/* Dev b - second x16 graphics / PCIe slot */
Device(PBRb) {
	Name(_ADR, 0x000B0000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD) { Return(APSb) }	/* APIC mode */
		Return (PSb)			/* PIC Mode */
	} /* end _PRT */
} /* end PBRb */

/* Dev c - second x16 graphics port 1 / PCIe slot */
Device(PBRc) {
	Name(_ADR, 0x000C0000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD) { Return(APSc) }	/* APIC mode */
		Return (PSc)			/* PIC Mode */
	} /* end _PRT */
} /* end PBRc */

/* Dev d - GPP2 port 0 */
Device(PBRd) {
	Name(_ADR, 0x000D0000)
	Name(_PRW, Package() {0x18, 4})
	Method(_PRT,0) {
		If(PMOD) { Return(APSd) }	/* APIC mode */
		Return (PSd)			/* PIC Mode */
	} /* end _PRT */
} /* end PBRd */
