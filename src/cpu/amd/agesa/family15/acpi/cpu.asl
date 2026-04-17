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
 * Processor Objects for AMD Family 15h (Bulldozer/Piledriver).
 *
 * Covers single-socket platforms (e.g. AM3+ FX, up to 8 cores, C000-C007)
 * and dual-socket G34 platforms (e.g. Opteron 6200/6300 series, up to 16
 * cores per socket, C000-C02F).  48 entries are defined to match the
 * maximum APIC ID range used by AGESA on dual-socket Interlagos systems.
 *
 * Each processor Device uses the Generic Processor HID (ACPI0007) as
 * required by ACPI 4.0+.  UIDs are assigned sequentially and correspond
 * to the APIC IDs assigned by AGESA.
 */
Scope (\_PR) {

	Device (C000) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x00)
	}

	Device (C001) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x01)
	}

	Device (C002) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x02)
	}

	Device (C003) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x03)
	}

	Device (C004) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x04)
	}

	Device (C005) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x05)
	}

	Device (C006) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x06)
	}

	Device (C007) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x07)
	}

	/* C008-C00F: second die / second socket die 0 (8-core single socket
	 * or socket 0 die 1 / socket 1 die 0 on dual-socket G34) */
	Device (C008) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x08)
	}

	Device (C009) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x09)
	}

	Device (C00A) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x0A)
	}

	Device (C00B) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x0B)
	}

	Device (C00C) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x0C)
	}

	Device (C00D) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x0D)
	}

	Device (C00E) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x0E)
	}

	Device (C00F) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x0F)
	}

	/* C010-C01F: socket 1 cores 0-15 (dual-socket G34) */
	Device (C010) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x10)
	}

	Device (C011) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x11)
	}

	Device (C012) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x12)
	}

	Device (C013) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x13)
	}

	Device (C014) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x14)
	}

	Device (C015) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x15)
	}

	Device (C016) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x16)
	}

	Device (C017) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x17)
	}

	Device (C018) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x18)
	}

	Device (C019) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x19)
	}

	Device (C01A) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x1A)
	}

	Device (C01B) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x1B)
	}

	Device (C01C) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x1C)
	}

	Device (C01D) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x1D)
	}

	Device (C01E) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x1E)
	}

	Device (C01F) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x1F)
	}

	/* C020-C02F: additional APIC ID headroom for AGESA dual-socket G34
	 * configurations where APIC IDs are not assigned contiguously */
	Device (C020) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x20)
	}

	Device (C021) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x21)
	}

	Device (C022) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x22)
	}

	Device (C023) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x23)
	}

	Device (C024) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x24)
	}

	Device (C025) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x25)
	}

	Device (C026) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x26)
	}

	Device (C027) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x27)
	}

	Device (C028) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x28)
	}

	Device (C029) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x29)
	}

	Device (C02A) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x2A)
	}

	Device (C02B) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x2B)
	}

	Device (C02C) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x2C)
	}

	Device (C02D) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x2D)
	}

	Device (C02E) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x2E)
	}

	Device (C02F) {
	Name(_HID, "ACPI0007")
	Name(_UID, 0x2F)
	}
} /* End _PR scope */
