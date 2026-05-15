#include <southbridge/amd/cimx/sb800/gpio_oem.h>
#include <console/console.h>
#include "NbPlatform.h"

void mainboard_rd890_override_cfg(AMD_NB_CONFIG_BLOCK *gConfigPtr) {
	if(FCH_GPIO(61) >> 7) {
		printk(BIOS_INFO, "RD890: PCIE_X8_2 = OFF\n");
		gConfigPtr->Northbridges[0].pPcieConfig->PortConfiguration[12].PortPresent = OFF;
		gConfigPtr->Northbridges[0].pPcieConfig->PortConfiguration[12].PortLinkMode = 0;
		printk(BIOS_INFO, "RD890: PCIE_X16/X8_3 = x16\n");
		gConfigPtr->Northbridges[0].pPcieConfig->CoreConfiguration[1] = GFX_CONFIG_AAAA;
	} else {
		printk(BIOS_INFO, "RD890: PCIE_X8_2 = x8\n");
		printk(BIOS_INFO, "RD890: PCIE_X16/X8_3 = x8\n");
	}
};
