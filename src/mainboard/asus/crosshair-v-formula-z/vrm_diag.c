/*
 * PMBus VRM diagnostic dump for ASUS Sabertooth 990FX R2.0.
 *
 * Probes the known PMBus devices on O2_SMB1 (via the KB3722Q EC bridge),
 * reads all standard and vendor-specific registers, and logs the results.
 *
 * Two decoding paths are used:
 *
 *   pmbus_linear_milliunit() — for non-VOUT registers (READ_VIN, READ_IOUT,
 *     READ_POUT, etc.) that use the packed PMBus linear format where bits[15:11]
 *     carry the signed 5-bit exponent N and bits[10:0] carry the mantissa Y.
 *     Result = Y × 2^N × 1000 (i.e. millivolts, milliamps, milliwatts).
 *
 *   pmbus_vout_millivolts() — for VOUT registers (READ_VOUT, VOUT_COMMAND, etc.)
 *     that in linear mode return only the mantissa Y; the exponent N comes from
 *     VOUT_MODE bits[4:0] (sign-extended to 5 bits).
 *
 * Gated by CONFIG_BOARD_VRM_DIAG; not compiled in normal builds.
 */

#include <stdint.h>
#include <console/console.h>
#include <string.h>
#include "ec.h"
#include "vrm_asp1000c.h"
#include "vrm_asp0911.h"
#include "vrm_diag.h"

/* -----------------------------------------------------------------------
 * Standard PMBus Part I rev 1.3b command codes
 * ----------------------------------------------------------------------- */
#define PMBUS_OPERATION          0x01
#define PMBUS_ON_OFF_CONFIG      0x02
#define PMBUS_CAPABILITY         0x19
#define PMBUS_VOUT_MODE          0x20
#define PMBUS_VOUT_COMMAND       0x21
#define PMBUS_VOUT_TRIM          0x22
#define PMBUS_VOUT_CAL_OFFSET    0x23
#define PMBUS_VOUT_MAX           0x24
#define PMBUS_VOUT_MARGIN_HIGH   0x25
#define PMBUS_VOUT_MARGIN_LOW    0x26
#define PMBUS_IOUT_CAL_GAIN      0x38
#define PMBUS_IOUT_CAL_OFFSET    0x39
#define PMBUS_STATUS_BYTE        0x78
#define PMBUS_STATUS_WORD        0x79
#define PMBUS_STATUS_VOUT        0x7A
#define PMBUS_STATUS_IOUT        0x7B
#define PMBUS_STATUS_INPUT       0x7C
#define PMBUS_STATUS_TEMPERATURE 0x7D
#define PMBUS_STATUS_CML         0x7E
#define PMBUS_STATUS_OTHER       0x7F
#define PMBUS_STATUS_MFR         0x80
#define PMBUS_READ_VIN           0x88
#define PMBUS_READ_IIN           0x89
#define PMBUS_READ_VCAP          0x8A
#define PMBUS_READ_VOUT          0x8B
#define PMBUS_READ_IOUT          0x8C
#define PMBUS_READ_TEMP_1        0x8D
#define PMBUS_READ_TEMP_2        0x8E
#define PMBUS_READ_TEMP_3        0x8F
#define PMBUS_READ_FAN_1         0x90
#define PMBUS_READ_FAN_2         0x91
#define PMBUS_READ_POUT          0x96
#define PMBUS_READ_PIN           0x97
#define PMBUS_REVISION           0x98
#define PMBUS_MFR_ID             0x99
#define PMBUS_MFR_MODEL          0x9A
#define PMBUS_MFR_REVISION       0x9B
#define PMBUS_MFR_LOCATION       0x9C
#define PMBUS_MFR_DATE           0x9D
#define PMBUS_MFR_SERIAL         0x9E

/* CAPABILITY register bits */
#define PMBUS_CAP_PEC        (1 << 7)
#define PMBUS_CAP_400K       (1 << 5)
#define PMBUS_CAP_SMBALERT   (1 << 4)

/* VOUT_MODE encoding */
#define PMBUS_VOUTMODE_LINEAR  0x00
#define PMBUS_VOUTMODE_VID     0x20
#define PMBUS_VOUTMODE_DIRECT  0x40
#define PMBUS_VOUTMODE_MASK    0xE0

