/* hda_verb.c - HD Audio codec pin configuration
 */
#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek ALC899 HDA CODEC */
	0x10EC0899,		/* Vendor/Device ID */
	0x10438436,		/* Subsystem ID */
	15,			/* Number of entries (1 SUBVENDOR + 14 PIN_CFG) */
	/* Program the codec's subsystem ID register so it advertises the
	 * board's SSID rather than the chip's reset default.  Realtek's
	 * Linux driver derives several BIOS hint bits (incl. its
	 * "SKU not ready" check at NID 0x1D) from this. */
	AZALIA_SUBVENDOR(0x0, 0x104385bf),
	AZALIA_PIN_CFG(0, 17, 0x99430130), // SPDIF-out (Fixed)
	AZALIA_PIN_CFG(0, 18, 0x411111f0), // Speaker (None) [DISABLED]
	AZALIA_PIN_CFG(0, 20, 0x01014410), // Line-out (Green Jack)
	AZALIA_PIN_CFG(0, 21, 0x01011012), // Line-out (Black Jack)
	AZALIA_PIN_CFG(0, 22, 0x01016011), // Line-out (Orange Jack)
	AZALIA_PIN_CFG(0, 23, 0x01012014), // Line-out (Grey Jack)
	AZALIA_PIN_CFG(0, 24, 0x01a19850), // Mic (Pink Jack)
	AZALIA_PIN_CFG(0, 25, 0x02a19c60), // Mic (Pink Jack)
	AZALIA_PIN_CFG(0, 26, 0x0181345f), // Line-in (Blue Jack)
	AZALIA_PIN_CFG(0, 27, 0x02214c20), // Headphones (Green Jack)
	AZALIA_PIN_CFG(0, 28, 0x411111f0), // Speaker (None) [DISABLED]
	AZALIA_PIN_CFG(0, 29, 0x4007e629), // beep widget
	AZALIA_PIN_CFG(0, 30, 0x01456140), // SPDIF-out (Orange Jack)
	AZALIA_PIN_CFG(0, 31, 0x411111f0), // Speaker (None) [DISABLED]
};
const u32 pc_beep_verbs[0] = {};
AZALIA_ARRAY_SIZES;
