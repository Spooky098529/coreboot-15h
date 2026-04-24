/* $NoKeywords:$ */
/**
 * @file
 *
 * mpSorA3.c
 *
 * Platform specific settings for OR AM3 DDR3 SO-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/OR/AM3)
 * @e \$Revision: 50871 $ @e \$Date: 2011-04-14 15:39:51 -0600 (Thu, 14 Apr 2011) $
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
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "mu.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_OR_AM3_MPSORA3_FILECODE
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
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   SlowMode,   AddTmgCtl,   ODC
//
STATIC CONST PSCFG_SAO_ENTRY OrAM3SODdr3SAO[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00112222},
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003B0000, .ODC = 0x00112222},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10112222},
  {.DimmPerCh = 1, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00380000, .ODC = 0x10112222},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x20112222},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00360000, .ODC = 0x20112222},
  {.DimmPerCh = 1, .DDRrate = DDR1600 + DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x30112222},
  {.DimmPerCh = 1, .DDRrate = DDR1600 + DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00000000, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x00112222},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x003B0000, .ODC = 0x00112222},
  {.DimmPerCh = 2, .DDRrate = DDR667, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x10222322},
  {.DimmPerCh = 2, .DDRrate = DDR800, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00390039, .ODC = 0x20222322},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x10112222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00380000, .ODC = 0x10112222},
  {.DimmPerCh = 2, .DDRrate = DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00350037, .ODC = 0x30222322},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x20112222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00360000, .ODC = 0x20112222},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00000035, .ODC = 0x30222322},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .SlowMode = 0, .AddTmgCtl = 0x00000000, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00000000, .ODC = 0x30112222},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .SlowMode = 1, .AddTmgCtl = 0x00000033, .ODC = 0x30222322}
};
CONST PSC_TBL_ENTRY SAOTblEntSOAM3 = {
   .Header = {.PSCType = PSCFG_SAO, .DimmType = SODIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_AM3, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrAM3SODdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   .TBLPtr = (VOID *)&OrAM3SODdr3SAO
};
// training configuratrions
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   2D
//
STATIC CONST PSCFG_S___ENTRY OrAM3SODdr3S__[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667, .VDDIO = VOLT_ALL, .Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Enable__ = 0},
  };
CONST PSC_TBL_ENTRY S__TblEntSOAM3 = {
   .Header = {.PSCType = PSCFG_S__, .DimmType = SODIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_AM3, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrAM3SODdr3S__) / sizeof (PSCFG_S___ENTRY),
   .TBLPtr = (VOID *)&OrAM3SODdr3S__
};
// ODT pattern for 1 DPC
// Format:
//  Dimm0,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_1D_ODTPAT_ENTRY Or1SODdr3OdtPat[] = {
  {.Dimm0 = DIMM_SR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00000001},
  {.Dimm0 = DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00000401}
};
CONST PSC_TBL_ENTRY OdtPat1DTblEntSOAM3 = {
   .Header = {.PSCType = PSCFG_ODT_PAT_1D, .DimmType = SODIMM_TYPE, .NumOfDimm = _1DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_AM3, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or1SODdr3OdtPat) / sizeof (PSCFG_1D_ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or1SODdr3OdtPat
};

// ODT pattern for 2 DPC
// Format:
//  Dimm0,   Dimm1,   ,RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG____ODTPAT_ENTRY Or2SODdr3OdtPat[] = {
  {.Dimm0 = NP, .Dimm1 = DIMM_SR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x00020000},
  {.Dimm0 = NP, .Dimm1 = DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x08020000},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x01010202, .WrODTCSHigh = 0x00000000, .WrODTCSLow = 0x09030603}
};
CONST PSC_TBL_ENTRY OdtPat2DTblEntSOAM3 = {
   .Header = {.PSCType = PSCFG_ODT_PAT___, .DimmType = SODIMM_TYPE, .NumOfDimm = _2DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_AM3, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or2SODdr3OdtPat) / sizeof (PSCFG____ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or2SODdr3OdtPat
};

// ODT pattern for 3 DPC
// Format:
//  Dimm0,   Dimm1,   Dimm2,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_3D_ODTPAT_ENTRY Or3SODdr3OdtPat[] = {
  {.Dimm0 = NP, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000000, .RdODTCSLow = 0x00000000, .WrODTCSHigh = 0x00000404, .WrODTCSLow = 0x00000000},
  {.Dimm0 = DIMM_SR + DIMM_DR, .Dimm1 = NP, .Dimm2 = DIMM_SR + DIMM_DR, .RdODTCSHigh = 0x00000101, .RdODTCSLow = 0x00000404, .WrODTCSHigh = 0x00000105, .WrODTCSLow = 0x00000405}
};
CONST PSC_TBL_ENTRY OdtPat3DTblEntSOAM3 = {
   .Header = {.PSCType = PSCFG_ODT_PAT_3D, .DimmType = SODIMM_TYPE, .NumOfDimm = _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_AM3, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (Or3SODdr3OdtPat) / sizeof (PSCFG_3D_ODTPAT_ENTRY),
   .TBLPtr = (VOID *)&Or3SODdr3OdtPat
};

// Dram Term and Dynamic Dram Term
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   Dimm,   Rank,   RttNom,    RttWr
//
STATIC CONST PSCFG_RTT_ENTRY DramTermOrAM3SODIMM[] = {
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600 + DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 1, .DDRrate = DDR1600 + DDR1866, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = NP, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 2, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR667 + DDR800 + DDR1066, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 1, .RttWr = 0},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1333, .VDDIO = V1_5, .Dimm0 = DIMM_DR, .Dimm1 = DIMM_SR + DIMM_DR, .Dimm2 = NP, .Dimm = DIMM_DR, .Rank = R0 + R1, .RttNom = 5, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = NP, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 3, .RttWr = 2},
  {.DimmPerCh = 2, .DDRrate = DDR1600, .VDDIO = V1_5, .Dimm0 = DIMM_SR, .Dimm1 = DIMM_SR, .Dimm2 = NP, .Dimm = DIMM_SR, .Rank = R0, .RttNom = 4, .RttWr = 1}
};
CONST PSC_TBL_ENTRY DramTermTblEntSOAM3 = {
   .Header = {.PSCType = PSCFG_RTT, .DimmType = SODIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_AM3, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (DramTermOrAM3SODIMM) / sizeof (PSCFG_RTT_ENTRY),
   .TBLPtr = (VOID *)&DramTermOrAM3SODIMM
};

// Max Freq.
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqOrAM3SODIMM[] = {
  {._MAXFREQ_ENTRY = {.DimmPerCh = 1, .Dimms = 1, .SR = 1, .DR = 0, .QR = 0, .Speed1_5V = DDR1866_FREQUENCY, .Speed1_35V = 0, .Speed1_25V = 0}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 1, .Dimms = 1, .SR = 0, .DR = 1, .QR = 0, .Speed1_5V = DDR1866_FREQUENCY, .Speed1_35V = 0, .Speed1_25V = 0}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 1, .SR = 1, .DR = 0, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = 0, .Speed1_25V = 0}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 1, .SR = 0, .DR = 1, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = 0, .Speed1_25V = 0}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 2, .DR = 0, .QR = 0, .Speed1_5V = DDR1600_FREQUENCY, .Speed1_35V = 0, .Speed1_25V = 0}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 1, .DR = 1, .QR = 0, .Speed1_5V = DDR1333_FREQUENCY, .Speed1_35V = 0, .Speed1_25V = 0}},
  {._MAXFREQ_ENTRY = {.DimmPerCh = 2, .Dimms = 2, .SR = 0, .DR = 2, .QR = 0, .Speed1_5V = DDR1333_FREQUENCY, .Speed1_35V = 0, .Speed1_25V = 0}}
};
CONST PSC_TBL_ENTRY MaxFreqTblEntSOAM3 = {
   .Header = {.PSCType = PSCFG_MAXFREQ, .DimmType = SODIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = OR_SOCKET_AM3, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (MaxFreqOrAM3SODIMM) / sizeof (PSCFG_MAXFREQ_ENTRY),
   .TBLPtr = (VOID *)&MaxFreqOrAM3SODIMM
};

