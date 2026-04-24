/* $NoKeywords:$ */
/**
 * @file
 *
 * mpLorC3.c
 *
 * Platform specific settings for OR C32 DDR3 LRDIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/OR/C32)
 * @e \$Revision: 56315 $ @e \$Date: 2011-07-11 15:59:14 -0600 (Mon, 11 Jul 2011) $
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
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "mu.h"
#include "GeneralServices.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)


#define FILECODE PROC_MEM_PS_OR_C32_MPLORC3_FILECODE
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
STATIC CONST PSCFG_SAO_ENTRY OrC32LRDdr3SAO[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00112222},
  {.DimmPerCh = 1, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10112222},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003C3C3C, .ODC = 0x20112222},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3A3A, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393939, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393939, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393939, .ODC = 0x30332222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00112222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10222222},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10112222},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x20222222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393C39, .ODC = 0x20112222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3C3A, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00373A37, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00363936, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00383A38, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00332222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x20222222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00380038, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x10332222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x003A003A, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00360036, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00373C37, .ODC = 0x20332222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00383C38, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00353A35, .ODC = 0x30332222},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00333933, .ODC = 0x30332222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00360036, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00333C33, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .SlowMode = 0, .AddTmgCtl = 0x00363A36, .ODC = 0x30222222},
};
CONST PSC_TBL_ENTRY SAOTblEntLRC32 = {
   .Header = {.PSCType = PSCFG_SAO, .DimmType = LRDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrC32LRDdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   .TBLPtr = (VOID *)&OrC32LRDdr3SAO
};
// training configuratrions
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   2D
//
STATIC CONST PSCFG_S___ENTRY OrC32LRDdr3S__[] = {
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800 + DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .Enable__ = 1},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP + DIMM_LR, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP + DIMM_LR, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = NP + DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .Enable__ = 1},
  };
CONST PSC_TBL_ENTRY S__TblEntLRC32 = {
   .Header = {.PSCType = PSCFG_S__, .DimmType = LRDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrC32LRDdr3S__) / sizeof (PSCFG_S___ENTRY),
   .TBLPtr = (VOID *)&OrC32LRDdr3S__
};
// ODT pattern for 1 DPC
// Format:
//  Dimm0,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_1D_ODTPAT_ENTRY Or1LRDdr3OdtPat[] = {
  {.Dimm0 = DIMM_LR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000101, .WrODTCSLow = 0x00000101}
};
CONST PSC_TBL_ENTRY OdtPat1DTblEntLRC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT_1D, .DimmType = LRDIMM_TYPE, .NumOfDimm = _1DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or1LRDdr3OdtPat) / sizeof (PSCFG_1D_ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or1LRDdr3OdtPat
};

// ODT pattern for 2 DPC
// Format:
//  Dimm0,   Dimm1,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG____ODTPAT_ENTRY Or2LRDdr3OdtPat[] = {
  {.Dimm0 = NP, .Dimm1 = DIMM_LR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x02020000, .WrODTCSLow = 0x02020000},
  {.Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .RdODTCSHigh = 0x01010202, .RdODTCSLow = 0x01010202, .WrODTCSHigh = 0x03030303, .WrODTCSLow = 0x03030303}
};
CONST PSC_TBL_ENTRY OdtPat2DTblEntLRC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT___, .DimmType = LRDIMM_TYPE, .NumOfDimm = _2DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or2LRDdr3OdtPat) / sizeof (PSCFG____ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or2LRDdr3OdtPat
};

// ODT pattern for 3 DPC
// Format:
//  Dimm0,   Dimm1,   Dimm2,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_3D_ODTPAT_ENTRY Or3LRDdr3OdtPat[] = {
  {.Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000404, .WrODTCSLow = 0x00000000},
  {.Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .RdODTCSHigh = 0x00000101, .RdODTCSLow = 0x00000404, .WrODTCSHigh = 0x00000505, .WrODTCSLow = 0x00000505},
  {.Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .RdODTCSHigh = 0x00000303, .RdODTCSLow = 0x05050606, .WrODTCSHigh = 0x00000707, .WrODTCSLow = 0x07070707}
};
CONST PSC_TBL_ENTRY OdtPat3DTblEntLRC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT_3D, .DimmType = LRDIMM_TYPE, .NumOfDimm = _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or3LRDdr3OdtPat) / sizeof (PSCFG_3D_ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or3LRDdr3OdtPat
};

// Dram Term and Dynamic Dram Term
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,  RttNom,   RttWr
//
STATIC CONST PSCFG_LR_RTT_ENTRY DramTermOrC32LRDIMM[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP + DIMM_LR, .Dimm2 = DIMM_LR, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .RttNom = 5, .RttWr = 2},
};
CONST PSC_TBL_ENTRY DramTermTblEntLRC32 = {
   .Header = {.PSCType = PSCFG_LR_RTT, .DimmType = LRDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (DramTermOrC32LRDIMM) / sizeof (PSCFG_LR_RTT_ENTRY),
   .TBLPtr = (VOID *)&DramTermOrC32LRDIMM
};
// Max Freq.
// Format :
// DimmPerCh,   Dimms,  LR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_LR_MAXFREQ_ENTRY ROMDATA MaxFreqOrC32LRDIMM[] = {
  {._LR_MAXFREQ_ENTRY = {.DimmPerCh = 1, .Dimms = 1, .LR = 1, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._LR_MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 1, .LR = 1, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._LR_MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .LR = 2, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._LR_MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 1, .LR = 1, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._LR_MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .LR = 2, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._LR_MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 3, .LR = 3, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR667_FREQUENCY}}
};
CONST PSC_TBL_ENTRY MaxFreqTblEntLRC32 = {
   .Header = {.PSCType = PSCFG_LR_MAXFREQ, .DimmType = LRDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (MaxFreqOrC32LRDIMM) / sizeof (PSCFG_LR_MAXFREQ_ENTRY),
   .TBLPtr = (VOID *)&MaxFreqOrC32LRDIMM
};

// IBT
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   F0RC8,   F1RC0,   F1RC1,   F1RC2
//
STATIC CONST PSCFG_L_IBT_ENTRY OrLRDdr3IBT[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .F0RC8 = 0, .F1RC0 = 0, .F1RC1 = 0, .F1RC2 = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .F0RC8 = 0, .F1RC0 = 0, .F1RC1 = 0, .F1RC2 = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .F0RC8 = 0, .F1RC0 = 0, .F1RC1 = 0, .F1RC2 = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = NP, .F0RC8 = 0, .F1RC0 = 0, .F1RC1 = 0, .F1RC2 = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .F0RC8 = 0, .F1RC0 = 0, .F1RC1 = 0, .F1RC2 = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_LR, .Dimm2 = NP, .F0RC8 = 0, .F1RC0 = 0, .F1RC1 = 0, .F1RC2 = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = NP, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP + DIMM_LR, .Dimm2 = DIMM_LR, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800 + DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .F0RC8 = 0, .F1RC0 = 0, .F1RC1 = 0, .F1RC2 = 0},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP + DIMM_LR, .Dimm2 = DIMM_LR, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_LR, .F0RC8 = 0, .F1RC0 = 0, .F1RC1 = 0, .F1RC2 = 0},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_LR, .Dimm1 = DIMM_LR, .Dimm2 = DIMM_LR, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_LR, .Dimm1 = NP, .Dimm2 = DIMM_LR, .F0RC8 = 1, .F1RC0 = 1, .F1RC1 = 1, .F1RC2 = 1},
};
CONST PSC_TBL_ENTRY IBTTblEntLRC32 = {
   .Header = {.PSCType = PSCFG_LR_IBT, .DimmType = LRDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrLRDdr3IBT) / sizeof (PSCFG_L_IBT_ENTRY),
   .TBLPtr = (VOID *)&OrLRDdr3IBT
};

//
// MemClkDis
//
STATIC CONST UINT8 ROMDATA Or3LRDdr3CLKDis[] = {0x03, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00};
CONST PSC_TBL_ENTRY ClkDisMapEntLRC32 = {
   .Header = {.PSCType = PSCFG_CLKDIS, .DimmType = LRDIMM_TYPE, .NumOfDimm = _1DIMM + _2DIMM + _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or3LRDdr3CLKDis) / sizeof (UINT8),
   .TBLPtr = (VOID *)&Or3LRDdr3CLKDis
};

//
// WL pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA WLPas1SeedOrC32LRDIMM[] = {
  {.DimmPerCh = _1DIMM + _2DIMM + _3DIMM, .Channel = CH_ALL, .SeedVal = 0xF7}
};
CONST PSC_TBL_ENTRY WLPass1SeedEntLRC32 = {
   .Header = {.PSCType = PSCFG_WL_PASS1_SEED, .DimmType = LRDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (WLPas1SeedOrC32LRDIMM) / sizeof (PSCFG_SEED_ENTRY),
   .TBLPtr = (VOID *)&WLPas1SeedOrC32LRDIMM
};

//
// HW RxEn pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA HWRxEnPas1SeedOrC32LRDIMM[] = {
  {.DimmPerCh = _1DIMM + _2DIMM + _3DIMM, .Channel = CH_A, .SeedVal = 0x132},
  {.DimmPerCh = _1DIMM + _2DIMM + _3DIMM, .Channel = CH_B, .SeedVal = 0x122}
};
CONST PSC_TBL_ENTRY HWRxEnPass1SeedEntLRC32 = {
   .Header = {.PSCType = PSCFG_HWRXEN_PASS1_SEED, .DimmType = LRDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (HWRxEnPas1SeedOrC32LRDIMM) / sizeof (PSCFG_SEED_ENTRY),
   .TBLPtr = (VOID *)&HWRxEnPas1SeedOrC32LRDIMM
};