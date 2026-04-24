/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_WINBOND_W83527HG_H
#define SUPERIO_WINBOND_W83527HG_H

#include <device/pnp_type.h>

#define W83527HG_KBC		5  /* PS/2 keyboard & mouse */
#define W83527HG_WDTO_PLED	8  /* WDTO#, PLED */
#define W83527HG_ACPI		10 /* ACPI */
#define W83527HG_HWM		11 /* Hardware monitor */
#define W83527HG_PECI		12 /* PECI */

/* The following are handled using "virtual LDNs" (hence the _V suffix). */
#define W83527HG_GPIO235_V	9  /* GPIO2, GPIO3, GPIO5 */

/*
 * Virtual devices sharing the enables are encoded as follows:
 *   VLDN = baseLDN[7:0] | [10:8] bitpos of enable in 0x30 of baseLDN
 */

#define W83527HG_GPIO2	((0 << 8) | W83527HG_GPIO235_V)
#define W83527HG_GPIO3	((1 << 8) | W83527HG_GPIO235_V)
#define W83527HG_GPIO5	((3 << 8) | W83527HG_GPIO235_V)

/* Note: There is no GPIO1 or GPIO4 on the W83527HG as per datasheet. */

#endif /* SUPERIO_WINBOND_W83527HG_H */
