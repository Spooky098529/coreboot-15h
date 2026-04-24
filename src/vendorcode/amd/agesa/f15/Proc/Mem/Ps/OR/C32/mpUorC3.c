/* $NoKeywords:$ */
/**
 * @file
 *
 * mpUorC3.c
 *
 * Platform specific settings for OR C32 DDR3 U-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/OR/C32)
 * @e \$Revision: 55134 $ @e \$Date: 2011-06-16 15:27:02 -0600 (Thu, 16 Jun 2011) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  * ***************************************************************************
  *
 */

#include "AGESA.h"
#include "AdvancedApi.h"
#include "PlatformMemoryConfiguration.h"
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_OR_C32_MPUORC3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
// Slow mode, Address timing and Output drive compensation
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   SlowMode,   AddTmgCtl,  ODC
//
STATIC CONST PSCFG_SAO_ENTRY OrC32UDdr3SAO[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00112222},
  {.DimmPerCh = 1, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003B0000, .ODC = 0x00112222},
  {.DimmPerCh = 1, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10112222},
  {.DimmPerCh = 1, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003B0000, .ODC = 0x10112222},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00383837, .ODC = 0x20112222},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00363635, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00353533, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00003533, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00353533, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00003533, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00333330, .ODC = 0x30332222},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00003330, .ODC = 0x30332222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00112222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003B0000, .ODC = 0x00112222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x10222222},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10112222},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003B0000, .ODC = 0x10112222},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x20222222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00383837, .ODC = 0x20112222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3A3A, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00363635, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00003939, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00353533, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00003533, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00003938, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00003938, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00003939, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00332222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x003B0000, .ODC = 0x00332222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x10222222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10332222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x003B0000, .ODC = 0x10332222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x20222222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00383837, .ODC = 0x20332222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x003A3A3A, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00363635, .ODC = 0x30332222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .SlowMode = 1, .AddTmgCtl = 0x00003939, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00353533, .ODC = 0x30332222},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .SlowMode = 1, .AddTmgCtl = 0x00003533, .ODC = 0x30332222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .SlowMode = 1, .AddTmgCtl = 0x00003938, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 1, .AddTmgCtl = 0x00003938, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .SlowMode = 1, .AddTmgCtl = 0x00003939, .ODC = 0x30222222},
};
CONST PSC_TBL_ENTRY SAOTblEntUC32 = {
   .Header = {.PSCType = PSCFG_SAO, .DimmType = UDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrC32UDdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   .TBLPtr = (VOID *)&OrC32UDdr3SAO
};
// training configuratrions
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   2D
//
STATIC CONST PSCFG_S___ENTRY OrC32UDdr3S__[] = {
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800 + DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Enable__ = 1},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Enable__ = 1},  };
CONST PSC_TBL_ENTRY S__TblEntUC32 = {
   .Header = {.PSCType = PSCFG_S__, .DimmType = UDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrC32UDdr3S__) / sizeof (PSCFG_S___ENTRY),
   .TBLPtr = (VOID *)&OrC32UDdr3S__
};
// ODT pattern for 1 DPC
// Format:
//  Dimm0,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_1D_ODTPAT_ENTRY Or1UDdr3OdtPat[] = {
  {.Dimm0 = DIMM_SR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00000001},
  {.Dimm0 = DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00000401}
};
CONST PSC_TBL_ENTRY OdtPat1DTblEntUC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT_1D, .DimmType = UDIMM_TYPE, .NumOfDimm = _1DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or1UDdr3OdtPat) / sizeof (PSCFG_1D_ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or1UDdr3OdtPat
};

// ODT pattern for 2 DPC
// Format:
//  Dimm0,   Dimm1,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG____ODTPAT_ENTRY Or2UDdr3OdtPat[] = {
  {.Dimm0 = NP, .Dimm1 = DIMM_SR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00020000},
  {.Dimm0 = NP, .Dimm1 = DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x08020000},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x01010202, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x09030603}
};
CONST PSC_TBL_ENTRY OdtPat2DTblEntUC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT___, .DimmType = UDIMM_TYPE, .NumOfDimm = _2DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or2UDdr3OdtPat) / sizeof (PSCFG____ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or2UDdr3OdtPat
};

// ODT pattern for 3 DPC
// Format:
//  Dimm0,   Dimm1,   Dimm2,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_3D_ODTPAT_ENTRY Or3UDdr3OdtPat[] = {
  {.Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000004, .WrODTCSLow = 0x00000000},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000101, .RdODTCSLow = 0x00000404, .WrODTCSHigh = 0x00000105, .WrODTCSLow = 0x00000405}
};
CONST PSC_TBL_ENTRY OdtPat3DTblEntUC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT_3D, .DimmType = UDIMM_TYPE, .NumOfDimm = _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or3UDdr3OdtPat) / sizeof (PSCFG_3D_ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or3UDdr3OdtPat
};

