/* hda_verb.c - HD Audio codec pin configuration
 */
#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Codec 0: Realtek 0892 (vendor 0x10EC device 0x0892 rev 1.3 subsys 0x10438436) */
	0x10EC0892,		/* Vendor/Device ID */
	0x10438436,		/* Subsystem ID */
	15,			/* Number of entries (1 SUBVENDOR + 14 PIN_CFG) */
	/* Program the codec's subsystem ID register so it advertises the
	 * board's SSID rather than the chip's reset default.  Realtek's
	 * Linux driver derives several BIOS hint bits (incl. its
	 * "SKU not ready" check at NID 0x1D) from this. */
	AZALIA_SUBVENDOR(0x0, 0x10438436),
	/* NID 0x11  Port=Fixed/Int   Dev=SPDIF-Out    Conn=ATAPI         Color=Unknown  Loc=Int-Dig-Disp    Grp=4 Seq=0 */
	AZALIA_PIN_CFG(0, 0x11, 0x99430140),
	/* NID 0x12  Port=No-Conn     Dev=Speaker      Conn=1/8"          Color=Black    Loc=Ext-Rear        Grp=15 Seq=0 */
	AZALIA_PIN_CFG(0, 0x12, 0x411111F0),
	/* NID 0x14  Port=Jack        Dev=Line-Out     Conn=1/8"          Color=Green    Loc=Ext-Rear        Grp=1 Seq=0 */
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	/* NID 0x15  Port=Jack        Dev=Line-Out     Conn=1/8"          Color=Black    Loc=Ext-Rear        Grp=1 Seq=2 */
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),
	/* NID 0x16  Port=Jack        Dev=Line-Out     Conn=1/8"          Color=Orange   Loc=Ext-Rear        Grp=1 Seq=1 */
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	/* NID 0x17  Port=Jack        Dev=Line-Out     Conn=1/8"          Color=Grey     Loc=Ext-Rear        Grp=1 Seq=4 */
	AZALIA_PIN_CFG(0, 0x17, 0x01012014),
	/* NID 0x18  Port=Jack        Dev=Mic-In       Conn=1/8"          Color=Pink     Loc=Ext-Rear        Grp=5 Seq=0 */
	AZALIA_PIN_CFG(0, 0x18, 0x01A19850),
	/* NID 0x19  Port=Jack        Dev=Mic-In       Conn=1/8"          Color=Pink     Loc=Ext-Front       Grp=6 Seq=0 */
	AZALIA_PIN_CFG(0, 0x19, 0x02A19C60),
	/* NID 0x1A  Port=Jack        Dev=Line-In      Conn=1/8"          Color=Blue     Loc=Ext-Rear        Grp=5 Seq=15 */
	AZALIA_PIN_CFG(0, 0x1A, 0x0181305F),
	/* NID 0x1B  Port=Jack        Dev=HP-Out       Conn=1/8"          Color=Green    Loc=Ext-Front       Grp=2 Seq=0 */
	AZALIA_PIN_CFG(0, 0x1B, 0x02214C20),
	/* NID 0x1C  Port=No-Conn     Dev=Speaker      Conn=1/8"          Color=Black    Loc=Ext-Rear        Grp=15 Seq=0 */
	AZALIA_PIN_CFG(0, 0x1C, 0x411111F0),
	/* NID 0x1D  Port=No-Conn     Dev=Line-Out     Conn=Analog-Other  Color=White    Loc=Ext-N/A         Grp=2 Seq=9 */
	AZALIA_PIN_CFG(0, 0x1D, 0x4007E629),
	/* NID 0x1E  Port=Jack        Dev=SPDIF-Out    Conn=Optical       Color=Orange   Loc=Ext-Rear        Grp=3 Seq=0 */
	AZALIA_PIN_CFG(0, 0x1E, 0x01456130),
	/* NID 0x1F  Port=No-Conn     Dev=Speaker      Conn=1/8"          Color=Black    Loc=Ext-Rear        Grp=15 Seq=0 */
	AZALIA_PIN_CFG(0, 0x1F, 0x411111F0),
};
const u32 pc_beep_verbs[0] = {};
AZALIA_ARRAY_SIZES;
