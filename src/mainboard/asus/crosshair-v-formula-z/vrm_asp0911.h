/*
 * ASP0911 (UPI Semiconductor power meter / current monitor)
 * accessed via the ENE KB3722Q embedded controller PMBus bridge on O2_SMB1.
 *
 * The ASP0911 (component PU2000) measures input voltage, output current,
 * and output power for the CPU rail and reports them over PMBus.
 *
 * PMBus slave address (8-bit write format): determined by probing O2_SMB1
 * at runtime or from board schematics; see asp0911_probe().
 *
 * Known candidate addresses (7-bit) based on UPI Semiconductor ASP0911
 * datasheet and typical ASUS board strapping:
 *   0x22 → write addr 0x44
 *   0x40 → write addr 0x80
 *   0x44 → write addr 0x88
 *
 * Kconfig ASP0911_SLAVE_ADDR allows the address to be overridden at
 * build time once confirmed from hardware.
 */

#ifndef __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_ASP0911_H__
#define __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_ASP0911_H__

#include <stdint.h>

/*
 * Build-time configurable slave address.  Defaults to 0 (probe at runtime).
 * Set to the confirmed 8-bit write address via Kconfig.
 */
#ifndef CONFIG_ASP0911_SLAVE_ADDR
#define CONFIG_ASP0911_SLAVE_ADDR  0
#endif

/*
 * Standard PMBus monitoring registers (all return 2-byte PMBus linear format).
 * These are standard across PMBus-compliant power monitors.
 */
#define ASP0911_REG_READ_VIN    0x88  /* Input voltage  (2-byte PMBus linear, mV) */
#define ASP0911_REG_READ_IOUT   0x8C  /* Output current (2-byte PMBus linear, mA) */
#define ASP0911_REG_READ_POUT   0x96  /* Output power   (2-byte PMBus linear, mW) */

/*
 * Candidate 8-bit write addresses for the ASP0911 on O2_SMB1.
 * Hardware probing tries each in order until a device acknowledges.
 */
static const u8 asp0911_probe_addrs[] = {
	0x44,  /* 7-bit 0x22 */
	0x80,  /* 7-bit 0x40 */
	0x88,  /* 7-bit 0x44 */
	0x90,  /* 7-bit 0x48 */
};

/*
 * PMBus linear format decoder (identical to ASP1000C version).
 * bits[15:11] = signed 5-bit exponent N
 * bits[10:0]  = unsigned 11-bit mantissa Y
 * Result = Y * 2^N  (in the register's native unit: mV, mA, or mW)
 */
static inline int asp0911_linear_to_int(u16 raw)
{
	int n = (int16_t)(raw & 0xf800) >> 11;
	int y = raw & 0x07ff;
	if (n >= 0)
		return y << n;
	return y >> (-n);
}

u8  asp0911_probe(void);
int asp0911_get_vin_mv(u8 slave_addr, int *mv_out);
int asp0911_get_iout_ma(u8 slave_addr, int *ma_out);
int asp0911_get_pout_mw(u8 slave_addr, int *mw_out);
void asp0911_report(void);

#endif /* __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_ASP0911_H__ */
