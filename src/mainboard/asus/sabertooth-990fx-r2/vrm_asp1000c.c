/*
 * ASP1000C CPU VCore controller driver for ASUS Sabertooth 990FX R2.0.
 *
 * The ASP1000C (Chil/Intersil multi-phase PWM) is bridged to coreboot via
 * the ENE KB3722Q embedded controller's PMBus bridge on O2_SMB1.
 *
 * Persistent-state warning: writing ASP1000C_VENDOR_UNLOCK_KEY (0x29) to
 * register 0x94 latches the chip into "vendor OC write mode".  This state
 * survives warm reboot and only clears on AC power loss.  Leaving the chip
 * in OC mode unconditionally produced a fault-mode where the system boots
 * but everything (CPU, UART) ran at a fraction of normal speed — visible
 * as ~100ms-per-character serial output that persisted across reboots until
 * AC was removed.  asp1000c_init() therefore does NOT unlock; the unlock is
 * isolated to asp1000c_oc_unlock() and must be paired with a lock afterward.
 */

#include <stdint.h>
#include <console/console.h>
#include "ec.h"
#include "vrm_asp1000c.h"

/* write 0x00 to vendor-specific reg 0x1C; function undocumented, must run before AGESA */
int asp1000c_init(void)
{
	int ret = kb3722q_pmbus_write_byte(ASP1000C_SLAVE_ADDR,
					   ASP1000C_REG_INIT_RESET, 0x00);
	if (ret) {
		printk(BIOS_WARNING,
		       "ASP1000C: init write (reg 0x%02x) failed (%d)\n",
		       ASP1000C_REG_INIT_RESET, ret);
		return ret;
	}

	printk(BIOS_DEBUG, "ASP1000C: init OK (slave 0x%02x)\n",
	       ASP1000C_SLAVE_ADDR);
	return 0;
}

/*
 * Enter OC write mode by writing the vendor unlock key to register 0x94.
 * Must be paired with asp1000c_oc_lock() — see header comment for why.
 */
int asp1000c_oc_unlock(void)
{
	int ret = kb3722q_pmbus_write_byte(ASP1000C_SLAVE_ADDR,
					   ASP1000C_REG_VENDOR_UNLOCK,
					   ASP1000C_VENDOR_UNLOCK_KEY);
	if (ret)
		printk(BIOS_WARNING,
		       "ASP1000C: OC unlock failed (%d)\n", ret);
	return ret;
}

/* exit OC write mode by clearing register 0x94 */
int asp1000c_oc_lock(void)
{
	int ret = kb3722q_pmbus_write_byte(ASP1000C_SLAVE_ADDR,
					   ASP1000C_REG_VENDOR_UNLOCK, 0x00);
	if (ret)
		printk(BIOS_WARNING,
		       "ASP1000C: OC lock failed (%d)\n", ret);
	return ret;
}

/*
 * Read the current VCore output voltage via READ_VOUT (PMBus reg 0x8B).
 * Decodes the 2-byte PMBus linear format and writes the result in mV
 * to *mv_out.  Returns 0 on success, -1 on communication error.
 */
int asp1000c_get_vcore_mv(int *mv_out)
{
	u16 raw;
	int ret;

	ret = kb3722q_pmbus_read_word(ASP1000C_SLAVE_ADDR,
				      ASP1000C_REG_READ_VOUT, &raw);
	if (ret) {
		printk(BIOS_WARNING,
		       "ASP1000C: READ_VOUT failed (%d)\n", ret);
		return -1;
	}

	*mv_out = asp1000c_linear_to_mv(raw);
	printk(BIOS_DEBUG, "ASP1000C: VCore = %d mV (raw 0x%04x)\n",
	       *mv_out, raw);
	return 0;
}

/*
 * Set the VCore target voltage by writing a VID code to VOUT_COMMAND (0x75).
 *
 * The VID code is a 1-byte vendor-specific value.  A value of 0x00 means
 * "no offset from nominal".  The exact encoding is board-specific; use
 * asp1000c_get_vcore_mv() before and after to verify the applied change.
 *
 * VOUT_MAX (0x76) and VOUT_MARGIN_HIGH/LOW (0x74/0x73) are left untouched
 * to keep the voltage within the limits already configured by AGESA.
 */
int asp1000c_set_vcore_vid(u8 vid)
{
	int ret = kb3722q_pmbus_write_byte(ASP1000C_SLAVE_ADDR,
					   ASP1000C_REG_VOUT_COMMAND, vid);
	if (ret)
		printk(BIOS_WARNING,
		       "ASP1000C: VOUT_COMMAND write failed (%d)\n", ret);
	else
		printk(BIOS_DEBUG,
		       "ASP1000C: VOUT_COMMAND set to VID 0x%02x\n", vid);
	return ret;
}
