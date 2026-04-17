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
 *
 * AMD Family 15h AGESA northbridge ACPI.
 *
 * Defines the PCI root bridge identity and the CPU-side devices that are
 * managed by the AGESA family15 northbridge driver: the HyperTransport
 * root (AMRT) and the Northbridge Miscellaneous Control function (NBF3)
 * which exposes the k10temp hardware thermal sensor.
 *
 * Include this file inside a Device(PCI0) block in \_SB scope, before
 * any discrete I/O hub northbridge (e.g. RD890) or southbridge includes.
 *
 * _BBN, _STA, _PRT, _CRS, _INI, and _OSC come from the southbridge
 * fch.asl include. TOM1/TOM2 are written at runtime by AGESA's
 * northbridge_fill_ssdt_generator().
 */
External (TOM1)
External (TOM2)
Name(_HID, EISAID("PNP0A08"))	/* PCI Express Root Bridge */
Name(_CID, EISAID("PNP0A03"))	/* PCI Root Bridge */
Name(_ADR, 0x00180000)		/* Dev# = BSP Dev#, Func# = 0 */

/* HyperTransport root / IOMMU */
Device(AMRT) {
	Name(_ADR, 0x00000000)
} /* end AMRT */

/* Northbridge function 3 - Miscellaneous Control (k10temp thermal sensor) */
Device(NBF3) {
	Name(_ADR, 0x00180003)

	/* Family 15h hardware thermal sensor (k10temp) */
	#include <northbridge/amd/amdfam10/thermal_mixin.asl>
} /* end NBF3 */
