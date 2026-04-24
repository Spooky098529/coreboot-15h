/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>

#include "w83527hg.h"

static void w83527hg_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case W83527HG_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = w83527hg_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, W83527HG_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x07ff, 0x07ff, },
	{ NULL, W83527HG_WDTO_PLED, },
	{ NULL, W83527HG_GPIO2, },
	{ NULL, W83527HG_GPIO3, },
	{ NULL, W83527HG_GPIO5, },
	{ NULL, W83527HG_ACPI, PNP_IRQ0, },
	{ NULL, W83527HG_HWM, PNP_IO0 | PNP_IRQ0, 0x07fe, },
	{ NULL, W83527HG_PECI, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_winbond_w83527hg_ops = {
	CHIP_NAME("Winbond W83527HG Super I/O")
	.enable_dev = enable_dev,
};
