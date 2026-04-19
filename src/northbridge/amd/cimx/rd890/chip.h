/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#ifndef _CIMX_RD890_CHIP_H_
#define _CIMX_RD890_CHIP_H_

/**
 * RD890 specific device configuration
 */
struct northbridge_amd_cimx_rd890_config
{
	u8 index;
	u8 gpp1_configuration;
	u8 gpp2_configuration;
	u8 gpp3a_configuration;
	u16 port_enable;
	u8 port_aspm;	/* Per-NB ASPM mode for all trained ports:
			 * 0 = Disabled
			 * 1 = L0s only
			 * 2 = L1 only
			 * 3 = L0s and L1
			 * 4 = L0s downstream only
			 * 5 = L0s downstream only and L1
			 */
};

#endif /* _CIMX_RD890_CHIP_H_ */