/* STATUS_WORD bit descriptions (upper byte in high bits, lower byte in low) */
#define SW_VOUT_OV   (1u << 15)
#define SW_IOUT_OC   (1u << 14)
#define SW_VIN_UV    (1u << 13)
#define SW_TEMP      (1u << 12)
#define SW_CML       (1u << 11)
#define SW_OTHER     (1u << 10)
#define SW_SMBALERT  (1u << 9)
#define SW_UNKNOWN   (1u << 8)
#define SW_BUSY      (1u << 7)
#define SW_OFF       (1u << 6)
#define SW_VOUT_OV_L (1u << 5)   /* VOUT fault/warning */
#define SW_IOUT_OC_L (1u << 4)   /* IOUT/POUT fault/warning */
#define SW_INPUT     (1u << 3)
#define SW_MFR       (1u << 2)
#define SW_PGOOD_N   (1u << 1)
#define SW_OPCODE    (1u << 0)

/* -----------------------------------------------------------------------
 * PMBus linear format decoders
 * ----------------------------------------------------------------------- */

/*
 * Decode packed PMBus linear format to milli-units (mV, mA, mW, m°C).
 * bits[15:11] = signed 5-bit exponent N, bits[10:0] = unsigned mantissa Y.
 * value = Y × 2^N × 1000
 *
 * Used for: READ_VIN, READ_IIN, READ_IOUT, READ_POUT, READ_PIN, READ_TEMP_*,
 * VOUT_TRIM, IOUT_CAL_GAIN, IOUT_CAL_OFFSET, and most VOUT config registers
 * on devices that pack N into the word (non-standard but common).
 */
static int pmbus_linear_milliunit(u16 raw)
{
	int n = (int16_t)(raw & 0xf800) >> 11;
	int y = raw & 0x07ff;
	if (n >= 0)
		return (y * 1000) << n;
	return (y * 1000) >> (-n);
}

/*
 * Decode a VOUT register value when VOUT_MODE indicates linear mode.
 * The chip returns only the mantissa Y; the exponent N is in VOUT_MODE[4:0].
 * value_mV = Y × 2^N × 1000
 *
 * vout_mode: the byte returned by PMBUS_VOUT_MODE (0x20).
 */
static int pmbus_vout_millivolts(u16 raw_y, u8 vout_mode)
{
	/* Sign-extend 5-bit exponent from VOUT_MODE bits[4:0] */
	int n = (int8_t)((vout_mode & 0x1f) << 3) >> 3;
	if (n >= 0)
		return ((int)raw_y * 1000) << n;
	return ((int)raw_y * 1000) >> (-n);
}

/* -----------------------------------------------------------------------
 * Per-register dump helpers
 * ----------------------------------------------------------------------- */

static void diag_byte(u8 addr, u8 reg, const char *label)
{
	u8 val;
	if (kb3722q_pmbus_read_byte(addr, reg, &val) != 0)
		printk(BIOS_INFO, "    %-26s(0x%02x): --\n", label, reg);
	else
		printk(BIOS_INFO, "    %-26s(0x%02x): 0x%02x  (%3d)\n",
		       label, reg, val, val);
}

static void diag_word_linear(u8 addr, u8 reg, const char *label,
			     const char *unit)
{
	u16 raw;
	if (kb3722q_pmbus_read_word(addr, reg, &raw) != 0) {
		printk(BIOS_INFO, "    %-26s(0x%02x): --\n", label, reg);
		return;
	}
	printk(BIOS_INFO, "    %-26s(0x%02x): 0x%04x  = %d m%s\n",
	       label, reg, raw, pmbus_linear_milliunit(raw), unit);
}

/*
 * Decode and print a VOUT register both ways: using the packed linear decoder
 * (which works if the chip packs N into the word) and using the VOUT_MODE
 * exponent (which is the PMBus standard for linear-mode VOUT registers).
 * Printing both lets us determine empirically which encoding the chip uses.
 */
