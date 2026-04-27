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
	u8 port_aspm;	/* Default ASPM mode for trained ports on this NB:
			 *   0 = use CONFIG_RD890_PCIE_ASPM_MODE
			 *   1 = L0s only
			 *   2 = L1 only
			 *   3 = L0s and L1
			 *   4 = L0s downstream only
			 *   5 = L0s downstream only and L1
			 * Use port_aspm_disable to override individual ports
			 * to disabled (mode 0) while keeping the default
			 * for the rest.
			 */
	u32 port_aspm_disable;	/* Bitmask, 1 bit per port_id (2..13).
				 * If bit N is set, ASPM is forced disabled
				 * on port N regardless of port_aspm/Kconfig.
				 * Use this for endpoints with known ASPM
				 * incompatibility (e.g. ASMedia ASM1042
				 * USB3 controllers, which corrupt DMA when
				 * the link enters L1).
				 */
	u32 port_gen1;		/* Bitmask, 1 bit per port_id (2..13).
				 * If bit N is set, PortLinkMode is forced to
				 * PcieLinkModeGen1 on port N, overriding the
				 * default of Gen2.  Use this for endpoints
				 * that report PCIe correctable errors at Gen2
				 * speeds due to marginal board trace signal
				 * integrity (e.g. ASMedia ASM1042 USB3 on
				 * boards with long PCIe traces).
				 */
};

#endif /* _CIMX_RD890_CHIP_H_ */
