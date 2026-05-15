/*
 * BiosCallOuts.c - AGESA callback handlers
 */

#include <AGESA.h>
#include <amdlib.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <SB800.h>
#include <stdlib.h>
#include <northbridge/amd/agesa/agesawrapper.h>

const BIOS_CALLOUT_STRUCT BiosCallouts[] = {
	{ AGESA_DO_RESET,                      agesa_Reset },
	{ AGESA_READ_SPD,                      agesa_ReadSpd },
	{ AGESA_READ_SPD_RECOVERY,             agesa_NoopUnsupported },
	{ AGESA_RUNFUNC_ONAP,                  agesa_RunFuncOnAp },
	{ AGESA_HOOKBEFORE_DRAM_INIT,          agesa_NoopSuccess },
	{ AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY, agesa_NoopSuccess },
	{ AGESA_HOOKBEFORE_DQS_TRAINING,       agesa_NoopSuccess },
	{ AGESA_HOOKBEFORE_EXIT_SELF_REF,      agesa_NoopSuccess },
};

const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);
