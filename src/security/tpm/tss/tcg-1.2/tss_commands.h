/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

struct s_tpm_extend_cmd{
	uint8_t buffer[34];
	uint16_t pcrNum;
	uint16_t inDigest;
};
extern const struct s_tpm_extend_cmd tpm_extend_cmd;

struct s_tpm_get_random_cmd{
	uint8_t buffer[14];
	uint16_t bytesRequested;
};

extern const struct s_tpm_get_random_cmd tpm_get_random_cmd;

struct s_tpm_getownership_cmd{
	uint8_t buffer[22];
};
extern const struct s_tpm_getownership_cmd tpm_getownership_cmd;

const struct s_tpm_getpermissions_cmd{
	uint8_t buffer[22];
	uint16_t index;
} tpm_getpermissions_cmd = {
	{0x0, 0xc1, 0x0, 0x0, 0x0, 0x16, 0x0, 0x0, 0x0, 0x65,
	0x0, 0x0, 0x0, 0x11, 0x0, 0x0, 0x0, 0x4, },
18, };

struct s_tpm_getstclearflags_cmd{
	uint8_t buffer[22];
};
extern const struct s_tpm_getstclearflags_cmd tpm_getstclearflags_cmd;

struct s_tpm_getflags_cmd{
	uint8_t buffer[22];
};
extern const struct s_tpm_getflags_cmd tpm_getflags_cmd;

struct s_tpm_physicalsetdeactivated_cmd{
	uint8_t buffer[11];
	uint16_t deactivated;
};
extern const struct s_tpm_physicalsetdeactivated_cmd tpm_physicalsetdeactivated_cmd;

struct s_tpm_physicalenable_cmd{
	uint8_t buffer[10];
};
extern const struct s_tpm_physicalenable_cmd tpm_physicalenable_cmd;

struct s_tpm_physicaldisable_cmd{
	uint8_t buffer[10];
};
extern const struct s_tpm_physicaldisable_cmd tpm_physicaldisable_cmd;

struct s_tpm_forceclear_cmd{
	uint8_t buffer[10];
};
extern const struct s_tpm_forceclear_cmd tpm_forceclear_cmd;

struct s_tpm_readpubek_cmd{
	uint8_t buffer[30];
};
extern const struct s_tpm_readpubek_cmd tpm_readpubek_cmd;

struct s_tpm_continueselftest_cmd{
	uint8_t buffer[10];
};
extern const struct s_tpm_continueselftest_cmd tpm_continueselftest_cmd;

struct s_tpm_selftestfull_cmd{
	uint8_t buffer[10];
};
extern const struct s_tpm_selftestfull_cmd tpm_selftestfull_cmd;

struct s_tpm_resume_cmd{
	uint8_t buffer[12];
};
extern const struct s_tpm_resume_cmd tpm_resume_cmd;

struct s_tpm_savestate_cmd{
	uint8_t buffer[10];
};
extern const struct s_tpm_savestate_cmd tpm_savestate_cmd;

struct s_tpm_startup_cmd{
	uint8_t buffer[12];
};
extern const struct s_tpm_startup_cmd tpm_startup_cmd;

struct s_tpm_finalizepp_cmd{
	uint8_t buffer[12];
};
extern const struct s_tpm_finalizepp_cmd tpm_finalizepp_cmd;

struct s_tpm_pplock_cmd{
	uint8_t buffer[12];
};
extern const struct s_tpm_pplock_cmd tpm_pplock_cmd;

struct s_tpm_ppenable_cmd{
	uint8_t buffer[12];
};
extern const struct s_tpm_ppenable_cmd tpm_ppenable_cmd;

struct s_tpm_ppassert_cmd{
	uint8_t buffer[12];
};
extern const struct s_tpm_ppassert_cmd tpm_ppassert_cmd;

struct s_tpm_pcr_read_cmd{
	uint8_t buffer[14];
	uint16_t pcrNum;
};
extern const struct s_tpm_pcr_read_cmd tpm_pcr_read_cmd;

struct s_tpm_nv_read_cmd{
	uint8_t buffer[22];
	uint16_t index;
	uint16_t length;
};
extern const struct s_tpm_nv_read_cmd tpm_nv_read_cmd;

struct s_tpm_nv_write_cmd{
	uint8_t buffer[256];
	uint16_t index;
	uint16_t length;
	uint16_t data;
};
extern const struct s_tpm_nv_write_cmd tpm_nv_write_cmd;

struct s_tpm_nv_definespace_cmd{
	uint8_t buffer[101];
	uint16_t index;
	uint16_t perm;
	uint16_t size;
};
extern const struct s_tpm_nv_definespace_cmd tpm_nv_definespace_cmd;

extern const int kWriteInfoLength;
const int kNvDataPublicPermissionsOffset = 60;
