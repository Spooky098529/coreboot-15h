/*
 * acpi/superio.asl -- Super I/O: ITE IT8620E at 0x2E
 *
 * Exposes COM1 (LDN1) and the EC/HWM (LDN4) through the generic SIO ASL.
 * The PS/2 keyboard/mouse are declared statically in ec.asl (same split as
 * the Sabertooth port this board was derived from).
 */

#include "ec.asl"

#undef SUPERIO_DEV
#undef SUPERIO_PNP_BASE
#undef IT8620E_SHOW_SP1
#undef IT8620E_SHOW_SP2
#undef IT8620E_SHOW_EC
#undef IT8620E_SHOW_KBCK
#undef IT8620E_SHOW_KBCM
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	0x2e
#define IT8620E_SHOW_SP1
#define IT8620E_SHOW_EC
#include <superio/ite/it8620e/acpi/superio.asl>
