/*
 * BiosCallOuts.c - AGESA callback handlers
 */

#include <AGESA.h>
#include <amdlib.h>
#include <northbridge/amd/agesa/BiosCallOuts.h>
#include <SB800.h>
#include <stdlib.h>
#include <northbridge/amd/agesa/agesawrapper.h>

static AGESA_STATUS board_BeforeDramInit(UINT32 Func, UINTN Data, VOID *ConfigPtr)
{
	MEM_DATA_STRUCT *MemData = ConfigPtr;

	/* DDR3 voltage defaults to 1.5V (safe default) */
        /* TODO: implement VRM configuration if needed */
	MemData->ParameterListPtr->DDR3Voltage = VOLT1_5;

	return AGESA_SUCCESS;
}

const BIOS_CALLOUT_STRUCT BiosCallouts[] = {
	{ AGESA_DO_RESET,                      agesa_Reset },
	{ AGESA_READ_SPD,                      agesa_ReadSpd },
	{ AGESA_READ_SPD_RECOVERY,             agesa_NoopUnsupported },
	{ AGESA_RUNFUNC_ONAP,                  agesa_RunFuncOnAp },
	{ AGESA_HOOKBEFORE_DRAM_INIT,          board_BeforeDramInit },
	{ AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY, agesa_NoopSuccess },
	{ AGESA_HOOKBEFORE_DQS_TRAINING,       agesa_NoopSuccess },
	{ AGESA_HOOKBEFORE_EXIT_SELF_REF,      agesa_NoopSuccess },
};

const int BiosCalloutsLen = ARRAY_SIZE(BiosCallouts);
