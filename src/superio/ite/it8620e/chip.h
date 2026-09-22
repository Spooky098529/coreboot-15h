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

#ifndef SUPERIO_ITE_IT8620E_CHIP_H
#define SUPERIO_ITE_IT8620E_CHIP_H

#include <superio/ite/common/env_ctrl_chip.h>

struct superio_ite_it8620e_config {
	struct ite_ec_config ec;
};

#endif /* SUPERIO_ITE_IT8620E_CHIP_H */
