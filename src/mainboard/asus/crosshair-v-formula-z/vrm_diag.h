#ifndef __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_DIAG_H__
#define __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_DIAG_H__

/*
 * vrm_pmbus_diag() — comprehensive PMBus diagnostic dump.
 *
 * Probes all known VRM devices on O2_SMB1 via the KB3722Q EC PMBus bridge,
 * reads every standard and vendor-specific register, and logs the results at
 * BIOS_INFO level.  Covers:
 *   - Device identity: PMBUS_REVISION, CAPABILITY, MFR_ID/MODEL/REVISION
 *   - VOUT encoding: VOUT_MODE + all VOUT configuration registers
 *   - All STATUS_* registers decoded to named fault bits
 *   - All telemetry READ_* registers with PMBus linear-format decoding
 *   - ASP1000C vendor-specific registers (0x73-0x76, 0x94, 0x9C-0x9F)
 *
 * Compiled only when CONFIG_BOARD_VRM_DIAG is set.
 */
void vrm_pmbus_diag(void);

#endif /* __MAINBOARD_ASUS_SABERTOOTH_990FX_R2_VRM_DIAG_H__ */