static void diag_word_vout(u8 addr, u8 reg, const char *label, u8 vout_mode)
{
	u16 raw;
	int mv_packed, mv_mode;
	if (kb3722q_pmbus_read_word(addr, reg, &raw) != 0) {
		printk(BIOS_INFO, "    %-26s(0x%02x): --\n", label, reg);
		return;
	}
	mv_packed = pmbus_linear_milliunit(raw);
	mv_mode   = pmbus_vout_millivolts(raw, vout_mode);
	printk(BIOS_INFO,
	       "    %-26s(0x%02x): 0x%04x  packed=%d mV  mode-N=%d mV\n",
	       label, reg, raw, mv_packed, mv_mode);
}

static void diag_block_string(u8 addr, u8 reg, const char *label)
{
	u8 buf[KB3722Q_PMBUS_BLOCK_MAXLEN + 1];
	u8 len = 0;
	if (kb3722q_pmbus_read_block(addr, reg, buf, &len) != 0 || len == 0) {
		printk(BIOS_INFO, "    %-26s(0x%02x): --\n", label, reg);
		return;
	}
	buf[len] = '\0';
	printk(BIOS_INFO, "    %-26s(0x%02x): \"%s\" (%d B)\n",
	       label, reg, buf, len);
}

static void diag_block_hex(u8 addr, u8 reg, const char *label)
{
	u8 buf[KB3722Q_PMBUS_BLOCK_MAXLEN];
	u8 len = 0, i;
	if (kb3722q_pmbus_read_block(addr, reg, buf, &len) != 0 || len == 0) {
		printk(BIOS_INFO, "    %-26s(0x%02x): --\n", label, reg);
		return;
	}
	printk(BIOS_INFO, "    %-26s(0x%02x): [%d B]", label, reg, len);
	for (i = 0; i < len; i++)
		printk(BIOS_INFO, " %02x", buf[i]);
	printk(BIOS_INFO, "\n");
}

/* -----------------------------------------------------------------------
 * CAPABILITY / VOUT_MODE / STATUS_WORD detailed decoders
 * ----------------------------------------------------------------------- */

static u8 diag_capability(u8 addr)
{
	u8 cap;
	if (kb3722q_pmbus_read_byte(addr, PMBUS_CAPABILITY, &cap) != 0) {
		printk(BIOS_INFO, "    CAPABILITY              (0x19): --\n");
		return 0;
	}
	printk(BIOS_INFO,
	       "    CAPABILITY              (0x19): 0x%02x"
	       "  PEC=%d  SMBALERT=%d  MaxBus=%s\n",
	       cap,
	       !!(cap & PMBUS_CAP_PEC),
	       !!(cap & PMBUS_CAP_SMBALERT),
	       (cap & PMBUS_CAP_400K) ? "400kHz" : "100kHz");
	return cap;
}

static u8 diag_vout_mode(u8 addr)
{
	u8 mode;
	const char *mode_str;
	int n;
	if (kb3722q_pmbus_read_byte(addr, PMBUS_VOUT_MODE, &mode) != 0) {
		printk(BIOS_INFO, "    VOUT_MODE               (0x20): --\n");
		return 0;
	}
	switch (mode & PMBUS_VOUTMODE_MASK) {
	case PMBUS_VOUTMODE_LINEAR: mode_str = "linear"; break;
	case PMBUS_VOUTMODE_VID:    mode_str = "VID";    break;
	case PMBUS_VOUTMODE_DIRECT: mode_str = "direct"; break;
	default:                    mode_str = "?";      break;
	}
	n = (int8_t)((mode & 0x1f) << 3) >> 3;
	printk(BIOS_INFO,
	       "    VOUT_MODE               (0x20): 0x%02x  %s  exp=%+d\n",
	       mode, mode_str, n);
	return mode;
}

static void diag_pmbus_revision(u8 addr)
{
	u8 rev;
	if (kb3722q_pmbus_read_byte(addr, PMBUS_REVISION, &rev) != 0) {
		printk(BIOS_INFO, "    PMBUS_REVISION          (0x98): --\n");
		return;
	}
	/* Part I: bits[7:4] → major.minor; Part II: bits[3:0] → major.minor */
	printk(BIOS_INFO,
	       "    PMBUS_REVISION          (0x98): 0x%02x"
	       "  Part-I=%d.%d  Part-II=%d.%d\n",
	       rev,
	       (rev >> 6) & 0x3, (rev >> 4) & 0x3,
	       (rev >> 2) & 0x3,  rev       & 0x3);
}

