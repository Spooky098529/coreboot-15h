/*
 * ENE KB3722Q Embedded Controller — firmware protocol definitions.
 *
 * The KB3722Q (component O2U2) is a dedicated EC that:
 *   - Masters the VRM SMBus (O2_SMB1), bridging PMBus commands from the BIOS
 *     to the ASP1000C CPU VCore and ASP1103 DRAM VDD regulators.
 *   - Manages the four diagnostic POST LEDs (CPU/DRAM/VGA/BOOT) autonomously
 *     based on POST progress signals received from the BIOS.
 *   - Controls fan operating modes on boot.
 *
 * EC firmware identification string: "MBECI-0006" (via command 0xA1).
 *
 * The KB3722Q uses the standard ACPI EC protocol on ports 0x62/0x66, separate
 * from the IT8721F i8042 KBC which uses 0x60/0x64.  The OS does not interact
 * with the KB3722Q; all communication is firmware-internal.
 */

#ifndef __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_EC_H__
#define __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_EC_H__

#include <stdint.h>

/* EC I/O ports */
#define KB3722Q_DATA_PORT  0x62
#define KB3722Q_CMD_PORT   0x66

/* EC status register bits (read from CMD_PORT) */
#define KB3722Q_IBF  (1 << 1)  /* Input Buffer Full  — poll for 0 before writing */
#define KB3722Q_OBF  (1 << 0)  /* Output Buffer Full — poll for 1 before reading */

/* EC command bytes (written to CMD_PORT, then operands to DATA_PORT) */
#define KB3722Q_CMD_READ   0x80  /* Read reg:  [cmd][reg_addr] → read data from 0x62 */
#define KB3722Q_CMD_WRITE  0x81  /* Write reg: [cmd][reg_addr][data] */

/* EC register map (accessed via CMD_READ/CMD_WRITE above) */
#define KB3722Q_REG_MEMOK_LED    0x7E  /* W: 0x00 = MemOK! LED off (memory OK) */
#define KB3722Q_REG_MEMOK_COUNT  0x7F  /* R: boot retry counter (0–4) */
#define KB3722Q_REG_MEMOK_ACK    0x93  /* W: 0x00 = signal MemOK! success to EC */
#define KB3722Q_REG_FAN_MODE     0xD2  /* RMW: bit0=0 → fan controller auto mode */
#define KB3722Q_REG_MEMOK_FLAG   0xD3  /* R: bit1 = MemOK! retry sequence active */
#define KB3722Q_REG_FAN_SPEED    0xD6  /* RMW: bits[3:0]=0 → all fans auto speed */

/* CMOS register tracking MemOK! state (used by Memory_OK_DXE) */
#define CMOS_MEMOK_IDX   0x46
#define CMOS_MEMOK_PEND  (1 << 7)  /* Set when MemOK! retry is pending */

/*
 * KB3722Q PMBus bridge — EC register overlay for O2_SMB1 transactions.
 *
 * Accessed via CMD_READ/CMD_WRITE above.  Write the slave address, command,
 * and data bytes, then write the transaction type to reg 0x00 to trigger.
 * Poll reg 0x01 for completion: 0x00 = success, 0x1A/0x18 = busy (retry),
 * any other value = error.
 *
 * Transaction types (written to reg 0x00):
 *   0x06  Write Byte   — sends slave + cmd + 1 data byte (reg 0x04)
 *   0x07  Read Byte    — sends slave + cmd, reads 1 byte from reg 0x04
 *   0x08  Write Word   — sends slave + cmd + 2 data bytes (regs 0x04-0x05)
 *   0x09  Read Word    — sends slave + cmd, reads 2 bytes from regs 0x04-0x05
 *   0x0A  Block Write  — byte count in reg 0x24; data in regs 0x04..0x04+N
 *   0x0B  Block Read   — byte count in reg 0x24; results in regs 0x04..0x04+N
 *
 * The PMBus slave address is the 8-bit write format (7-bit addr << 1).
 */
#define KB3722Q_PMBUS_REG_TRIGGER  0x00
#define KB3722Q_PMBUS_REG_STATUS   0x01
#define KB3722Q_PMBUS_REG_SLAVE    0x02
#define KB3722Q_PMBUS_REG_CMD      0x03
#define KB3722Q_PMBUS_REG_DATA0    0x04
#define KB3722Q_PMBUS_REG_DATA1    0x05
#define KB3722Q_PMBUS_REG_BLKLEN   0x24

#define KB3722Q_PMBUS_TX_WRITE_BYTE  0x06
#define KB3722Q_PMBUS_TX_READ_BYTE   0x07
#define KB3722Q_PMBUS_TX_WRITE_WORD  0x08
#define KB3722Q_PMBUS_TX_READ_WORD   0x09
#define KB3722Q_PMBUS_TX_BLOCK_WR    0x0A
#define KB3722Q_PMBUS_TX_BLOCK_RD    0x0B

#define KB3722Q_PMBUS_STATUS_OK      0x00
#define KB3722Q_PMBUS_STATUS_BUSY1   0x1A
#define KB3722Q_PMBUS_STATUS_BUSY2   0x18
#define KB3722Q_PMBUS_MAX_RETRIES    10
#define KB3722Q_PMBUS_BLOCK_MAXLEN   32

void kb3722q_ec_init(void);
void kb3722q_post_memory(void);

int kb3722q_pmbus_write_byte(u8 slave_write_addr, u8 pmbus_reg, u8 val);
int kb3722q_pmbus_write_word(u8 slave_write_addr, u8 pmbus_reg, u16 val);
int kb3722q_pmbus_read_byte(u8 slave_write_addr, u8 pmbus_reg, u8 *val);
int kb3722q_pmbus_read_word(u8 slave_write_addr, u8 pmbus_reg, u16 *val);
/*
 * Block read from a PMBus slave via the EC bridge (transaction type 0x0B).
 * buf must be at least KB3722Q_PMBUS_BLOCK_MAXLEN bytes.  On success, *len_out
 * is the number of bytes written to buf (as reported by the EC in reg 0x24).
 * Returns 0 on success, -1 on EC communication error or malformed response.
 *
 * Note: the EC firmware stores the device-returned byte-count in reg 0x24 and
 * the payload in regs 0x04..0x04+N.  The PMBus byte-count byte from the slave
 * is consumed by the EC and not repeated in the data buffer.
 */
int kb3722q_pmbus_read_block(u8 slave_write_addr, u8 pmbus_reg,
			     u8 *buf, u8 *len_out);

#endif /* __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_EC_H__ */
