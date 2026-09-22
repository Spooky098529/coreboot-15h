/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>
#include <superio/ite/common/env_ctrl.h>

#include "chip.h"
#include "it8620e.h"

static void it8620e_init(struct device *dev)
{
	const struct superio_ite_it8620e_config *conf = dev->chip_info;
	const struct resource *res;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	/* TODO: Might potentially need code for FDC etc. */
	case IT8620E_EC:
		res = find_resource(dev, PNP_IDX_IO0);
		if (!conf || !res)
			break;
		ite_ec_init(res->base, &conf->ec);
		break;
	case IT8620E_KBCK:
		set_kbc_ps2_mode();
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	}
}

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = it8620e_init,
	.ops_pnp_mode     = &pnp_conf_mode_870155_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, IT8620E_FDC, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ff8, },
	{ NULL, IT8620E_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, IT8620E_SP2, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, IT8620E_PP, PNP_IO0 | PNP_IRQ0 | PNP_DRQ0, 0x0ffc, },
	{ NULL, IT8620E_EC, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x0ff8, 0x0ff8, },
	{ NULL, IT8620E_KBCK, PNP_IO0 | PNP_IO1 | PNP_IRQ0, 0x0fff, 0x0fff, },
	{ NULL, IT8620E_KBCM, PNP_IRQ0, },
	{ NULL, IT8620E_GPIO, PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IRQ0,
		0x0fff, 0x0ff8, 0x0ff8, },
	{ NULL, IT8620E_IR, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_ite_it8620e_ops = {
	CHIP_NAME("ITE IT8620E Super I/O")
	.enable_dev = enable_dev
};
