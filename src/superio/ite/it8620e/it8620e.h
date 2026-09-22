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

#ifndef SUPERIO_ITE_IT8620E_H
#define SUPERIO_ITE_IT8620E_H

/*
 * Chip ID 0x8620 rev 0x04 at port 0x2E. LDN 0x04 (EC) and LDN 0x07 (GPIO)
 * match the common ITE EC/GPIO layout (see superio/ite/common/env_ctrl.h);
 * LDN 0x0A is the PME/CIR block. LDN 0x00-0x03/0x05/0x06 follow the same
 * generation's (it8728f/it8786e) numbering by convention.
 */
#define IT8620E_FDC  0x00 /* Floppy */
#define IT8620E_SP1  0x01 /* Com1 */
#define IT8620E_SP2  0x02 /* Com2 */
#define IT8620E_PP   0x03 /* Parallel port */
#define IT8620E_EC   0x04 /* Environment controller */
#define IT8620E_KBCK 0x05 /* PS/2 keyboard */
#define IT8620E_KBCM 0x06 /* PS/2 mouse */
#define IT8620E_GPIO 0x07 /* GPIO */
#define IT8620E_IR   0x0a /* Consumer IR / PME */

/* Global configuration registers. */
#define IT8620E_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8620E_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8620E_CONFIG_REG_CHIPID    0x20 /* Chip ID, high byte (0x86). */
#define IT8620E_CONFIG_REG_CHIPVERS  0x22 /* Chip version */
#define IT8620E_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8620E_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. 'Special register' */

#endif /* SUPERIO_ITE_IT8620E_H */
