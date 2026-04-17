/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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
 * Supermicro H8SCM SuperIO (ITE8718) and board-specific PCI0 devices.
 *
 * Include this file inside Device(PCI0) scope, after the southbridge
 * fch.asl include.
 */

/* PCI slot 1, 2, 3 */
Device(PIBR) {
	Name(_ADR, 0x00140004)
	Name(_PRW, Package() {0x18, 4})

	Method(_PRT, 0) {
		Return (PCIB)
	}
}

/* Primary (and only) IDE channel */
Device(IDEC) {
	Name(_ADR, 0x00140001)
	#include "acpi/ide.asl"
} /* end IDEC */

/* ITE8718 Support */
OperationRegion (IOID, SystemIO, 0x2E, 0x02)	/* sometimes it is 0x4E */
	Field (IOID, ByteAcc, NoLock, Preserve)
	{
		SIOI,   8,    SIOD,   8		/* 0x2E and 0x2F */
	}

IndexField (SIOI, SIOD, ByteAcc, NoLock, Preserve)
{
		Offset (0x07),
	LDN,	8,	/* Logical Device Number */
		Offset (0x20),
	CID1,	8,	/* Chip ID Byte 1, 0x87 */
	CID2,	8,	/* Chip ID Byte 2, 0x12 */
		Offset (0x30),
	ACTR,	8,	/* Function activate */
		Offset (0xF0),
	APC0,	8,	/* APC/PME Event Enable Register */
	APC1,	8,	/* APC/PME Status Register */
	APC2,	8,	/* APC/PME Control Register 1 */
	APC3,	8,	/* Environment Controller Special Configuration Register */
	APC4,	8	/* APC/PME Control Register 2 */
}

/* Enter the 8718 MB PnP Mode */
Method (EPNP)
{
	Store(0x87, SIOI)
	Store(0x01, SIOI)
	Store(0x55, SIOI)
	Store(0x55, SIOI) /* 8718 magic number */
}
/* Exit the 8718 MB PnP Mode */
Method (XPNP)
{
	Store (0x02, SIOI)
	Store (0x02, SIOD)
}
/*
 * Keyboard PME is routed to SB700 Gevent3. We can wake
 * up the system by pressing the key.
 */
Method (SIOS, 1)
{
	/* We only enable KBD PME for S5. */
	If (LLess (Arg0, 0x05))
	{
		EPNP()
		/* DBGO("8718F\n") */

		Store (0x4, LDN)
		Store (One, ACTR)  /* Enable EC */
		/*
		Store (0x4, LDN)
		Store (0x04, APC4)
		*/  /* falling edge. which mode? Not sure. */

		Store (0x4, LDN)
		Store (0x08, APC1) /* clear PME status, Use 0x18 for mouse & KBD */
		Store (0x4, LDN)
		Store (0x08, APC0) /* enable PME, Use 0x18 for mouse & KBD */

		XPNP()
	}
}
Method (SIOW, 1)
{
	EPNP()
	Store (0x4, LDN)
	Store (Zero, APC0) /* disable keyboard PME */
	Store (0x4, LDN)
	Store (0xFF, APC1) /* clear keyboard PME status */
	XPNP()
}
