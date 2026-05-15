/*
 * acpi/superio.asl -- Super I/O: ITE IT8721F at 0x2E
 *
 * The Super I/O ACPI device definitions are provided by
 * the coreboot Super I/O driver, not the board.
 * This file is for any board-specific SIO overrides and
 * configuration. On this board, we just enable only the
 * wired functions, and set the base & device ID to match
 * other references (default of SIO0)
 */

#include "ec.asl"

#undef SUPERIO_DEV
#undef SUPERIO_PNP_BASE
#undef IT8721F_SHOW_SP1
#undef IT8721F_SHOW_SP2
#undef IT8721F_SHOW_EC
#undef IT8721F_SHOW_KBCK
#undef IT8721F_SHOW_KBCM
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define IT8721F_SHOW_SP1
#define IT8721F_SHOW_EC
#include <superio/ite/it8721f/acpi/superio.asl>
