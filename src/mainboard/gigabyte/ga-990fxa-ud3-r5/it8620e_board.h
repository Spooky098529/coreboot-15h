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

#ifndef GA_990FXA_UD3_R5_IT8620E_BOARD_H
#define GA_990FXA_UD3_R5_IT8620E_BOARD_H

#include <types.h>

/* Gigabyte DualBIOS boot stages written to IT8620E LDN7 CR EF bits 6:4 */
#define DUALBIOS_STAGE_PEI	6	/* vendor DualBiosPei: romstage */
#define DUALBIOS_STAGE_DXE	7	/* vendor DualBiosDxe: ramstage */

/* Re-arm the EC's LDN7 watchdog to its maximum timeout (romstage). */
void it8620e_rearm_wdt_max(void);

/* Write the DualBIOS handshake stage to LDN7 CR EF (romstage + ramstage). */
void it8620e_dualbios_stage(u8 stage);

/* Drive GP17 high like the vendor BIOS (PS/2 power); ramstage, after LPC decode. */
void it8620e_gp17_high(void);

/* Program the vendor fan registers into the EC at @ec_base (ramstage). */
void it8620e_program_vendor_fans(u16 ec_base);

#endif
