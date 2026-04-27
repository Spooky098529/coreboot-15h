#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include "ec.h"

/* Maximum spin iterations before giving up on an EC poll */
#define KB3722Q_POLL_TIMEOUT  100000

static int kb3722q_wait_ibf_clear(void)
{
	int i;
	for (i = 0; i < KB3722Q_POLL_TIMEOUT; i++) {
		if (!(inb(KB3722Q_CMD_PORT) & KB3722Q_IBF))
			return 0;
	}
	printk(BIOS_WARNING, "KB3722Q: IBF timeout\n");
	return -1;
}

static int kb3722q_wait_obf_set(void)
{
	int i;
	for (i = 0; i < KB3722Q_POLL_TIMEOUT; i++) {
		if (inb(KB3722Q_CMD_PORT) & KB3722Q_OBF)
			return 0;
	}
	printk(BIOS_WARNING, "KB3722Q: OBF timeout\n");
	return -1;
}

static int kb3722q_read_reg(u8 reg, u8 *val)
{
	if (kb3722q_wait_ibf_clear())
		return -1;
	outb(KB3722Q_CMD_READ, KB3722Q_CMD_PORT);

	if (kb3722q_wait_ibf_clear())
		return -1;
	outb(reg, KB3722Q_DATA_PORT);

	if (kb3722q_wait_obf_set())
		return -1;
	*val = inb(KB3722Q_DATA_PORT);
	return 0;
}

static int kb3722q_write_reg(u8 reg, u8 val)
{
	if (kb3722q_wait_ibf_clear())
		return -1;
	outb(KB3722Q_CMD_WRITE, KB3722Q_CMD_PORT);

	if (kb3722q_wait_ibf_clear())
		return -1;
	outb(reg, KB3722Q_DATA_PORT);

	if (kb3722q_wait_ibf_clear())
		return -1;
	outb(val, KB3722Q_DATA_PORT);
	return 0;
}

static u8 cmos_read(u8 idx)
{
	/* Preserve the NMI-disable bit (bit7) from the current index state */
	u8 nmi = inb(0x70) & 0x80;
	outb(nmi | (idx & 0x7f), 0x70);
	return inb(0x71);
}

static void cmos_write(u8 idx, u8 val)
{
	u8 nmi = inb(0x70) & 0x80;
	outb(nmi | (idx & 0x7f), 0x70);
	outb(val, 0x71);
}

static int kb3722q_pmbus_wait_done(void)
{
	int retry;
	for (retry = 0; retry < KB3722Q_PMBUS_MAX_RETRIES; retry++) {
		int i;
		u8 status;
		for (i = 0; i < KB3722Q_POLL_TIMEOUT; i++) {
			if (kb3722q_read_reg(KB3722Q_PMBUS_REG_STATUS, &status))
				return -1;
			if (status != KB3722Q_PMBUS_STATUS_BUSY1 &&
			    status != KB3722Q_PMBUS_STATUS_BUSY2)
				break;
		}
		if (status == KB3722Q_PMBUS_STATUS_OK)
			return 0;
		if (status != KB3722Q_PMBUS_STATUS_BUSY1 &&
		    status != KB3722Q_PMBUS_STATUS_BUSY2) {
			printk(BIOS_WARNING,
			       "KB3722Q PMBus: error status 0x%02x\n", status);
			return -1;
		}
	}
	printk(BIOS_WARNING, "KB3722Q PMBus: busy timeout after %d retries\n",
	       KB3722Q_PMBUS_MAX_RETRIES);
	return -1;
}

int kb3722q_pmbus_write_byte(u8 slave_write_addr, u8 pmbus_reg, u8 val)
{
	kb3722q_write_reg(KB3722Q_PMBUS_REG_STATUS, 0x00);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_SLAVE, slave_write_addr);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_CMD, pmbus_reg);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_DATA0, val);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_TRIGGER, KB3722Q_PMBUS_TX_WRITE_BYTE);
	return kb3722q_pmbus_wait_done();
}

int kb3722q_pmbus_write_word(u8 slave_write_addr, u8 pmbus_reg, u16 val)
{
	kb3722q_write_reg(KB3722Q_PMBUS_REG_STATUS, 0x00);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_SLAVE, slave_write_addr);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_CMD, pmbus_reg);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_DATA0, val & 0xff);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_DATA1, (val >> 8) & 0xff);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_TRIGGER, KB3722Q_PMBUS_TX_WRITE_WORD);
	return kb3722q_pmbus_wait_done();
}

