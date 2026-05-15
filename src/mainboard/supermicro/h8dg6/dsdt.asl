/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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

DefinitionBlock (
	"DSDT.AML",	/* Output filename */
	"DSDT",		/* Signature */
	0x02,		/* DSDT Revision, needs to be 2 for 64bit */
	"AMD   ",	/* OEMID */
	"COREBOOT",	/* TABLE ID */
	0x00010001	/* OEM Revision */
	)
{
	#include <arch/acpi.h>
	/* #include <arch/x86/acpi/debug.asl> */

	/* Board-specific global data and _PIC method */
	#include "acpi/mainboard.asl"

	/* Processor objects */
	#include <cpu/amd/agesa/family15/acpi/cpu.asl>

	/* PCI interrupt routing tables */
	#include "acpi/routing.asl"

	Scope(\_SB) {
		#include <arch/x86/acpi/globutil.asl>

		/* PCI interrupt link devices */
		#include <southbridge/amd/cimx/sb700/acpi/pci_int.asl>

		Device(PCI0) {
			/* AGESA family 15h northbridge */
			#include <northbridge/amd/agesa/family15/acpi/northbridge.asl>

			/* RD890 PCIe root ports */
			#define RD890_GFX_PORT_0_ACTIVE
			#define RD890_GPP_PORT_0_ACTIVE
			#define RD890_GFX2_PORT_0_ACTIVE
			#define RD890_GFX2_PORT_1_ACTIVE
			#define RD890_GPP2_PORT_0_ACTIVE
			#include <northbridge/amd/cimx/rd890/acpi/northbridge.asl>

			/* SB700 CIMx southbridge */
			#include <southbridge/amd/cimx/sb700/acpi/fch.asl>

			/* Board-specific: SuperIO, UARTs */
			#include "acpi/superio.asl"
		} /* End Device(PCI0) */

		Device(PWRB) {
			Name(_HID, EISAID("PNP0C0C"))
			Name(_UID, 0xAA)
			Name(_PRW, Package () {3, 0x04})	/* wake from S1-S4 */
			Name(_STA, 0x0B) /* present, enabled, not shown in UI */
		}
	} /* End \_SB scope */

	#include <southbridge/amd/common/acpi/sleepstates.asl>

	/* Sleep/wake methods */
	#include <southbridge/amd/cimx/sb700/acpi/sleep.asl>

	/* GPE handlers */
	#include <southbridge/amd/cimx/sb700/acpi/gpe.asl>

	/* Board-specific USB overcurrent GPE handlers */
	#include "acpi/usb.asl"

	/* SMBus ACPI support */
	#include <southbridge/amd/cimx/sb800/acpi/smbus.asl>
}
/* End of ASL file */