static void diag_status_word(u8 addr)
{
	u16 sw;
	if (kb3722q_pmbus_read_word(addr, PMBUS_STATUS_WORD, &sw) != 0) {
		printk(BIOS_INFO, "    STATUS_WORD             (0x79): --\n");
		return;
	}
	printk(BIOS_INFO,
	       "    STATUS_WORD             (0x79): 0x%04x%s%s%s%s%s%s%s%s%s%s\n",
	       sw,
	       (sw & SW_VOUT_OV)  ? " VOUT_OV"   : "",
	       (sw & SW_IOUT_OC)  ? " IOUT_OC"   : "",
	       (sw & SW_VIN_UV)   ? " VIN_UV"    : "",
	       (sw & SW_TEMP)     ? " TEMP"      : "",
	       (sw & SW_CML)      ? " CML"       : "",
	       (sw & SW_OTHER)    ? " OTHER"     : "",
	       (sw & SW_BUSY)     ? " BUSY"      : "",
	       (sw & SW_OFF)      ? " OFF"       : "",
	       (sw & SW_PGOOD_N)  ? " PGOOD#"    : "",
	       (sw == 0)          ? " [OK]"      : "");
}

/* -----------------------------------------------------------------------
 * Full device probe
 * ----------------------------------------------------------------------- */