int kb3722q_pmbus_read_byte(u8 slave_write_addr, u8 pmbus_reg, u8 *val)
{
	kb3722q_write_reg(KB3722Q_PMBUS_REG_STATUS, 0x00);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_SLAVE, slave_write_addr);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_CMD, pmbus_reg);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_TRIGGER, KB3722Q_PMBUS_TX_READ_BYTE);
	if (kb3722q_pmbus_wait_done())
		return -1;
	return kb3722q_read_reg(KB3722Q_PMBUS_REG_DATA0, val);
}

int kb3722q_pmbus_read_word(u8 slave_write_addr, u8 pmbus_reg, u16 *val)
{
	u8 lo, hi;
	kb3722q_write_reg(KB3722Q_PMBUS_REG_STATUS, 0x00);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_SLAVE, slave_write_addr);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_CMD, pmbus_reg);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_TRIGGER, KB3722Q_PMBUS_TX_READ_WORD);
	if (kb3722q_pmbus_wait_done())
		return -1;
	if (kb3722q_read_reg(KB3722Q_PMBUS_REG_DATA0, &lo))
		return -1;
	if (kb3722q_read_reg(KB3722Q_PMBUS_REG_DATA1, &hi))
		return -1;
	*val = ((u16)hi << 8) | lo;
	return 0;
}

/* EC stores block-read byte count at reg 0x24; payload starts at reg 0x04 */
int kb3722q_pmbus_read_block(u8 slave_write_addr, u8 pmbus_reg,
			     u8 *buf, u8 *len_out)
{
	u8 count, i;

	kb3722q_write_reg(KB3722Q_PMBUS_REG_STATUS, 0x00);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_SLAVE,  slave_write_addr);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_CMD,    pmbus_reg);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_BLKLEN, KB3722Q_PMBUS_BLOCK_MAXLEN);
	kb3722q_write_reg(KB3722Q_PMBUS_REG_TRIGGER, KB3722Q_PMBUS_TX_BLOCK_RD);
	if (kb3722q_pmbus_wait_done())
		return -1;
	if (kb3722q_read_reg(KB3722Q_PMBUS_REG_BLKLEN, &count))
		return -1;
	if (count == 0 || count > KB3722Q_PMBUS_BLOCK_MAXLEN) {
		*len_out = 0;
		return -1;
	}
	*len_out = count;
	for (i = 0; i < count; i++) {
		if (kb3722q_read_reg(KB3722Q_PMBUS_REG_DATA0 + i, &buf[i]))
			return -1;
	}
	return 0;
}

/* set KB3722Q EC fan controller to auto mode before IT8721F SmartGuardian takes over */
void kb3722q_ec_init(void)
{
	u8 val;

	if (kb3722q_read_reg(KB3722Q_REG_FAN_MODE, &val) == 0)
		kb3722q_write_reg(KB3722Q_REG_FAN_MODE, val & ~(1 << 0));

	if (kb3722q_read_reg(KB3722Q_REG_FAN_SPEED, &val) == 0)
		kb3722q_write_reg(KB3722Q_REG_FAN_SPEED, val & ~0x0f);

	printk(BIOS_DEBUG, "KB3722Q: fan controller set to auto mode\n");
}

/* clear MemOK! LED and acknowledge any pending retry */
void kb3722q_post_memory(void)
{
	u8 cmos_val;

	kb3722q_write_reg(KB3722Q_REG_MEMOK_LED, 0x00);

	cmos_val = cmos_read(CMOS_MEMOK_IDX);
	if (cmos_val & CMOS_MEMOK_PEND) {
		kb3722q_write_reg(KB3722Q_REG_MEMOK_ACK, 0x00);
		cmos_write(CMOS_MEMOK_IDX, cmos_val & ~CMOS_MEMOK_PEND);
		printk(BIOS_INFO,
		       "KB3722Q: MemOK! retry complete, acknowledged success\n");
	} else {
		printk(BIOS_DEBUG, "KB3722Q: memory OK, LED cleared\n");
	}
}
