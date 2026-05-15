/*
 * ASP0911 CPU power meter driver for ASUS Sabertooth 990FX R2.0.
 *
 * The ASP0911 (UPI Semiconductor, component PU2000) reports input voltage,
 * output current, and output power on the CPU supply rail over PMBus via
 * the ENE KB3722Q EC bridge on O2_SMB1.
 *
 * The ASP0911 PMBus address isn't fixed by hardware strapping; this driver
 * probes O2_SMB1 at runtime to discover the device, then reads telemetry.
 *
 * If CONFIG_ASP0911_SLAVE_ADDR is set to a non-zero value in Kconfig the
 * probe is skipped and that address is used directly.
 */

#include <stdint.h>
#include <console/console.h>
#include "ec.h"
#include "vrm_asp0911.h"

/*
 * Probe O2_SMB1 for the ASP0911 by attempting a READ_VIN read against each
 * candidate address.  Returns the responding 8-bit write address, or 0 if
 * none found.
 */
u8 asp0911_probe(void)
{
#if CONFIG_ASP0911_SLAVE_ADDR != 0
	printk(BIOS_DEBUG, "ASP0911: using configured addr 0x%02x\n",
	       CONFIG_ASP0911_SLAVE_ADDR);
	return CONFIG_ASP0911_SLAVE_ADDR;
#else
	size_t i;
	for (i = 0; i < ARRAY_SIZE(asp0911_probe_addrs); i++) {
		u16 dummy;
		u8 addr = asp0911_probe_addrs[i];
		if (kb3722q_pmbus_read_word(addr, ASP0911_REG_READ_VIN,
					    &dummy) == 0) {
			printk(BIOS_DEBUG,
			       "ASP0911: found at slave addr 0x%02x "
			       "(7-bit 0x%02x)\n", addr, addr >> 1);
			return addr;
		}
	}
	printk(BIOS_WARNING, "ASP0911: device not found on O2_SMB1\n");
	return 0;
#endif
}

/*
 * Read the input voltage (12V supply) in millivolts via READ_VIN (0x88).
 */
int asp0911_get_vin_mv(u8 slave_addr, int *mv_out)
{
	u16 raw;
	if (kb3722q_pmbus_read_word(slave_addr, ASP0911_REG_READ_VIN, &raw))
		return -1;
	*mv_out = asp0911_linear_to_int(raw);
	return 0;
}

/*
 * Read the output current in milliamps via READ_IOUT (0x8C).
 */
int asp0911_get_iout_ma(u8 slave_addr, int *ma_out)
{
	u16 raw;
	if (kb3722q_pmbus_read_word(slave_addr, ASP0911_REG_READ_IOUT, &raw))
		return -1;
	*ma_out = asp0911_linear_to_int(raw);
	return 0;
}

/*
 * Read the output power in milliwatts via READ_POUT (0x96).
 */
int asp0911_get_pout_mw(u8 slave_addr, int *mw_out)
{
	u16 raw;
	if (kb3722q_pmbus_read_word(slave_addr, ASP0911_REG_READ_POUT, &raw))
		return -1;
	*mw_out = asp0911_linear_to_int(raw);
	return 0;
}

/*
 * Probe for the ASP0911, then log VIN, IOUT and POUT at BIOS_INFO level.
 * Called from mainboard ramstage init after memory and VRM are up.
 */
void asp0911_report(void)
{
	u8 addr;
	int vin, iout, pout;

	addr = asp0911_probe();
	if (!addr)
		return;

	if (asp0911_get_vin_mv(addr, &vin) == 0)
		printk(BIOS_INFO, "ASP0911: VIN  = %d mV\n", vin);

	if (asp0911_get_iout_ma(addr, &iout) == 0)
		printk(BIOS_INFO, "ASP0911: IOUT = %d mA\n", iout);

	if (asp0911_get_pout_mw(addr, &pout) == 0)
		printk(BIOS_INFO, "ASP0911: POUT = %d mW\n", pout);
}
