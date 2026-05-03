/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>
#include <pc80/keyboard.h>

#include "w83527hg.h"

static void w83527hg_init(struct device *dev)
{
	uint8_t byte;
	uint8_t power_status;
	const char *power_setting;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case W83527HG_KBC:
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case W83527HG_ACPI:
		power_status = (CONFIG_MAINBOARD_POWER_FAILURE_STATE & 0x3);
		pnp_enter_conf_mode_8787(dev);
		pnp_set_logical_device(dev);
		byte = pnp_read_config(dev, 0xe4);
		byte &= ~0x60;
		if(power_status) {
			byte |= power_status << 5;
		}
		pnp_write_config(dev, 0xe4, byte);
		pnp_exit_conf_mode_aa(dev);
		switch(power_status) {
			case 0x00:
				power_setting = "OFF";
				break;
			case 0x01:
				power_setting = "ON";
				break;
			case 0x02:
				power_setting = "LAST";
				break;
			default:
				power_setting = "INVALID";
		}
		printk(BIOS_INFO, "default power state set to %s\n", power_setting);
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
