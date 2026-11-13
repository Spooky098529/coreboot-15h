/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Board-level IT8620E handling for the GA-990FXA-UD3 R5: the Gigabyte
 * DualBIOS handshake, the EC watchdog re-arm and the vendor fan setup.
 * See PORTING_PLAN.md sections 5.1, 5.1a, 5.4 and 5.5.
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <superio/hwm5_conf.h>
#include <superio/ite/it8620e/it8620e.h>

#include "it8620e_board.h"

#define ITE_CFG_PORT	0x2e
#define ITE_DAT_PORT	0x2f

/* Raw ITE MB-PnP config access, as in the GA-78LMT-USB3 WIP board. */
static u8 ite_cr_read(u8 cr)
{
	outb(cr, ITE_CFG_PORT);
	return inb(ITE_DAT_PORT);
}

static void ite_cr_write(u8 cr, u8 val)
{
	outb(cr, ITE_CFG_PORT);
	outb(val, ITE_DAT_PORT);
}

static void ite_enter_ldn(u8 ldn)
{
	outb(0x87, ITE_CFG_PORT);	/* enter config: ITE MB-PnP key */
	outb(0x01, ITE_CFG_PORT);
	outb(0x55, ITE_CFG_PORT);
	outb(0x55, ITE_CFG_PORT);
	ite_cr_write(IT8620E_CONFIG_REG_LDN, ldn);
}

static void ite_exit(void)
{
	ite_cr_write(IT8620E_CONFIG_REG_CC, 0x02);	/* exit config */
}

/*
 * The IT8620E's LDN7 watchdog may be armed by the EC's own firmware out of
 * reset (Gigabyte DualBIOS boot-fail detection) and reset the board if it's
 * never serviced. Zeroing CR72/73/74 does not stop a counter the EC has
 * already started, so re-arm it to the maximum timeout instead (0xFFFF s,
 * ~18 h). Copied from the GA-78LMT-USB3 WIP board (same Super I/O).
 *
 * The "before" values logged below show whether the EC armed the watchdog
 * at power-on (Q3); the vendor-OS dumps can't, since the vendor BIOS has
 * already run by then.
 *
 * TODO(ud3r5): Q3 - stopgap; nothing services the watchdog after coreboot.
 * The vendor BIOS ends with CR73=CR74=0 (disabled). Add that as the disarm
 * in mainboard_final() once the first boot log shows the EC state.
 */
void it8620e_rearm_wdt_max(void)
{
	u8 b72, b73, b74, a72, a73, a74;

	ite_enter_ldn(IT8620E_GPIO);	/* LDN 7 (GPIO/WDT) */

	b72 = ite_cr_read(0x72);
	b73 = ite_cr_read(0x73);
	b74 = ite_cr_read(0x74);
	ite_cr_write(0x73, 0xff);	/* WDT timeout LSB -> max */
	ite_cr_write(0x74, 0xff);	/* WDT timeout MSB -> max (0xFFFF units) */
	ite_cr_write(0x72, 0x20);	/* WDT control last: enable, EC's seconds mode */
	a72 = ite_cr_read(0x72);
	a73 = ite_cr_read(0x73);
	a74 = ite_cr_read(0x74);

	ite_exit();

	printk(BIOS_INFO, "IT8620E WDT(LDN7): before CR72=%02x CR73=%02x CR74=%02x "
	       "(timeout=%u) -> rearm-max CR72=%02x CR73=%02x CR74=%02x (timeout=%u)\n",
	       b72, b73, b74, (b74 << 8) | b73, a72, a73, a74, (a74 << 8) | a73);
}

/*
 * Gigabyte DualBIOS handshake (reverse-engineered from DualBiosPei and
 * DualBiosDxe): LDN7 CR EF is a mailbox to Gigabyte's EC firmware.
 *   bit 0    flash select (vendor DXE toggles it for backup-chip access)
 *            - NEVER touched here
 *   bit 1    read-only "DualBIOS present"
 *   bits 3:2 set by PEI
 *   bits 6:4 boot stage: PEI writes 6, DXE writes 7
 *   bit 7    preserved
 * PEI: EF = (EF & 0x8F) | (6 << 4) | 0x0C
 * DXE: EF = (EF & 0x8F) | (7 << 4)
 * Vendor end state on hardware: CR EF = 0x7E.
 */
