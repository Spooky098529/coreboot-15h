/* hda_verb.c - HD Audio codec pin configuration
 */
#include <device/azalia_device.h>

/*
 * Realtek ALC1150, values read back from the vendor BIOS
 * (dumps/board-dump-20261113-122222/hda-hwC0D0.txt, init_pin_configs).
 *
 * Only the ten real pin complexes are programmed. On the ALC1150, NIDs 0x12,
 * 0x1C, 0x1D and 0x1F are vendor-defined widgets, not pins (see the codec
 * node list in asound-_proc_asound_card0_codec_0.txt), so no pin-config
 * verbs are sent to them - matching the vendor BIOS.
 *
 * GPU HDMI audio (codec 1) lives on the add-in card and is not listed here.
 */
const u32 cim_verb_data[] = {
	0x10EC0900,		/* Codec Vendor/Device ID: Realtek ALC1150 */
	0x1458A056,		/* Subsystem ID */
	11,			/* Number of entries (1 SUBVENDOR + 10 PIN_CFG) */
	AZALIA_SUBVENDOR(0x0, 0x1458A056),
	AZALIA_PIN_CFG(0, 0x11, 0x99430130),	/* SPDIF out, internal (ATAPI) */
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),	/* Line out, rear, green */
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),	/* Line out, rear, black */
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),	/* Line out, rear, orange */
	AZALIA_PIN_CFG(0, 0x17, 0x01012014),	/* Line out, rear, grey */
	AZALIA_PIN_CFG(0, 0x18, 0x01A19050),	/* Mic in, rear, pink */
	AZALIA_PIN_CFG(0, 0x19, 0x02A19060),	/* Mic in, front, pink */
	AZALIA_PIN_CFG(0, 0x1A, 0x0181305F),	/* Line in, rear, blue */
	AZALIA_PIN_CFG(0, 0x1B, 0x02214020),	/* HP out, front, green */
	AZALIA_PIN_CFG(0, 0x1E, 0x01452140),	/* SPDIF out, rear, optical */
};
const u32 pc_beep_verbs[0] = {};
AZALIA_ARRAY_SIZES;
