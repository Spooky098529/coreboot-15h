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

/*
 * ACPI - create the Fixed ACPI Description Tables (FADT)
 *
 * Shared FADT for all CIMx SB700/SB710/SB750/SP5100 boards.
 */

#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <device/device.h>
#include <version.h>
#include "Platform.h"

void acpi_create_fadt(acpi_fadt_t *fadt, acpi_facs_t *facs, void *dsdt)
{
	acpi_header_t *header = &(fadt->header);

	printk(BIOS_DEBUG, "ACPI_BLK_BASE: 0x%04x\n", PM1_EVT_BLK_ADDRESS);

	memset((void *)fadt, 0, sizeof(acpi_fadt_t));
	memcpy(header->signature, "FACP", 4);
	header->length = sizeof(acpi_fadt_t);
	header->revision = get_acpi_table_revision(FADT);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);
	header->asl_compiler_revision = asl_revision;

	fadt->firmware_ctrl = (uintptr_t)facs;
	fadt->dsdt = (uintptr_t)dsdt;

	/* 3 = Workstation, 4 = Enterprise Server, 7 = Performance Server */
	fadt->preferred_pm_profile = 0x03;
	fadt->sci_int = 9;
	/* SMM disabled in coreboot */
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0xf0;
	fadt->acpi_disable = 0xf1;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0;	/* smi_cmd=0, no SMI needed for P-state */

	outl(0x1, PM1_CNT_BLK_ADDRESS);	/* set SCI_EN */
	fadt->pm1a_evt_blk = PM1_EVT_BLK_ADDRESS;
	fadt->pm1b_evt_blk = 0x0000;
	fadt->pm1a_cnt_blk = PM1_CNT_BLK_ADDRESS;
	fadt->pm1b_cnt_blk = 0x0000;
	fadt->pm2_cnt_blk = ACPI_PMA_CNT_BLK_ADDRESS;
	fadt->pm_tmr_blk = PM1_TMR_BLK_ADDRESS;
	fadt->gpe0_blk = GPE0_BLK_ADDRESS;
	fadt->gpe1_blk = 0x0000;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 8;
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;

	fadt->cst_cnt = 0;	/* smi_cmd=0, no SMI needed for C-state */
	fadt->p_lvl2_lat = 101;	/* > 100: C2 not supported via legacy P_LVL2 */
	fadt->p_lvl3_lat = 1001;	/* > 1000: C3 not supported via legacy P_LVL3 */
	fadt->flush_size = 0;
	fadt->flush_stride = 0;
	fadt->duty_offset = 1;
	fadt->duty_width = 3;
	fadt->day_alrm = 0;
	fadt->mon_alrm = 0;
	fadt->century = 0;
	fadt->iapc_boot_arch = 0x3;
	/* WBINVD | PROC_C1 | P_LVL2_UP | RTC_S4 | TMR_VAL_EXT | RESET_REG_SUP | HEADLESS */
	fadt->flags = 0x0001c1ad;

	fadt->res2 = 0;

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.access_size = 0;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0x0;

	fadt->reset_value = 6;
	fadt->x_firmware_ctl_l = ((uintptr_t)facs) & 0xffffffff;
	fadt->x_firmware_ctl_h = ((uint64_t)(uintptr_t)facs) >> 32;
	fadt->x_dsdt_l = ((uintptr_t)dsdt) & 0xffffffff;
	fadt->x_dsdt_h = ((uint64_t)(uintptr_t)dsdt) >> 32;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = 0;
	fadt->x_pm1a_evt_blk.addrl = PM1_EVT_BLK_ADDRESS;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = 1;
	fadt->x_pm1b_evt_blk.bit_width = 4;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.access_size = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = 0;
	fadt->x_pm1a_cnt_blk.addrl = PM1_CNT_BLK_ADDRESS;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = 1;
	fadt->x_pm1b_cnt_blk.bit_width = 2;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.access_size = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = 1;
	fadt->x_pm2_cnt_blk.bit_width = 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = 0;
	fadt->x_pm2_cnt_blk.addrl = ACPI_PMA_CNT_BLK_ADDRESS;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = 0;
	fadt->x_pm_tmr_blk.addrl = PM1_TMR_BLK_ADDRESS;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 64;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = 0;
	fadt->x_gpe0_blk.addrl = GPE0_BLK_ADDRESS;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.access_size = 0;
	fadt->x_gpe1_blk.addrl = 0;
	fadt->x_gpe1_blk.addrh = 0x0;

	header->checksum = acpi_checksum((void *)fadt, sizeof(acpi_fadt_t));
}
