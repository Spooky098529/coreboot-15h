#include <arch/acpi.h>
DefinitionBlock ("DSDT.AML", "DSDT", 0x02, OEM_ID, ACPI_TABLE_CREATOR, 0x00010001)
{
	#include "acpi/mainboard.asl"

	#include <cpu/amd/agesa/family15/acpi/cpu.asl>

	#include "acpi/routing.asl"

	Scope(\_SB) {
		#include <arch/x86/acpi/globutil.asl>

		Device(PCI0) {
			#include <northbridge/amd/agesa/family15/acpi/northbridge.asl>

			#define RD890_GFX_PORT_0_ACTIVE
			#define RD890_GPP_PORT_0_ACTIVE
			#define RD890_GPP_PORT_1_ACTIVE
			#define RD890_GPP_PORT_2_ACTIVE
			#define RD890_GPP_PORT_3_ACTIVE
			#define RD890_GPP_PORT_4_ACTIVE
			#define RD890_GPP_PORT_5_ACTIVE
			#define RD890_GFX2_PORT_0_ACTIVE
			#define RD890_GPP2_PORT_0_ACTIVE
			#include <northbridge/amd/cimx/rd890/acpi/northbridge.asl>
			#include <southbridge/amd/cimx/sb800/acpi/fch.asl>
		}
	}

	#include <southbridge/amd/common/acpi/sleepstates.asl>
	#include "acpi/sleep.asl"
	#include "acpi/gpe.asl"
}