#define DUALBIOS_CR		0xef
#define DUALBIOS_KEEP_MASK	0x8f	/* bits 7, 3:0 */
#define DUALBIOS_PEI_BITS	0x0c	/* bits 3:2 */

void it8620e_dualbios_stage(u8 stage)
{
	u8 before, after, val;

	ite_enter_ldn(IT8620E_GPIO);	/* LDN 7 */

	before = ite_cr_read(DUALBIOS_CR);
	val = (before & DUALBIOS_KEEP_MASK) | ((stage & 0x7) << 4);
	if (stage == DUALBIOS_STAGE_PEI)
		val |= DUALBIOS_PEI_BITS;
	ite_cr_write(DUALBIOS_CR, val);
	after = ite_cr_read(DUALBIOS_CR);

	ite_exit();

	printk(BIOS_INFO, "IT8620E DualBIOS: CR EF %02x -> %02x (stage %u%s)\n",
	       before, after, stage,
	       (after & (1 << 1)) ? "" : ", DualBIOS-present bit1 CLEAR?");
}

/*
 * Vendor fan setup, read back from the live board under the vendor BIOS
 * (dumps/hwtest-20261113-153848/info.txt, "EC regs 00-FF"). Written in this
 * order: config registers first, then the auto-curve blocks. The curve bytes
 * are copied verbatim - the IT8620E curve layout isn't fully understood
 * (FAN1's block is all zero yet the fan spins), so don't "fix" them.
 *
 * Header mapping (cosmetic): per the manual only CPU_FAN and SYS_FAN1 (both
 * 4-pin) have speed control, CPU_FAN driven by CPU temperature and SYS_FAN1
 * by system temperature. FAN2 follows TMPIN1 (system), so FAN2 = SYS_FAN1;
 * CPU_FAN is FAN1 or FAN3 (both follow TMPIN3 = CPU). SYS_FAN2/PWR_FAN are
 * 3-pin, tach only.
 */
struct ec_reg {
	u8 reg;
	u8 val;
};

static const struct ec_reg vendor_fan_regs[] = {
	{ 0x13, 0x76 },	/* fan main control (tach enables, SmartGuardian) */
	{ 0x14, 0xc7 },	/* fan ctl mode / PWM polarity & clock */
	{ 0x0c, 0x30 },	/* tach 16-bit enables */
	{ 0x15, 0x82 },	/* FAN1 PWM: automatic, TMPIN3 */
	{ 0x16, 0x80 },	/* FAN2 PWM: automatic, TMPIN1 */
	{ 0x17, 0x82 },	/* FAN3 PWM: automatic, TMPIN3 */
	/* FAN1 auto curve 0x60-0x67 */
	{ 0x60, 0x00 }, { 0x61, 0x00 }, { 0x62, 0x00 }, { 0x63, 0x00 },
	{ 0x64, 0x00 }, { 0x65, 0x00 }, { 0x66, 0x00 }, { 0x67, 0x00 },
	/* FAN2 auto curve 0x68-0x6F */
	{ 0x68, 0x00 }, { 0x69, 0x14 }, { 0x6a, 0x32 }, { 0x6b, 0x46 },
	{ 0x6c, 0x20 }, { 0x6d, 0x03 }, { 0x6e, 0x00 }, { 0x6f, 0x00 },
	/* FAN3 auto curve 0x70-0x77 */
	{ 0x70, 0x00 }, { 0x71, 0x14 }, { 0x72, 0x41 }, { 0x73, 0x60 },
	{ 0x74, 0x1c }, { 0x75, 0x03 }, { 0x76, 0x00 }, { 0x77, 0x3f },
};

