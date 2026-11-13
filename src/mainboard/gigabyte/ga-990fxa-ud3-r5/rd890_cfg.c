/*
 * This file is part of the coreboot project.
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

#include <console/console.h>
#include "NbPlatform.h"

/*
 * Northbridge IOAPIC at 0xFEC20000, as in the vendor MADT (ID 0x0A, GSI
 * base 24). The shared rd890 code uses IO_APIC_ADDR (0xFEC00000) for NB 0,
 * which is also the SB950 IOAPIC: two IOAPICs decoding the same address,
 * rd890_ht_init()'s setup_ioapic() rewriting the SB IOAPIC, and the IVRS
 * fixup (which looks IOAPIC IDs up by address in the MADT) giving both the
 * same ID. The MADT and MP table read this address back from the chip.
 */
#define UD3R5_NB_IOAPIC_ADDR	0xFEC20000

void mainboard_rd890_override_cfg(AMD_NB_CONFIG_BLOCK *gConfigPtr)
{
	gConfigPtr->Northbridges[0].pNbConfig->IoApicBaseAddress = UD3R5_NB_IOAPIC_ADDR;
	printk(BIOS_INFO, "RD890: NB IOAPIC at 0x%x\n", UD3R5_NB_IOAPIC_ADDR);
}
