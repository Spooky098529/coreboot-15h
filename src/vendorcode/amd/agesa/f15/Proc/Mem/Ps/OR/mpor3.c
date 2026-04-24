/* $NoKeywords:$ */
/**
 * @file
 *
 * mpor3.c
 *
 * Platform specific settings for OR
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
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
#include "Filecode.h"

CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)



#define FILECODE PROC_MEM_PS_OR_MPOR3_FILECODE
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
//
// Common tables of Orochi platform specific configuration
//

// MR0[WR]
// Format :
// D18F2x22C_dct[1:0][Twr], MR0[WR]
//
CONST PSCFG_MR0WR_ENTRY MR0WR[] = {
  {.Timing = 0x10, .Value = 0},
  {.Timing = 0x05, .Value = 1},
  {.Timing = 0x06, .Value = 2},
  {.Timing = 0x07, .Value = 3},
  {.Timing = 0x08, .Value = 4},
  {.Timing = 0x0A, .Value = 5},
  {.Timing = 0x0C, .Value = 6},
  {.Timing = 0x0E, .Value = 7}
};
CONST PSC_TBL_ENTRY MR0WrTblEntry = {
   .Header = {.PSCType = PSCFG_MR0WR, .DimmType = DT_DONT_CARE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (MR0WR) / sizeof (PSCFG_MR0WR_ENTRY),
   .TBLPtr = (VOID *)&MR0WR
};

// MR0[CL]
// Format :
// D18F2x200_dct[1:0][Tcl], MR0[CL][3:1], MR0[CL][0]
//
CONST PSCFG_MR0CL_ENTRY MR0CL[] = {
  {.Timing = 0x05, .Value = 1, .Value1 = 0},
  {.Timing = 0x06, .Value = 2, .Value1 = 0},
  {.Timing = 0x07, .Value = 3, .Value1 = 0},
  {.Timing = 0x08, .Value = 4, .Value1 = 0},
  {.Timing = 0x09, .Value = 5, .Value1 = 0},
  {.Timing = 0x0A, .Value = 6, .Value1 = 0},
  {.Timing = 0x0B, .Value = 7, .Value1 = 0},
  {.Timing = 0x0C, .Value = 0, .Value1 = 1},
  {.Timing = 0x0D, .Value = 1, .Value1 = 1},
  {.Timing = 0x0E, .Value = 2, .Value1 = 1},
  {.Timing = 0x0F, .Value = 3, .Value1 = 1},
  {.Timing = 0x10, .Value = 4, .Value1 = 1}
};
CONST PSC_TBL_ENTRY MR0CLTblEntry = {
   .Header = {.PSCType = PSCFG_MR0CL, .DimmType = DT_DONT_CARE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (MR0CL) / sizeof (PSCFG_MR0CL_ENTRY),
   .TBLPtr = (VOID *)&MR0CL
};


//
// CKE tri-state
//
STATIC CONST UINT8 ROMDATA OrDdr3CKETri[] = {0x55, 0xAA};
CONST PSC_TBL_ENTRY OrDdr3CKETriEnt = {
   .Header = {.PSCType = PSCFG_CKETRI, .DimmType = DT_DONT_CARE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrDdr3CKETri) / sizeof (UINT8),
   .TBLPtr = (VOID *)&OrDdr3CKETri
};


//
// ODT tri-state [UDIMM & RDIMM][1DPC & 2DPC]
//
// Bit 0: MEMODT[1,0][0]
// Bit 1: MEMODT[1,0][1]
// Bit 2: MEMODT[1,0][2]
// Bit 3: MEMODT[1,0][3]
//
// Dimm 0 : BP_MEMCSx[1:0], BP_MEMODTx[2, 0]
// Dimm 1 : BP_MEMCSx[3:2], BP_MEMODTx[3, 1]
STATIC CONST UINT8 ROMDATA OrDdr3ODTTri[] = {0x01, 0x04, 0x02, 0x08};
CONST PSC_TBL_ENTRY OrDdr3ODTTriEnt = {
   .Header = {.PSCType = PSCFG_ODTTRI, .DimmType = UDIMM_TYPE + RDIMM_TYPE, .NumOfDimm = _1DIMM + _2DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrDdr3ODTTri) / sizeof (UINT8),
   .TBLPtr = (VOID *)&OrDdr3ODTTri
};
//
// ODT tri-state [UDIMM & RDIMM][3DPC]
//
// Dimm 0: BP_MEMCSx[1:0], BP_MEMODTx[0]
// Dimm 1: BP_MEMCSx[7:6, 3:2], BP_MEMODTx[3, 1]
// Dimm 2: BP_MEMCSx[5:4], BP_MEMODTx[2]
STATIC CONST UINT8 ROMDATA OrDdr3ODTTri3D[] = {0x03, 0x44, 0x30, 0x88};
CONST PSC_TBL_ENTRY OrDdr3ODTTri3DEnt = {
   .Header = {.PSCType = PSCFG_ODTTRI, .DimmType = UDIMM_TYPE + RDIMM_TYPE, .NumOfDimm = _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrDdr3ODTTri3D) / sizeof (UINT8),
   .TBLPtr = (VOID *)&OrDdr3ODTTri3D
};


//
// ODT tri-state [LRDIMM][1DPC & 2DPC]
//
// Bit 0: MEMODT[1,0][0]
// Bit 1: MEMODT[1,0][1]
// Bit 2: MEMODT[1,0][2]
// Bit 3: MEMODT[1,0][3]
//
// Dimm 0 : BP_MEMCSx[5:4, 1:0], BP_MEMODTx[2, 0]
// Dimm 1 : BP_MEMCSx[7:6, 3:2], BP_MEMODTx[3, 1]
// LR : Assert DIMM ODT0 only
STATIC CONST UINT8 ROMDATA OrLRDdr3ODTTri[] = {0x03, 0x0C, 0x00, 0x00};
CONST PSC_TBL_ENTRY OrLRDdr3ODTTriEnt = {
   .Header = {.PSCType = PSCFG_ODTTRI, .DimmType = LRDIMM_TYPE, .NumOfDimm = _1DIMM + _2DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrLRDdr3ODTTri) / sizeof (UINT8),
   .TBLPtr = (VOID *)&OrLRDdr3ODTTri
};

//
// ODT tri-state [LRDIMM][3DPC]
//
// Dimm 0: BP_MEMCSx[1:0], BP_MEMODTx[0]
// Dimm 1: BP_MEMCSx[7:6, 3:2], BP_MEMODTx[3, 1]
// Dimm 2: BP_MEMCSx[5:4], BP_MEMODTx[2]
// LR : Assert DIMM ODT0 only
STATIC CONST UINT8 ROMDATA OrLRDdr3ODTTri3D[] = {0x03, 0x0C, 0x30, 0xC0};
CONST PSC_TBL_ENTRY OrLRDdr3ODTTri3DEnt = {
   .Header = {.PSCType = PSCFG_ODTTRI, .DimmType = LRDIMM_TYPE, .NumOfDimm = _3DIMM, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrLRDdr3ODTTri3D) / sizeof (UINT8),
   .TBLPtr = (VOID *)&OrLRDdr3ODTTri3D
};

//
// ChipSel tri-state [UDIMM]
//
STATIC CONST UINT8 ROMDATA OrUDdr3CSTri[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
CONST PSC_TBL_ENTRY OrUDdr3CSTriEnt = {
   .Header = {.PSCType = PSCFG_CSTRI, .DimmType = UDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrUDdr3CSTri) / sizeof (UINT8),
   .TBLPtr = (VOID *)&OrUDdr3CSTri
};
//
// ChipSel tri-state [RDIMM & LRDIMM]
// BIOS must not tri-state chip select pin corresponding to the second chip
// select of a single rank registered dimm
STATIC CONST UINT8 ROMDATA OrDdr3CSTri[] = {0x01, 0x03, 0x04, 0x0C, 0x10, 0x30, 0x40, 0xC0};
CONST PSC_TBL_ENTRY OrDdr3CSTriEnt = {
   .Header = {.PSCType = PSCFG_CSTRI, .DimmType = LRDIMM_TYPE + RDIMM_TYPE, .NumOfDimm = NOD_DONT_CARE, .LogicalCpuid = {.Family = AMD_FAMILY_15_OR, .Revision = AMD_F15_ALL}, .PackageType = PT_DONT_CARE, .TechType = DDR3_TECHNOLOGY},
   .TableSize = sizeof (OrDdr3CSTri) / sizeof (UINT8),
   .TBLPtr = (VOID *)&OrDdr3CSTri
};