/*
 * Temperature-monitoring setup the fan curves depend on. Without it every
 * TMPIN reads -128 C under coreboot and the curves sit at minimum (seen on
 * hardware: CPU fan 333 rpm, SYS_FAN1 stopped). Values:
 *  - vendor end state from the live dump (dumps/hwtest-20261113-153848),
 *    for registers that differed under coreboot;
 *  - external-sensor host registers 0x89-0x9E from the vendor PEI EC init
 *    table (CORE_PEI, table at 0xFFFB0604), since their end-state values
 *    include live status/data bytes.
 * Reg 0x00 is handled separately (START bit only).
 */
static const struct ec_reg vendor_temp_regs[] = {
	{ 0x04, 0xff },	/* SMI# mask 1 (all masked) */
	{ 0x05, 0xef },	/* SMI# mask 2 */
	{ 0x0a, 0x48 },	/* interface select (external temp source) */
	{ 0x0b, 0x05 },	/* fan PWM smoothing / FAN4-5 source */
	{ 0x55, 0x80 },
	{ 0x5c, 0x65 },
	{ 0x84, 0xff }, { 0x85, 0xff }, { 0x86, 0xff }, { 0x87, 0xff },
	{ 0x89, 0x30 }, { 0x8a, 0x01 }, { 0x8b, 0x02 }, { 0x8c, 0x01 },
	{ 0x8d, 0x00 }, { 0x8e, 0xe0 },
	{ 0x90, 0xff }, { 0x91, 0x00 }, { 0x92, 0x00 },
	{ 0x94, 0xff }, { 0x95, 0x00 }, { 0x96, 0x00 },
	{ 0x98, 0x40 }, { 0x99, 0x97 }, { 0x9c, 0x40 }, { 0x9e, 0xc2 },
	{ 0x51, 0xca },	/* TMPIN1 thermistor, TMPIN2 diode, TMPIN3 = external (CPU) */
};

#define EC_REG_CONFIG		0x00
#define EC_CONFIG_START		(1 << 0)
#define EC_CONFIG_INIT		(1 << 4)	/* write 1 = reset; never write it */
#define EC_REG_TMPIN(n)		(0x28 + (n))	/* TMPIN1..3 = 0x29..0x2b */
#define EC_TEMP_INVALID		0x80		/* reads as -128 C */

/*
 * Full-speed fallback. PORTING_PLAN.md suggests 0x15-0x17 = 0x7F, but the
 * IT8620E has 8-bit PWM (SUPERIO_ITE_ENV_CTRL_8BIT_PWM): in software mode
 * the duty cycle lives in 0x63/0x6B/0x73, not in 0x15-0x17 bits 6:0, and
 * the vendor leaves 0x63 = 0. So set the 8-bit duty to 0xFF first, then
 * switch to software mode with 0x7F (full scale on 7-bit parts as well).
 */
static void it8620e_fans_full_speed(u16 base)
{
	static const u8 duty_regs[] = { 0x63, 0x6b, 0x73 };
	static const u8 ctl_regs[] = { 0x15, 0x16, 0x17 };
	size_t i;

	for (i = 0; i < ARRAY_SIZE(duty_regs); i++)
		pnp_write_hwm5_index(base, duty_regs[i], 0xff);
	for (i = 0; i < ARRAY_SIZE(ctl_regs); i++)
		pnp_write_hwm5_index(base, ctl_regs[i], 0x7f);

	printk(BIOS_WARNING, "IT8620E EC: fans forced to FULL SPEED (software mode, duty 0xff)\n");
}

/*
 * Must run AFTER the IT8620E chip driver's ite_ec_init() (device init),
 * because that clears reg 0x13 and rewrites 0x14 and 0x50.
 */
