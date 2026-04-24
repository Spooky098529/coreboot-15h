/* $NoKeywords:$ */
/**
 * @file
 *
 * mpRorC3.c
 *
 * Platform specific settings for OR C32 DDR3 R-DIMM system
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

#define FILECODE PROC_MEM_PS_OR_C32_MPRORC3_FILECODE
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
STATIC CONST PSCFG_SAO_ENTRY OrC32RDdr3SAO[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00112222},
  {.DimmPerCh = 1, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00222222},
  {.DimmPerCh = 1, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10112222},
  {.DimmPerCh = 1, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10222222},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003C3C3C, .ODC = 0x20112222},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003C3C3C, .ODC = 0x30222222},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3A3A, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3A3A, .ODC = 0x30222222},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393939, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3A3A, .ODC = 0x30222222},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393939, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393939, .ODC = 0x30332222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00112222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00222222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10222222},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10112222},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10222222},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x20222222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393C39, .ODC = 0x20112222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00393C39, .ODC = 0x20222222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3C3A, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00373A37, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00373A37, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00383A38, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00363936, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3C3A, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003A3C3A, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00383A38, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00383A38, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00373A37, .ODC = 0x30222222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00383A38, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00332222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10222222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x20222222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10222222},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00380038, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x10332222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x20222222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x003A003A, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x003A003A, .ODC = 0x20222222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00360036, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00360036, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00373C37, .ODC = 0x20332222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00373C37, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00383C38, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00333C33, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00353A35, .ODC = 0x30332222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00363A36, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00333933, .ODC = 0x30332222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00360036, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00360036, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00360036, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00360036, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00383C38, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00333C33, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00333C33, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00333C33, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00373C37, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00333C33, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00363A36, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00303A30, .ODC = 0x30112222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .SlowMode = 0, .AddTmgCtl = 0x00363A36, .ODC = 0x30222222},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .SlowMode = 0, .AddTmgCtl = 0x00363A36, .ODC = 0x30222222},
};
CONST PSC_TBL_ENTRY SAOTblEntRC32 = {
   .Header = {.PSCType = PSCFG_SAO, .DimmType = RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrC32RDdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   .TBLPtr = (VOID *)&OrC32RDdr3SAO
};
// training configuratrions
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   2D
//
STATIC CONST PSCFG_S___ENTRY OrC32RDdr3S__[] = {
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_35 + V1_25, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 1},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR + DIMM_DR + DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_35 + V1_25, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Enable__ = 1},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP + DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP + DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP + DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = NP + DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Enable__ = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Enable__ = 1},
  };
CONST PSC_TBL_ENTRY S__TblEntRC32 = {
   .Header = {.PSCType = PSCFG_S__, .DimmType = RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrC32RDdr3S__) / sizeof (PSCFG_S___ENTRY),
   .TBLPtr = (VOID *)&OrC32RDdr3S__
};
// ODT pattern for 1 DPC
// Format:
//  Dimm0,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_1D_ODTPAT_ENTRY Or1RDdr3OdtPat[] = {
  {.Dimm0 = DIMM_SR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00000001},
  {.Dimm0 = DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00000401},
  {.Dimm0 = DIMM_QR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000505, .WrODTCSLow = 0x00000505}
};
CONST PSC_TBL_ENTRY OdtPat1DTblEntRC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT_1D, .DimmType = RDIMM_TYPE, .NumOfDimm = _1DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or1RDdr3OdtPat) / sizeof (PSCFG_1D_ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or1RDdr3OdtPat
};

// ODT pattern for 2 DPC
// Format:
//  Dimm0,   Dimm1,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG____ODTPAT_ENTRY Or2RDdr3OdtPat[] = {
  {.Dimm0 = NP, .Dimm1 = DIMM_SR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00020000},
  {.Dimm0 = NP, .Dimm1 = DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x08020000},
  {.Dimm0 = NP, .Dimm1 = DIMM_QR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x020A0000, .WrODTCSLow = 0x080A0000},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x01010202, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x09030603},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .RdODTCSHigh = 0x01010000, .RdODTCSLow = 0x01010A0A, .WrODTCSHigh = 0x01090000, .WrODTCSLow = 0x01030E0B},
  {.Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000202, .RdODTCSLow = 0x05050202, .WrODTCSHigh = 0x00000206, .WrODTCSLow = 0x0D070203},
  {.Dimm0 = DIMM_QR, .Dimm1 = DIMM_QR, .RdODTCSHigh = 0x05050A0A, .RdODTCSLow = 0x05050A0A, .WrODTCSHigh = 0x050D0A0E, .WrODTCSLow = 0x05070A0B}
};
CONST PSC_TBL_ENTRY OdtPat2DTblEntRC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT___, .DimmType = RDIMM_TYPE, .NumOfDimm = _2DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or2RDdr3OdtPat) / sizeof (PSCFG____ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or2RDdr3OdtPat
};

// ODT pattern for 3 DPC
// Format:
//  Dimm0,   Dimm1,   Dimm2,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_3D_ODTPAT_ENTRY Or3RDdr3OdtPat[] = {
  {.Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000004, .WrODTCSLow = 0x00000000},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000101, .RdODTCSLow = 0x00000404, .WrODTCSHigh = 0x00000105, .WrODTCSLow = 0x00000405},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000303, .RdODTCSLow = 0x05050606, .WrODTCSHigh = 0x00000307, .WrODTCSLow = 0x0D070607},
  {.Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x020A0000, .WrODTCSLow = 0x080A0000},
  {.Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x04040A0A, .RdODTCSLow = 0x04040000, .WrODTCSHigh = 0x040C0A0E, .WrODTCSLow = 0x04060000},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x05050B0B, .RdODTCSLow = 0x05050E0E, .WrODTCSHigh = 0x050D0B0F, .WrODTCSLow = 0x05070E0F}
};
CONST PSC_TBL_ENTRY OdtPat3DTblEntRC32 = {
   .Header = {.PSCType = PSCFG_ODT_PAT_3D, .DimmType = RDIMM_TYPE, .NumOfDimm = _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or3RDdr3OdtPat) / sizeof (PSCFG_3D_ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or3RDdr3OdtPat
};

// Dram Term and Dynamic Dram Term
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   Dimm,   Rank,   RttNom,    RttWr
//
STATIC CONST PSCFG_RTT_ENTRY DramTermOrC32RDIMM[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 2, .RttWr = 2},
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 1, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 2, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 2, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP + DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP + DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP + DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP + DIMM_SR + DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP + DIMM_DR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP + DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP + DIMM_DR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP + DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP + DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP + DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R0 + R2, .RttNom = 1, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .Rank = R1 + R3, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_DR, .Rank = R1, .RttNom = 0, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
};
CONST PSC_TBL_ENTRY DramTermTblEntRC32 = {
   .Header = {.PSCType = PSCFG_RTT, .DimmType = RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (DramTermOrC32RDIMM) / sizeof (PSCFG_RTT_ENTRY),
   .TBLPtr = (VOID *)&DramTermOrC32RDIMM
};

// POR Max Freq.
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqOrC32RDIMM[] = {
  {._MAXFREQ_ENTRY = {.DimmPerCh = 1, .Dimms = 1, .SR = 1, .DR = 0, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 1, .Dimms = 1, .SR = 0, .DR = 1, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 1, .Dimms = 1, .SR = 0, .DR = 0, .QR = 1, .Speed1_5V = DDR1333_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 1, .SR = 1, .DR = 0, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 1, .SR = 0, .DR = 1, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 1, .SR = 0, .DR = 0, .QR = 1, .Speed1_5V = DDR1333_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 2, .DR = 0, .QR = 0, .Speed1_5V = DDR1333_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 1, .DR = 1, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 1, .DR = 0, .QR = 1, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR800_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 0, .DR = 2, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 0, .DR = 1, .QR = 1, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR800_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 0, .DR = 0, .QR = 2, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR800_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 1, .SR = 1, .DR = 0, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 1, .SR = 0, .DR = 1, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1333_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 1, .SR = 0, .DR = 0, .QR = 1, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR800_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .SR = 2, .DR = 0, .QR = 0, .Speed1_5V = DDR1333_FREQUENCY, .Speed1_35V = DDR1333_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .SR = 1, .DR = 1, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .SR = 1, .DR = 0, .QR = 1, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR800_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .SR = 0, .DR = 2, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR1066_FREQUENCY, .Speed1_25V = DDR1066_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 2, .SR = 0, .DR = 1, .QR = 1, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR800_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 3, .SR = 3, .DR = 0, .QR = 0, .Speed1_5V = DDR1066_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR800_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 3, .SR = 2, .DR = 1, .QR = 0, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR667_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 3, .SR = 2, .DR = 0, .QR = 1, .Speed1_5V = DDR667_FREQUENCY, .Speed1_35V = DDR667_FREQUENCY, .Speed1_25V = DDR667_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 3, .SR = 1, .DR = 2, .QR = 0, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR667_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 3, .SR = 1, .DR = 1, .QR = 1, .Speed1_5V = DDR667_FREQUENCY, .Speed1_35V = DDR667_FREQUENCY, .Speed1_25V = DDR667_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 3, .SR = 0, .DR = 3, .QR = 0, .Speed1_5V = DDR800_FREQUENCY, .Speed1_35V = DDR800_FREQUENCY, .Speed1_25V = DDR667_FREQUENCY}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 3, .Dimms = 3, .SR = 0, .DR = 2, .QR = 1, .Speed1_5V = DDR667_FREQUENCY, .Speed1_35V = DDR667_FREQUENCY, .Speed1_25V = DDR667_FREQUENCY}}
};
CONST PSC_TBL_ENTRY MaxFreqTblEntRC32 = {
   .Header = {.PSCType = PSCFG_MAXFREQ, .DimmType = RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (MaxFreqOrC32RDIMM) / sizeof (PSCFG_MAXFREQ_ENTRY),
   .TBLPtr = (VOID *)&MaxFreqOrC32RDIMM
};

// RC2[IBT]
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   Dimm,   NumOfReg,   IBT
//
STATIC CONST PSCFG_MR2IBT_ENTRY OrRDdr3RC2IBT[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 0xF, .IBT = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = DIMM_QR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 1},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600, .VDDIO = V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 0xF, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_SR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 8},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 8},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_SR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 8},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_SR + DIMM_DR + DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 8},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_QR, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 1},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 0xF, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 8},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = NP + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667 + DDR800, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP + DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP + DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP + DIMM_DR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP + DIMM_DR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 8},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP + DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP + DIMM_DR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP + DIMM_DR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = NP + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 3, .DDRrate = DDR1066 + DDR1333, .VDDIO = VOLT_ALL, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP + DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35 + V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 3, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 8},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR800, .VDDIO = V1_25, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 8},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR + DIMM_QR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR + DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_DR, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 1, .IBT = 0},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = NP, .Dimm1 = DIMM_QR, .Dimm2 = NP, .Dimm = DIMM_QR, .NumOfReg = 2, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1066, .VDDIO = V1_35, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR + DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_5 + V1_35, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_DR, .Dimm = DIMM_DR, .NumOfReg = 1, .IBT = 1},
  {.DimmPerCh = 3, .DDRrate = DDR1333, .VDDIO = V1_25, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = DIMM_SR, .Dimm = DIMM_SR, .NumOfReg = 1, .IBT = 1},
};
CONST PSC_TBL_ENTRY RC2IBTTblEntRC32 = {
   .Header = {.PSCType = PSCFG_RC2IBT, .DimmType = RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrRDdr3RC2IBT) / sizeof (PSCFG_MR2IBT_ENTRY),
   .TBLPtr = (VOID *)&OrRDdr3RC2IBT
};

// RC10[OperatingSpeed]
// Format :
// DDRrate,   Operating Speed
//
STATIC CONST PSCFG_OPSPD_ENTRY OrRDdr3OpSPD[] = {
  {.DDRrate = DDR667 + DDR800, .OPSPD = 0},
  {.DDRrate = DDR1066, .OPSPD = 1},
  {.DDRrate = DDR1333, .OPSPD = 2},
  {.DDRrate = DDR1600, .OPSPD = 3}
};
CONST PSC_TBL_ENTRY RC10OpSpdTblEntRC32 = {
   .Header = {.PSCType = PSCFG_RC10OPSPD, .DimmType = RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrRDdr3OpSPD) / sizeof (PSCFG_OPSPD_ENTRY),
   .TBLPtr = (VOID *)&OrRDdr3OpSPD
};

//
// MemClkDis
//
STATIC CONST UINT8 ROMDATA Or3RDdr3CLKDis[] = {0x03, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00};
CONST PSC_TBL_ENTRY ClkDisMapEntRC32 = {
   .Header = {.PSCType = PSCFG_CLKDIS, .DimmType = RDIMM_TYPE, .NumOfDimm = _1DIMM + _2DIMM + _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or3RDdr3CLKDis) / sizeof (UINT8),
   .TBLPtr = (VOID *)&Or3RDdr3CLKDis
};

//
// WL pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA WLPas1SeedOrC32RDIMM[] = {
  {.DimmPerCh = _1DIMM + _2DIMM + _3DIMM, .Channel = CH_ALL, .SeedVal = 0x3E}
};
CONST PSC_TBL_ENTRY WLPass1SeedEntRC32 = {
   .Header = {.PSCType = PSCFG_WL_PASS1_SEED, .DimmType = RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (WLPas1SeedOrC32RDIMM) / sizeof (PSCFG_SEED_ENTRY),
   .TBLPtr = (VOID *)&WLPas1SeedOrC32RDIMM
};

//
// HW RxEn pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA HWRxEnPas1SeedOrC32RDIMM[] = {
  {.DimmPerCh = _1DIMM + _2DIMM, .Channel = CH_A, .SeedVal = 0x3F},
  {.DimmPerCh = _1DIMM + _2DIMM, .Channel = CH_B, .SeedVal = 0x3E},
  {.DimmPerCh = _3DIMM, .Channel = CH_A, .SeedVal = 0x47},
  {.DimmPerCh = _3DIMM, .Channel = CH_B, .SeedVal = 0x38}
};
CONST PSC_TBL_ENTRY HWRxEnPass1SeedEntRC32 = {
   .Header = {.PSCType = PSCFG_HWRXEN_PASS1_SEED, .DimmType = RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_C32, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (HWRxEnPas1SeedOrC32RDIMM) / sizeof (PSCFG_SEED_ENTRY),
   .TBLPtr = (VOID *)&HWRxEnPas1SeedOrC32RDIMM
};
