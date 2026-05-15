/*
 * ASP1000C (Chil/Intersil multi-phase CPU VCore PWM controller)
 * accessed via the ENE KB3722Q embedded controller PMBus bridge on O2_SMB1.
 *
 * PMBus slave address (8-bit write format): 0xB6 (7-bit: 0x5B).
 */

#ifndef __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_ASP1000C_H__
#define __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_ASP1000C_H__

#include <stdint.h>

/* PMBus 8-bit write address on O2_SMB1; set by ADDR pin strapping */
#define ASP1000C_SLAVE_ADDR     0xB6  /* 7-bit addr 0x5B */

/* Registers 0x73–0x76 are Chil/Intersil vendor-specific extensions */
#define ASP1000C_REG_VOUT_MARGIN_LOW   0x73  /* Lower margin voltage (1 byte, VID) */
#define ASP1000C_REG_VOUT_MARGIN_HIGH  0x74  /* Upper margin voltage (1 byte, VID) */
#define ASP1000C_REG_VOUT_COMMAND      0x75  /* Target output voltage (1 byte, VID) */
#define ASP1000C_REG_VOUT_MAX          0x76  /* Maximum allowed voltage (1 byte, VID) */
#define ASP1000C_REG_VENDOR_UNLOCK     0x94  /* Vendor OC mode: write 0x29 to unlock */

/*
 * Standard PMBus registers also supported by ASP1000C.
 */
#define ASP1000C_REG_READ_VOUT         0x8B  /* Read output voltage (2-byte PMBus linear) */
#define ASP1000C_REG_READ_IOUT         0x8C  /* Read output current (2-byte PMBus linear) */

#define ASP1000C_VENDOR_UNLOCK_KEY     0x29

/* Vendor-specific register at 0x1C; function undocumented, written 0x00 at init */
#define ASP1000C_REG_INIT_RESET        0x1C

/*
 * VID voltage encoding for the ASP1000C vendor-specific registers:
 *
 * The 1-byte VID code is an offset from the controller's programmed base
 * voltage.  Exact step size and base depend on the VID table programmed by
 * the board design:
 *   0x00  = no offset (nominal voltage)
 *   positive = higher voltage
 *   Two's-complement for negative offset
 *
 * READ_VOUT (0x8B) returns the actual measured output in 2-byte PMBus
 * linear format: bits[15:11] = signed 5-bit exponent N,
 * bits[10:0] = unsigned 11-bit mantissa Y.  Voltage = Y * 2^N mV.
 */

/* Decode a 2-byte PMBus linear-format value to millivolts */
static inline int asp1000c_linear_to_mv(u16 raw)
{
	int n = (int16_t)(raw & 0xf800) >> 11;  /* signed 5-bit exp */
	int y = raw & 0x07ff;                    /* 11-bit mantissa */
	if (n >= 0)
		return y << n;
	return y >> (-n);
}

int asp1000c_init(void);
int asp1000c_get_vcore_mv(int *mv_out);

/*
 * OC mode unlock/lock.
 *
 * Writing ASP1000C_VENDOR_UNLOCK_KEY (0x29) to register 0x94 puts the chip
 * in "vendor OC write mode": protected register windows become writeable
 * and (apparently) the regulation loop changes behaviour in ways we do not
 * yet fully understand.  Only enter this mode when actually applying OC
 * settings, and pair every unlock with a lock afterwards (the
 * sister chip ASP1103 explicitly clears 0x94 after use; we assume ASP1000C
 * is symmetric).
 *
 * Leaving the chip in OC mode unconditionally has been observed to cause
 * persistent boot misbehaviour that survives warm reboot and only clears
 * on AC power loss — so do NOT call asp1000c_oc_unlock() from boot init
 * paths; only call it transiently around an OC voltage change, paired with
 * asp1000c_oc_lock().
 */
int asp1000c_oc_unlock(void);
int asp1000c_oc_lock(void);
int asp1000c_set_vcore_vid(u8 vid);

#endif /* __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_ASP1000C_H__ */