void it8620e_program_vendor_fans(u16 base)
{
	size_t i;
	u8 val, t1 = EC_TEMP_INVALID, t3 = EC_TEMP_INVALID;
	int bad = 0, ms;

	for (i = 0; i < ARRAY_SIZE(vendor_temp_regs); i++)
		pnp_write_hwm5_index(base, vendor_temp_regs[i].reg,
				     vendor_temp_regs[i].val);

	for (i = 0; i < ARRAY_SIZE(vendor_fan_regs); i++)
		pnp_write_hwm5_index(base, vendor_fan_regs[i].reg,
				     vendor_fan_regs[i].val);

	/* Start monitoring. Not bit 1 (SMI# enable): coreboot has no handler. */
	val = pnp_read_hwm5_index(base, EC_REG_CONFIG);
	pnp_write_hwm5_index(base, EC_REG_CONFIG,
			     (val & ~EC_CONFIG_INIT) | EC_CONFIG_START);

	for (i = 0; i < ARRAY_SIZE(vendor_fan_regs); i++) {
		val = pnp_read_hwm5_index(base, vendor_fan_regs[i].reg);
		if (val != vendor_fan_regs[i].val) {
			printk(BIOS_WARNING, "IT8620E EC: reg %02x reads %02x, wrote %02x\n",
			       vendor_fan_regs[i].reg, val, vendor_fan_regs[i].val);
			bad = 1;
		}
	}

	/* The curves follow TMPIN1 (FAN2) and TMPIN3 (FAN1/FAN3): both must report. */
	for (ms = 0; ms < 3000; ms += 100) {
		t1 = pnp_read_hwm5_index(base, EC_REG_TMPIN(1));
		t3 = pnp_read_hwm5_index(base, EC_REG_TMPIN(3));
		if (t1 != EC_TEMP_INVALID && t3 != EC_TEMP_INVALID)
			break;
		mdelay(100);
	}
	printk(BIOS_INFO, "IT8620E EC: reg00=%02x TMPIN1=%d C TMPIN3=%d C (after %d ms)\n",
	       pnp_read_hwm5_index(base, EC_REG_CONFIG), (s8)t1, (s8)t3, ms);
	if (t1 == EC_TEMP_INVALID || t3 == EC_TEMP_INVALID) {
		printk(BIOS_WARNING, "IT8620E EC: temperature inputs not reporting\n");
		bad = 1;
	}

	if (bad) {
		printk(BIOS_WARNING, "IT8620E EC: fan curves can't be trusted\n");
		it8620e_fans_full_speed(base);
		return;
	}

	printk(BIOS_INFO, "IT8620E EC @0x%x: vendor fan configuration applied\n", base);
}

/*
 * GP17 as a GPIO output driven high, exactly as the vendor BIOS does on every
 * boot (CORE_PEI 0xfffadf22, pin table entry "17 25 7f 80 c0 7f 80", output
 * table entry "17 41"): CR25 bit7 (GPIO function), LDN7 CRC0 bit7 (simple
 * I/O), CRB0 bit7 = 0 (polarity), CRC8 bit7 (output), simple-I/O base 0x220
 * (LDN7 CR62/63), port 0x220 bit7 = 1. Vendor CR25 = 0x80; it was 0x00 under
 * coreboot, where the PS/2 port had no power. Needs LPC decode of 0x220.
 */
#define IT8620E_SIMPLE_IO_BASE	0x220

void it8620e_gp17_high(void)
{
	u8 cr25;

	ite_enter_ldn(IT8620E_GPIO);
	ite_cr_write(0x62, IT8620E_SIMPLE_IO_BASE >> 8);
	ite_cr_write(0x63, IT8620E_SIMPLE_IO_BASE & 0xff);
	cr25 = ite_cr_read(0x25);
	ite_cr_write(0x25, cr25 | 0x80);
	ite_cr_write(0xc0, ite_cr_read(0xc0) | 0x80);
	ite_cr_write(0xb0, ite_cr_read(0xb0) & 0x7f);
	ite_cr_write(0xc8, ite_cr_read(0xc8) | 0x80);
	ite_exit();

	outb(inb(IT8620E_SIMPLE_IO_BASE) | 0x80, IT8620E_SIMPLE_IO_BASE);

	printk(BIOS_INFO, "IT8620E: GP17 output high (CR25 %02x -> %02x, port 0x220 = %02x)\n",
	       cr25, cr25 | 0x80, inb(IT8620E_SIMPLE_IO_BASE));
}
