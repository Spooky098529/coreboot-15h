#
# This file is part of the coreboot project.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

romstage-y += buildOpts.c
romstage-y += BiosCallOuts.c
romstage-y += OemCustomize.c
romstage-y += ec.c
romstage-y += vrm_asp1000c.c

ramstage-y += buildOpts.c
ramstage-y += BiosCallOuts.c
ramstage-y += OemCustomize.c
ramstage-y += rd890_cfg.c
ramstage-y += ec.c
ramstage-y += vrm_asp1000c.c
ramstage-$(CONFIG_BOARD_VRM_ASP0911) += vrm_asp0911.c
ramstage-$(CONFIG_BOARD_VRM_DIAG)   += vrm_diag.c