static void diag_device(u8 addr, const char *name)
{
	u8 vout_mode;

	printk(BIOS_INFO, "\n[PMBus] %s  slave=0x%02x (7-bit 0x%02x)\n",
	       name, addr, addr >> 1);

	printk(BIOS_INFO, "  -- Identity --\n");
	diag_pmbus_revision(addr);
	diag_capability(addr);
	diag_byte(addr, PMBUS_OPERATION,     "OPERATION");
	diag_byte(addr, PMBUS_ON_OFF_CONFIG, "ON_OFF_CONFIG");

	printk(BIOS_INFO, "  -- VOUT encoding (read VOUT_MODE first) --\n");
	vout_mode = diag_vout_mode(addr);

	printk(BIOS_INFO, "  -- Output configuration --\n");
	/*
	 * Print VOUT configuration registers using both decoders so we can
	 * determine empirically which encoding the chip actually uses.
	 * "packed" = treats the raw word as packed-linear (N in bits[15:11]).
	 * "mode-N" = uses the exponent from VOUT_MODE (PMBus standard for VOUT).
	 */
	diag_word_vout(addr, PMBUS_VOUT_COMMAND,     "VOUT_COMMAND",     vout_mode);
	diag_word_vout(addr, PMBUS_VOUT_TRIM,        "VOUT_TRIM",        vout_mode);
	diag_word_vout(addr, PMBUS_VOUT_CAL_OFFSET,  "VOUT_CAL_OFFSET",  vout_mode);
	diag_word_vout(addr, PMBUS_VOUT_MAX,         "VOUT_MAX",         vout_mode);
	diag_word_vout(addr, PMBUS_VOUT_MARGIN_HIGH, "VOUT_MARGIN_HIGH", vout_mode);
	diag_word_vout(addr, PMBUS_VOUT_MARGIN_LOW,  "VOUT_MARGIN_LOW",  vout_mode);
	diag_word_linear(addr, PMBUS_IOUT_CAL_GAIN,    "IOUT_CAL_GAIN",  "A");
	diag_word_linear(addr, PMBUS_IOUT_CAL_OFFSET,  "IOUT_CAL_OFFSET","A");

	printk(BIOS_INFO, "  -- Status --\n");
	diag_status_word(addr);
	diag_byte(addr, PMBUS_STATUS_BYTE,        "STATUS_BYTE");
	diag_byte(addr, PMBUS_STATUS_VOUT,        "STATUS_VOUT");
	diag_byte(addr, PMBUS_STATUS_IOUT,        "STATUS_IOUT");
	diag_byte(addr, PMBUS_STATUS_INPUT,       "STATUS_INPUT");
	diag_byte(addr, PMBUS_STATUS_TEMPERATURE, "STATUS_TEMPERATURE");
	diag_byte(addr, PMBUS_STATUS_CML,         "STATUS_CML");
	diag_byte(addr, PMBUS_STATUS_OTHER,       "STATUS_OTHER");
	diag_byte(addr, PMBUS_STATUS_MFR,         "STATUS_MFR");

	printk(BIOS_INFO, "  -- Telemetry --\n");
	diag_word_linear(addr, PMBUS_READ_VIN,    "READ_VIN",    "V");
	diag_word_linear(addr, PMBUS_READ_IIN,    "READ_IIN",    "A");
	diag_word_linear(addr, PMBUS_READ_VCAP,   "READ_VCAP",   "V");
	/*
	 * READ_VOUT uses VOUT_MODE exponent (standard) but some chips pack N
	 * into the word anyway.  Print both decodings.
	 */
	diag_word_vout(addr, PMBUS_READ_VOUT,     "READ_VOUT",   vout_mode);
	diag_word_linear(addr, PMBUS_READ_IOUT,   "READ_IOUT",   "A");
	diag_word_linear(addr, PMBUS_READ_TEMP_1, "READ_TEMP_1", "C");
	diag_word_linear(addr, PMBUS_READ_TEMP_2, "READ_TEMP_2", "C");
	diag_word_linear(addr, PMBUS_READ_TEMP_3, "READ_TEMP_3", "C");
	diag_word_linear(addr, PMBUS_READ_FAN_1,  "READ_FAN_1",  "RPM");
	diag_word_linear(addr, PMBUS_READ_FAN_2,  "READ_FAN_2",  "RPM");
	diag_word_linear(addr, PMBUS_READ_POUT,   "READ_POUT",   "W");
	diag_word_linear(addr, PMBUS_READ_PIN,    "READ_PIN",    "W");

	printk(BIOS_INFO, "  -- MFR identification (block reads) --\n");
	diag_block_string(addr, PMBUS_MFR_ID,       "MFR_ID");
	diag_block_string(addr, PMBUS_MFR_MODEL,    "MFR_MODEL");
	diag_block_string(addr, PMBUS_MFR_REVISION, "MFR_REVISION");
	diag_block_string(addr, PMBUS_MFR_LOCATION, "MFR_LOCATION");
	diag_block_string(addr, PMBUS_MFR_DATE,     "MFR_DATE");
	diag_block_hex(addr,    PMBUS_MFR_SERIAL,   "MFR_SERIAL");
}

static void diag_asp1000c_vendor(void)
{
	u8 addr = ASP1000C_SLAVE_ADDR;

	printk(BIOS_INFO, "  -- ASP1000C vendor-specific registers --\n");
	/*
	 * Vendor VID-encoded VOUT registers (1-byte each, not standard PMBus
	 * word format).  Standard VOUT_COMMAND at 0x21 may be a no-op or alias.
	 */
	diag_byte(addr, ASP1000C_REG_VOUT_MARGIN_LOW,  "VOUT_MARGIN_LOW[VID](0x73)");
	diag_byte(addr, ASP1000C_REG_VOUT_MARGIN_HIGH, "VOUT_MARGIN_HIGH[VID](0x74)");
	diag_byte(addr, ASP1000C_REG_VOUT_COMMAND,     "VOUT_COMMAND[VID](0x75)");
	diag_byte(addr, ASP1000C_REG_VOUT_MAX,         "VOUT_MAX[VID](0x76)");
	/*
	 * Vendor-specific register at 0x1C; function undocumented (see vrm_asp1000c.h).
	 */
	diag_byte(addr, ASP1000C_REG_INIT_RESET,       "INIT_RESET(0x1C)");
	/*
	 * VENDOR_UNLOCK: write 0x29 to enable OC write mode; readback tells us
	 * current lock state.
	 */
	diag_byte(addr, ASP1000C_REG_VENDOR_UNLOCK,    "VENDOR_UNLOCK(0x94)");
	/*
	 * Probe additional unknown vendor registers around 0x9C-0x9F and
	 * 0xD0-0xD3 to discover hidden capability / telemetry registers.
	 */
	printk(BIOS_INFO, "  -- ASP1000C unknown vendor probe (0x9C-0x9F) --\n");
	diag_byte(addr, 0x9C, "reg_0x9C");
	diag_byte(addr, 0x9D, "reg_0x9D");
	diag_byte(addr, 0x9E, "reg_0x9E");
	diag_byte(addr, 0x9F, "reg_0x9F");
	printk(BIOS_INFO, "  -- ASP1000C unknown vendor probe (0xD0-0xD7) --\n");
	diag_byte(addr, 0xD0, "reg_0xD0");
	diag_byte(addr, 0xD1, "reg_0xD1");
	diag_byte(addr, 0xD2, "reg_0xD2");
	diag_byte(addr, 0xD3, "reg_0xD3");
	diag_byte(addr, 0xD4, "reg_0xD4");
	diag_byte(addr, 0xD5, "reg_0xD5");
	diag_byte(addr, 0xD6, "reg_0xD6");
	diag_byte(addr, 0xD7, "reg_0xD7");
}