// Dram Term and Dynamic Dram Term
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   Dimm,   Rank,   RttNom,    RttWr
//
STATIC CONST PSCFG_RTT_ENTRY DramTermOrC32UDIMM[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
};
CONST PSC_TBL_ENTRY DramTermTblEntUC32 = {
   .Header = {.PSCType = PSCFG_RTT, .DimmType = UDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (DramTermOrC32UDIMM) / sizeof (PSCFG_RTT_ENTRY),
   .TBLPtr = (VOID *)&DramTermOrC32UDIMM
};

// Max Freq.
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqOrC32UDIMM[] = {
  {._MAXFREQ_ENTRY = {.DimmPerCh = 1, .Dimms = 1, .SR = 1, .DR = 0, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 1, .Dimms = 1, .SR = 0, .DR = 1, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 1, .SR = 1, .DR = 0, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 1, .SR = 0, .DR = 1, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 2, .DR = 0, .QR = 0, .Speed1_5V = DDR1333_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 1, .DR = 1, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 0, .DR = 2, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 1, .SR = 1, .DR = 0, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 1, .SR = 0, .DR = 1, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .SR = 2, .DR = 0, .QR = 0, .Speed1_5V = DDR1333_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .SR = 1, .DR = 1, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .SR = 0, .DR = 2, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}}
};
CONST PSC_TBL_ENTRY MaxFreqTblEntUC32 = {
   .Header = {.PSCType = PSCFG_MAXFREQ, .DimmType = UDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (MaxFreqOrC32UDIMM) / sizeof (PSCFG_MAXFREQ_ENTRY),
   .TBLPtr = (VOID *)&MaxFreqOrC32UDIMM
};

//
// MemClkDis [1DPC & 2DPC]
//
STATIC CONST UINT8 ROMDATA OrUDdr3CLKDis[] = {0x01, 0x04, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00};
CONST PSC_TBL_ENTRY ClkDisMapEntUC32 = {
   .Header = {.PSCType = PSCFG_CLKDIS, .DimmType = UDIMM_TYPE, .NumOfDimm = _1DIMM + _2DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrUDdr3CLKDis) / sizeof (UINT8),
   .TBLPtr = (VOID *)&OrUDdr3CLKDis
};

//
// MemClkDis [3DPC]
//
STATIC CONST UINT8 ROMDATA Or3UDdr3CLKDis[] = {0x01, 0x02, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00};
CONST PSC_TBL_ENTRY ClkDisMap3DEntUC32 = {
   .Header = {.PSCType = PSCFG_CLKDIS, .DimmType = UDIMM_TYPE, .NumOfDimm = _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or3UDdr3CLKDis) / sizeof (UINT8),
   .TBLPtr = (VOID *)&Or3UDdr3CLKDis
};

//
// WL pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA WLPas1SeedOrC32UDIMM[] = {
  {.DimmPerCh = _1DIMM + _2DIMM + _3DIMM, .Channel = CH_ALL, .SeedVal = 0x12}
};
CONST PSC_TBL_ENTRY WLPass1SeedEntUC32 = {
   .Header = {.PSCType = PSCFG_WL_PASS1_SEED, .DimmType = UDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (WLPas1SeedOrC32UDIMM) / sizeof (PSCFG_SEED_ENTRY),
   .TBLPtr = (VOID *)&WLPas1SeedOrC32UDIMM
};

//
// HW RxEn pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA HWRxEnPas1SeedOrC32UDIMM[] = {
  {.DimmPerCh = _1DIMM + _2DIMM, .Channel = CH_A, .SeedVal = 0x39},
  {.DimmPerCh = _1DIMM + _2DIMM, .Channel = CH_B, .SeedVal = 0x32},
  {.DimmPerCh = _3DIMM, .Channel = CH_A, .SeedVal = 0x45},
  {.DimmPerCh = _3DIMM, .Channel = CH_B, .SeedVal = 0x37}
};
CONST PSC_TBL_ENTRY HWRxEnPass1SeedEntUC32 = {
   .Header = {.PSCType = PSCFG_HWRXEN_PASS1_SEED, .DimmType = UDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (HWRxEnPas1SeedOrC32UDIMM) / sizeof (PSCFG_SEED_ENTRY),
   .TBLPtr = (VOID *)&HWRxEnPas1SeedOrC32UDIMM
};
