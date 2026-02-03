/*
 * OemCustomize.c
 *
 * On fam15h discrete NB systems with RD890-compatible northbridge (like 990FX)
 * PCIe configuration is handled completely by the devicetree.cb. Other CPU
 * families and APU systems serve as a better reference for configuring slot & lane
 * PCIe mappings under AGESA OemCustomize.c
 */

#include <AGESA.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <PlatformMemoryConfiguration.h>

CONST PSO_ENTRY ROMDATA DefaultPlatformMemoryConfiguration[] = {
	NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, ANY_CHANNEL, 2), PSO_END};

const struct OEM_HOOK OemCustomize = {};