/* -----------------------------------------------------------------------
 * Candidate probe for unknown devices (ASP1103 DRAM VRM etc.)
 * ----------------------------------------------------------------------- */

/*
 * Probe a candidate address: try a STATUS_BYTE read.  If the device ACKs
 * (read succeeds), run the full device diagnostic.  This is used for devices
 * whose slave address is determined by probing at runtime.
 */
static int probe_candidate(u8 addr, const char *name)
{
	u8 dummy;
	if (kb3722q_pmbus_read_byte(addr, PMBUS_STATUS_BYTE, &dummy) != 0)
		return 0;
	printk(BIOS_INFO, "[PMBus] probe HIT: %s at 0x%02x (7-bit 0x%02x)"
	       " STATUS_BYTE=0x%02x\n", name, addr, addr >> 1, dummy);
	diag_device(addr, name);
	return 1;
}

/* -----------------------------------------------------------------------
 * Entry point
 * ----------------------------------------------------------------------- */

void vrm_pmbus_diag(void)
{
	u8 asp0911_addr;
	int i;

	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "========================================\n");
	printk(BIOS_INFO, " PMBus VRM Diagnostic — O2_SMB1 (KB3722Q bridge)\n");
	printk(BIOS_INFO, "========================================\n");

	diag_device(ASP1000C_SLAVE_ADDR, "ASP1000C (CPU VCore)");
	diag_asp1000c_vendor();

	asp0911_addr = asp0911_probe();
	if (asp0911_addr)
		diag_device(asp0911_addr, "ASP0911 (CPU power meter)");
	else
		printk(BIOS_INFO, "\n[PMBus] ASP0911 not found at any candidate address\n");

	/* ASP1103 DRAM VDD regulator: address not confirmed, probe candidates */
	printk(BIOS_INFO, "\n[PMBus] Probing for ASP1103 (DRAM VRM) — "
	       "address unconfirmed\n");
	{
		static const u8 asp1103_candidates[] = {
			0xC0,  /* 7-bit 0x60 */
			0xC2,  /* 7-bit 0x61 */
			0xC4,  /* 7-bit 0x62 */
			0xC6,  /* 7-bit 0x63 */
			0x20,  /* 7-bit 0x10 */
			0x22,  /* 7-bit 0x11 */
			0x40,  /* 7-bit 0x20 */
			0x42,  /* 7-bit 0x21 */
		};
		int found = 0;
		for (i = 0; i < (int)ARRAY_SIZE(asp1103_candidates); i++) {
			if (probe_candidate(asp1103_candidates[i],
					    "ASP1103? (DRAM VRM candidate)"))
				found++;
		}
		if (!found)
			printk(BIOS_INFO,
			       "[PMBus] ASP1103 not found at any candidate address\n");
	}

	printk(BIOS_INFO, "\n");
	printk(BIOS_INFO, "========================================\n");
	printk(BIOS_INFO, " PMBus VRM Diagnostic complete\n");
	printk(BIOS_INFO, "========================================\n\n");
}
