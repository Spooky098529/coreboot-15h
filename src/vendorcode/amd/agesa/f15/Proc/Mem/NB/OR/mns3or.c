/* $NoKeywords:$ */
/**
 * @file
 *
 * mns3or.c
 *
 * OR memory specific function to support S3 resume
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/OR)
 * @e \$Revision: 59560 $ @e \$Date: 2011-09-26 11:43:44 -0600 (Mon, 26 Sep 2011) $
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "S3.h"
#include "mfs3.h"
#include "mnor.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "F15PackageType.h"
#include "mnS3or.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_MEM_NB_OR_MNS3OR_FILECODE

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
BOOLEAN
MemS3ResumeConstructNBBlockOr (
  IN OUT   VOID *S3NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  );

UINT16
STATIC
MemNS3GetRegLstPtrOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  );

AGESA_STATUS
STATIC
MemNS3GetDeviceRegLstOr (
  IN       UINT32 RegisterLstID,
     OUT   VOID **RegisterHeader
  );

VOID
STATIC
MemNS3SetDfltPllLockTimeOr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3SetDynModeChangeOr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3SaveMR0Or (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
MemNS3RestoreMR0SetPPDOr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
STATIC
MemNS3GetCSROr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
STATIC
MemNS3SetCSROr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
PCI_SPECIAL_CASE PciSpecialCaseFuncOr[] = {
  {.Save = MemNS3GetCSROr, .Restore = MemNS3SetCSROr},
  {.Save = MemNS3GetBitFieldNb, .Restore = MemNS3SetBitFieldNb},
  {.Save = MemNS3GetNBPStateDepRegUnb, .Restore = MemNS3SetNBPStateDepRegUnb},
  { .Save = (VOID (*) (ACCESS_WIDTH, PCI_ADDR, VOID *, VOID *)) memDefRet, .Restore = MemNS3SetDfltPllLockTimeOr},
  { .Save = (VOID (*) (ACCESS_WIDTH, PCI_ADDR, VOID *, VOID *)) memDefRet, .Restore = MemNS3SetDisAutoCompUnb},
  { .Save = (VOID (*) (ACCESS_WIDTH, PCI_ADDR, VOID *, VOID *)) memDefRet, .Restore = MemNS3SetDynModeChangeOr},
  { .Save = (VOID (*) (ACCESS_WIDTH, PCI_ADDR, VOID *, VOID *)) memDefRet, .Restore = MemNS3DisableChannelNb},
  {.Save = MemNS3SaveNBRegiserUnb, .Restore = MemNS3RestoreNBRegiserUnb},
  {.Save = MemNS3GetBitFieldNb, .Restore = MemNS3SetPreDriverCalUnb},
  { .Save = (VOID (*) (ACCESS_WIDTH, PCI_ADDR, VOID *, VOID *)) memDefRet, .Restore = MemNS3SetMemClkFreqValUnb},
  {.Save = MemNS3SaveMR0Or, .Restore = MemNS3RestoreMR0SetPPDOr}
};

PCI_REG_DESCRIPTOR ROMDATA S3PciPreSelfRefDescriptorOr[] = {
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x110, .AndMask = 0xFFFFF8E7},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x40,  .AndMask = 0xFFFF0703},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x48,  .AndMask = 0xFFFF0703},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x50,  .AndMask = 0xFFFF0703},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x58,  .AndMask = 0xFFFF0703},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x60,  .AndMask = 0xFFFF0703},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x68,  .AndMask = 0xFFFF0703},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x70,  .AndMask = 0xFFFF0703},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x78,  .AndMask = 0xFFFF0703},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x140, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x148, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x150, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x158, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x160, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x168, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x170, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x178, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x44,  .AndMask = 0xFFFF0707},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x4C,  .AndMask = 0xFFFF0707},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x54,  .AndMask = 0xFFFF0707},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x5C,  .AndMask = 0xFFFF0707},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x64,  .AndMask = 0xFFFF0707},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x6C,  .AndMask = 0xFFFF0707},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x74,  .AndMask = 0xFFFF0707},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x7C,  .AndMask = 0xFFFF0707},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x144, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x14C, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x154, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x15C, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x164, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x16C, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x174, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 1, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x17C, .AndMask = 0x000000FF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0xF0,  .AndMask = 0xFF00FF87},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x120, .AndMask = 0x00FFFFFF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_1, .Offset = 0x124, .AndMask = 0x00FFFFFF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x10C, .AndMask = 0x07F3FBF9},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x114, .AndMask = 0xFFFFFC00},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x118, .AndMask = 0xF773FFFF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x11C, .AndMask = 0xAFFFFFFF},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x1B0, .AndMask = 0xFFD3FF3F},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 2, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x1B4, .AndMask = 0x000003FF}
};

CONST PCI_REGISTER_BLOCK_HEADER ROMDATA S3PciPreSelfRefOr = {
  .Version = 0,
  .NumRegisters = (sizeof (S3PciPreSelfRefDescriptorOr) / sizeof (PCI_REG_DESCRIPTOR)),
  .RegisterList = S3PciPreSelfRefDescriptorOr,
  .SpecialCases = NULL
};

CONDITIONAL_PCI_REG_DESCRIPTOR ROMDATA S3CPciPreSelfDescriptorOr[] = {
   // DCT 0
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x40,  .AndMask = 0x7FF83FEF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x44,  .AndMask = 0x7FF83FEF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x48,  .AndMask = 0x7FF83FEF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x4C,  .AndMask = 0x7FF83FEF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x50,  .AndMask = 0x7FF83FEF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x54,  .AndMask = 0x7FF83FEF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x58,  .AndMask = 0x7FF83FEF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x5C,  .AndMask = 0x7FF83FEF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x60,  .AndMask = 0x7FF83FE3, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x64,  .AndMask = 0x7FF83FE3, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x68,  .AndMask = 0x7FF83FE3, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x6C,  .AndMask = 0x7FF83FE3, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x78,  .AndMask = 0x00020000, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x80,  .AndMask = 0x0000FFFF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x84,  .AndMask = 0x00800003, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x88,  .AndMask = 0x3F000000, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x8C,  .AndMask = 0x00070000, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x90,  .AndMask = 0x0BFDF100, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0xA4,  .AndMask = 0x00F07900, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0xA8,  .AndMask = 0x3F60FFBC, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x200, .AndMask = 0x3F1F1F1F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x204, .AndMask = 0x0F3F0F3F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x208, .AndMask = 0x07070707, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x20C, .AndMask = 0x00000F1F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 2, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = SET_S3_NB_PSTATE_OFFSET (0x210, 0), .AndMask = 0xFFCF000F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 2, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = SET_S3_NB_PSTATE_OFFSET (0x210, 1), .AndMask = 0xFFCF000F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 2, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = SET_S3_NB_PSTATE_OFFSET (0x210, 2), .AndMask = 0xFFCF000F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 2, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = SET_S3_NB_PSTATE_OFFSET (0x210, 3), .AndMask = 0xFFCF000F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x214, .AndMask = 0x000F0F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x218, .AndMask = 0x0F0F0F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x21C, .AndMask = 0x001F1F00, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x220, .AndMask = 0x00001F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x224, .AndMask = 0x0000070F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x228, .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x22C, .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x230, .AndMask = 0x0F0F0F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x234, .AndMask = 0x0F0F0F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x238, .AndMask = 0x0F0F0F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x23C, .AndMask = 0x0F0F0F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x240, .AndMask = 0x000077FF, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x244, .AndMask = 0x0000000F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x248, .AndMask = 0x3F3F1F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x24C, .AndMask = 0x3F3F3F0F, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},

  // DCT 1
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x40,  .AndMask = 0x7FF83FEF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x44,  .AndMask = 0x7FF83FEF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x48,  .AndMask = 0x7FF83FEF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x4C,  .AndMask = 0x7FF83FEF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x50,  .AndMask = 0x7FF83FEF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x54,  .AndMask = 0x7FF83FEF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x58,  .AndMask = 0x7FF83FEF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x5C,  .AndMask = 0x7FF83FEF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x60,  .AndMask = 0x7FF83FE3, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x64,  .AndMask = 0x7FF83FE3, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x68,  .AndMask = 0x7FF83FE3, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x6C,  .AndMask = 0x7FF83FE3, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x78,  .AndMask = 0x00020000, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x80,  .AndMask = 0x0000FFFF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x84,  .AndMask = 0x00800003, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x88,  .AndMask = 0x3F000000, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x8C,  .AndMask = 0x00070000, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x90,  .AndMask = 0x0BFDF102, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0xA8,  .AndMask = 0x3F60FFBC, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x200, .AndMask = 0x3F1F1F1F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x204, .AndMask = 0x0F3F0F3F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x208, .AndMask = 0x07070707, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x20C, .AndMask = 0x00000F1F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 2, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = SET_S3_NB_PSTATE_OFFSET (0x210, 0), .AndMask = 0xFFC7000F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 2, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = SET_S3_NB_PSTATE_OFFSET (0x210, 1), .AndMask = 0xFFC7000F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 2, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = SET_S3_NB_PSTATE_OFFSET (0x210, 2), .AndMask = 0xFFC7000F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 2, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = SET_S3_NB_PSTATE_OFFSET (0x210, 3), .AndMask = 0xFFC7000F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x214, .AndMask = 0x000F0F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x218, .AndMask = 0x0F0F0F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x21C, .AndMask = 0x001F1F00, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x220, .AndMask = 0x00001F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x224, .AndMask = 0x0000070F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x228, .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x22C, .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x230, .AndMask = 0x0F0F0F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x234, .AndMask = 0x0F0F0F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x238, .AndMask = 0x0F0F0F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x23C, .AndMask = 0x0F0F0F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x240, .AndMask = 0x000077FF, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x244, .AndMask = 0x0000000F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x248, .AndMask = 0x3F3F1F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x24C, .AndMask = 0x3F3F3F0F, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},

  // DCT 0
  // Phy Initialization
  {.Type = {.SpecialCaseIndex = 5, .RegisterSize = 3, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0B),  .AndMask = 0, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFPllRegWaitTime, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  // 3. Phy voltage related
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFDataRxVioLvl, .AndMask = 0x00000018, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFClkRxVioLvl, .AndMask = 0x00000018, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmpVioLvl, .AndMask = 0x0000C000, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmdRxVioLvl, .AndMask = 0x00000018, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCsrComparator, .AndMask = 0x0000000C, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFAddrRxVioLvl, .AndMask = 0x00000018, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  // DCT 1
  // Phy Initialization
  {.Type = {.SpecialCaseIndex = 5, .RegisterSize = 3, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x0B), .AndMask = 0, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFPllRegWaitTime, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  // 3. Phy voltage related
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFDataRxVioLvl, .AndMask = 0x00000018, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFClkRxVioLvl, .AndMask = 0x00000018, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmpVioLvl, .AndMask = 0x0000C000, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmdRxVioLvl, .AndMask = 0x00000018, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFAddrRxVioLvl, .AndMask = 0x00000018, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},

  // 4. Frequency Change
  // Check if a channel needs to be disabled
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCKETri, .AndMask = 0, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 6, .RegisterSize = 3, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0, .AndMask = 0, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCKETri, .AndMask = 0, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 6, .RegisterSize = 3, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0, .AndMask = 0, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK},

  {.Type = {.SpecialCaseIndex = 3, .RegisterSize = 3, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFPllLockTime, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 3, .RegisterSize = 3, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFPllLockTime, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x94,  .AndMask = 0x0FFFEC1F, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 7, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x94,  .AndMask = 0x0FFFEC1F, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},

  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x04), .AndMask = 0x003F3F3F, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x04), .AndMask = 0x003F3F3F, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFProcOdtAdv, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFProcOdtAdv, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFSkewMemClk, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFSkewMemClk, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},

  {.Type = {.SpecialCaseIndex = 9, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = 0x94, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = DCT0_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 9, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = 0x94, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = DCT1_ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFPllLockTime, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFPllLockTime, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},

  // DCT 0
  // 5. Phy Fence
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0C), .AndMask = 0x7FFF3FFF, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFDataFence2, .AndMask = 0x00007FFF, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFClkFence2, .AndMask = 0x0000000F, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmdFence2, .AndMask = 0x0000000F, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFAddrFence2, .AndMask = 0x0000000F, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x00),  .AndMask = 0x70777777, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},

  // 6. Phy Compensation Init
  {.Type = {.SpecialCaseIndex = 4, .RegisterSize = 3, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFDisablePredriverCal, .AndMask = 0, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFDataByteTxPreDriverCal2Pad1, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFDataByteTxPreDriverCal2Pad2, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFDataByteTxPreDriverCal, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmdAddr0TxPreDriverCal2Pad1, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmdAddr0TxPreDriverCal2Pad2, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmdAddr1TxPreDriverCal2Pad1, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmdAddr1TxPreDriverCal2Pad2, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFAddrTxPreDriverCal2Pad1, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFAddrTxPreDriverCal2Pad2, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFAddrTxPreDriverCal2Pad3, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFAddrTxPreDriverCal2Pad4, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmdAddr0TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFCmdAddr1TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFAddrTxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFClock0TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFClock1TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFClock2TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  // DCT 1
  // 5. Phy Fence
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x0C), .AndMask = 0x7FFF0FFF, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFDataFence2, .AndMask = 0x00007FFF, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFClkFence2, .AndMask = 0x0000000F, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmdFence2, .AndMask = 0x0000000F, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFAddrFence2, .AndMask = 0x0000000F, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x00),  .AndMask = 0x70777777, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  // 6. Phy Compensation Init
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFDataByteTxPreDriverCal2Pad1, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFDataByteTxPreDriverCal2Pad2, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFDataByteTxPreDriverCal, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmdAddr0TxPreDriverCal2Pad1, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmdAddr0TxPreDriverCal2Pad2, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmdAddr1TxPreDriverCal2Pad1, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmdAddr1TxPreDriverCal2Pad2, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFAddrTxPreDriverCal2Pad1, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFAddrTxPreDriverCal2Pad2, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFAddrTxPreDriverCal2Pad3, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFAddrTxPreDriverCal2Pad4, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmdAddr0TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFCmdAddr1TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFAddrTxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFClock0TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFClock1TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 8, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFClock2TxPreDriverCalPad0, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},

  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFDisablePredriverCal, .AndMask = 0, .Mask1 = DCT0_MASK + DCT1_MASK, .Mask2 = ANY_DIMM_MASK}
};

CONST CPCI_REGISTER_BLOCK_HEADER ROMDATA S3CPciPreSelfRefOr = {
  .Version = 0,
  .NumRegisters = (sizeof (S3CPciPreSelfDescriptorOr) / sizeof (CONDITIONAL_PCI_REG_DESCRIPTOR)),
  .RegisterList = S3CPciPreSelfDescriptorOr,
  .SpecialCases = PciSpecialCaseFuncOr
};

CONDITIONAL_PCI_REG_DESCRIPTOR ROMDATA S3CPciPostSelfDescriptorOr[] = {
  // DCT0 MR0 value and CS Enable map
  {.Type = {.SpecialCaseIndex = 10, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT0, .Offset = 0,  .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  // DCT1 MR0 value and CS Enable map
  {.Type = {.SpecialCaseIndex = 10, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = DCT1, .Offset = 0,  .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  // DCT0
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x10),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x11),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x12),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x13),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x14),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x15),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x16),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x17),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x18),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x19),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x1A),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x1B),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x20),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x21),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x23),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x24),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x26),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x27),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x29),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x2A),  .AndMask = 0x03FF03FF, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x01),  .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x02),  .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x03),  .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x101), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x102), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x103), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x201), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x202), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x203), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x301), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x302), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x303), .AndMask = 0xFFFFFFFF, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x05),  .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x06),  .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x07),  .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x105), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x106), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x107), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x205), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x206), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x207), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x305), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x306), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x307), .AndMask = 0x3E3E3E3E, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x30),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x31),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x32),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x33),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x34),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x35),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x36),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x37),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x38),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x39),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x3A),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x3B),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x40),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x41),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x43),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x44),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x46),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x47),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x49),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x4A),  .AndMask = 0x00FF00FF, .Mask1 = DCT0_DDR3_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0D),  .AndMask = 0x037F037F, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFPhyClkConfig0, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFPhyClkConfig1, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFPhyClkConfig2, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFPhyClkConfig3, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFPhy0x0D0F0F13, .AndMask = 0x00000083, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFAddrCmdTri, .AndMask = 0x0000000A1, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved00C, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFEnRxPadStandby, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFEccDLLPwrDnConf, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFTriDM, .AndMask = 0, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFDisDllShutdownSR, .AndMask = 0x00000001, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_0,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_1,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_2,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_3,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_4,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_5,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_6,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_7,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_8,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_9,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_A,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_B,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_C,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_D,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_E,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_F,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_10, .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved4_11, .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x01},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_0,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_1,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_2,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_3,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_4,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_5,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_6,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_7,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_8,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_9,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_A,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_B,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_C,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_D,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_E,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_F,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_10, .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved5_11, .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x04},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_0,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_1,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_2,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_3,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_4,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_5,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_6,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_7,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_8,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_9,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_A,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_B,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_C,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_D,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_E,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_F,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_10, .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved6_11, .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_0,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_1,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_2,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_3,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_4,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_5,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_6,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_7,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_8,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_9,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_A,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_B,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_C,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_D,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_E,  .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_F,  .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_10, .AndMask = 0x0000001F, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved7_11, .AndMask = 0x00001F00, .Mask1 = DCT0_MASK, .Mask2 = 0x40},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved8_0, .AndMask = 0x00000003, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT0,   .Offset = BFReserved8_1, .AndMask = 0x0000007C, .Mask1 = DCT0_MASK, .Mask2 = ANY_DIMM_MASK},

    // DCT1
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x10),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x11),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x12),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x13),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x14),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x15),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x16),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x17),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x18),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x19),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x1A),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x1B),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x20),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x21),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x23),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x24),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x26),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x27),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x29),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x2A),  .AndMask = 0x03FF03FF, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x01),  .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x02),  .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x03),  .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x101), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x102), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x103), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x201), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x202), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x203), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x10},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x301), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x302), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x303), .AndMask = 0xFFFFFFFF, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x05),  .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x06),  .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x07),  .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x105), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x106), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x107), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x205), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x206), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x207), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x305), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x306), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x307), .AndMask = 0x3E3E3E3E, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x30),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x31),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x32),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x33),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x34),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x35),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x36),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x37),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x38),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x39),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x3A),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x3B),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x40),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x41),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x43),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x44),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x46),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x47),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x49),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x4A),  .AndMask = 0x00FF00FF, .Mask1 = DCT1_DDR3_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 1}, .Function = FUNC_2, .Offset = SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x0D),  .AndMask = 0x037F037F, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFPhyClkConfig0, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFPhyClkConfig1, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFPhyClkConfig2, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFPhyClkConfig3, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFPhy0x0D0F0F13, .AndMask = 0x00000083, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFAddrCmdTri, .AndMask = 0x0000000A1, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved00C, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFEnRxPadStandby, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFEccDLLPwrDnConf, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFTriDM, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFDisDllShutdownSR, .AndMask = 0, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_0,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_1,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_2,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_3,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_4,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_5,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_6,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_7,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_8,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_9,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_A,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_B,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_C,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_D,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_E,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_F,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_10, .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved4_11, .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x02},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_0,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_1,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_2,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_3,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_4,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_5,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_6,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_7,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_8,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_9,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_A,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_B,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_C,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_D,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_E,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_F,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_10, .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved5_11, .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x08},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_0,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_1,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_2,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_3,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_4,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_5,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_6,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_7,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_8,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_9,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_A,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_B,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_C,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_D,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_E,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_F,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_10, .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved6_11, .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x20},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_0,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_1,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_2,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_3,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_4,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_5,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_6,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_7,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_8,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_9,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_A,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_B,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_C,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_D,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_E,  .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_F,  .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 2, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_10, .AndMask = 0x0000001F, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved7_11, .AndMask = 0x00001F00, .Mask1 = DCT1_MASK, .Mask2 = 0x80},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved8_0, .AndMask = 0x00000003, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 1, .RegisterSize = 1, .SpecialCaseFlag = 1}, .Function = DCT1,   .Offset = BFReserved8_1, .AndMask = 0x0000007C, .Mask1 = DCT1_MASK, .Mask2 = ANY_DIMM_MASK},

  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x1B4, .AndMask = 0x08000000, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_3, .Offset = 0x180, .AndMask = 0x02000000, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_3, .Offset = 0x58,  .AndMask = 0x1F00001F, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_3, .Offset = 0x5C,  .AndMask = 0x00000001, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_3, .Offset = 0x80,  .AndMask = 0xE7E7E7E7, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_3, .Offset = 0x84,  .AndMask = 0xE7E7E7E7, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_3, .Offset = 0x1B8, .AndMask = 0x00000010, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_3, .Offset = 0x44,  .AndMask = 0x00400004, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_4, .Offset = 0x128, .AndMask = 0x0003F000, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  // Release NB P-state force
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 2, .SpecialCaseFlag = 0}, .Function = FUNC_5, .Offset = 0x170, .AndMask = 0x00006EDB, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x118, .AndMask = 0x00040000, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Function = FUNC_2, .Offset = 0x118, .AndMask = 0x00080000, .Mask1 = ANY_DIMM_MASK, .Mask2 = ANY_DIMM_MASK},
};

CONST CPCI_REGISTER_BLOCK_HEADER ROMDATA S3CPciPostSelfRefOr = {
  .Version = 0,
  .NumRegisters = (sizeof (S3CPciPostSelfDescriptorOr) / sizeof (CONDITIONAL_PCI_REG_DESCRIPTOR)),
  .RegisterList = S3CPciPostSelfDescriptorOr,
  .SpecialCases = PciSpecialCaseFuncOr
};

MSR_REG_DESCRIPTOR ROMDATA S3MSRPreSelfRefDescriptorOr[] = {
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Address = 0xC0010010, .AndMask = 0x00000000007F0000},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Address = 0xC001001A, .AndMask = 0x0000FFFFFF800000},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Address = 0xC001001D, .AndMask = 0x0000FFFFFF800000},
  {.Type = {.SpecialCaseIndex = 0, .RegisterSize = 0, .SpecialCaseFlag = 0}, .Address = 0xC001001F, .AndMask = 0x0044601080000600}
};

CONST MSR_REGISTER_BLOCK_HEADER ROMDATA S3MSRPreSelfRefOr = {
  .Version = 0,
  .NumRegisters = (sizeof (S3MSRPreSelfRefDescriptorOr) / sizeof (MSR_REG_DESCRIPTOR)),
  .RegisterList = S3MSRPreSelfRefDescriptorOr,
  .SpecialCases = NULL
};

VOID *MemS3RegListOr[] = {
  (VOID *)&S3PciPreSelfRefOr,
  NULL,
  (VOID *)&S3CPciPreSelfRefOr,
  (VOID *)&S3CPciPostSelfRefOr,
  (VOID *)&S3MSRPreSelfRefOr,
  NULL,
  NULL,
  NULL
};

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the northbridge block for S3 resume
 *
 *     @param[in,out]   *S3NBPtr   - Pointer to MEM_NB_BLOCK.
 *     @param[in,out]   *MemPtr  - Pointer to MEM_DATA_STRUCT.
 *     @param[in]       NodeID   - Node ID of the target node.
 *
 *      @return         BOOLEAN
 *                         TRUE - This is the correct constructor for the targeted node.
 *                         FALSE - This isn't the correct constructor for the targeted node.
 */
BOOLEAN
MemS3ResumeConstructNBBlockOr (
  IN OUT   VOID *S3NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  UINT32  PackageType;
  INT32 i;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = ((S3_MEM_NB_BLOCK *)S3NBPtr)->NBPtr;

  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[NodeID].SocketId, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedOr (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->MemPtr = MemPtr;
  NBPtr->MCTPtr = &(MemPtr->DiesPerSystem[NodeID]);
  NBPtr->PciAddr.AddressValue = MemPtr->DiesPerSystem[NodeID].PciAddr.AddressValue;
  MemNInitNBRegTableOr (NBPtr, NBPtr->NBRegTable);
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->Ganged = FALSE;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_OR;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_OR;

  for (i = 0; i < EnumSize; i++) {
    NBPtr->IsSupported[i] = FALSE;
  }

  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = (BOOLEAN (*) (MEM_NB_BLOCK *, VOID *)) memDefTrue;
  }

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &MemPtr->StdHeader);

  NBPtr->SwitchDCT = MemNSwitchDCTNb;
  NBPtr->SwitchChannel = MemNSwitchChannelNb;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldOr;
  NBPtr->GetBitField = MemNGetBitFieldNb;
  NBPtr->SetBitField = MemNSetBitFieldNb;
  NBPtr->MemNIsIdSupportedNb = MemNIsIdSupportedOr;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3ExitSelfRefReg = (VOID (*) (MEM_NB_BLOCK *, AMD_CONFIG_PARAMS *)) memDefRet;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetConPCIMask = MemNS3GetConPCIMaskUnb;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetConMSRMask = (VOID (*) (MEM_NB_BLOCK *, DESCRIPTOR_GROUP *)) memDefRet;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3Resume = MemNS3ResumeUNb;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3RestoreScrub = MemNS3RestoreScrubNb;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetRegLstPtr = MemNS3GetRegLstPtrOr;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetDeviceRegLst = MemNS3GetDeviceRegLstOr;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3SpecialCaseHeapSize = 0;
  NBPtr->FamilySpecificHook[DCTSelectSwitch] = MemNS3DctCfgSelectUnb;

  // AM3r2 does not support Dll shutdown
  PackageType = LibAmdGetPackageType (&(NBPtr->MemPtr->StdHeader));
  if (PackageType != PACKAGE_TYPE_AM3r2) {
    NBPtr->IsSupported[SetDllShutDown] = TRUE;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function returns the register list for each device for OR
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in, out]  *DescriptPtr - Pointer to DESCRIPTOR_GROUP
 *     @return          UINT16 - size of the device descriptor on the target node.
 */
UINT16
STATIC
MemNS3GetRegLstPtrOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  )
{
  UINT8 i;
  UINT16 Size;
  Size = 0;
  for (i = PRESELFREF; i <= POSTSELFREF; i ++) {
    DescriptPtr->PCIDevice[i].Type = (UINT8) (DEV_TYPE_PCI_PRE_ESR + i);
    DescriptPtr->PCIDevice[i].Node = NBPtr->Node;
    DescriptPtr->PCIDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((PCI_REGISTER_BLOCK_HEADER *) MemS3RegListOr[PCI_LST_ESR_OR - PCI_LST_ESR_OR + i] != NULL) {
      DescriptPtr->PCIDevice[i].RegisterListID = PCI_LST_ESR_OR + i;
      Size += sizeof (PCI_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->CPCIDevice[i].Type = (UINT8) (DEV_TYPE_CPCI_PRE_ESR + i);
    DescriptPtr->CPCIDevice[i].Node = NBPtr->Node;
    DescriptPtr->CPCIDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((CPCI_REGISTER_BLOCK_HEADER *) MemS3RegListOr[CPCI_LST_ESR_OR - PCI_LST_ESR_OR + i] != NULL) {
      DescriptPtr->CPCIDevice[i].RegisterListID = CPCI_LST_ESR_OR + i;
      Size += sizeof (CONDITIONAL_PCI_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->MSRDevice[i].Type = (UINT8) (DEV_TYPE_MSR_PRE_ESR + i);
    DescriptPtr->MSRDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((MSR_REGISTER_BLOCK_HEADER *) MemS3RegListOr[MSR_LST_ESR_OR - PCI_LST_ESR_OR + i] != NULL) {
      DescriptPtr->MSRDevice[i].RegisterListID = MSR_LST_ESR_OR + i;
      Size += sizeof (MSR_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->CMSRDevice[i].Type = (UINT8) (DEV_TYPE_CMSR_PRE_ESR + i);
    DescriptPtr->CMSRDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((CMSR_REGISTER_BLOCK_HEADER *) MemS3RegListOr[CMSR_LST_ESR_OR - PCI_LST_ESR_OR + i] != NULL) {
      DescriptPtr->CMSRDevice[i].RegisterListID = CMSR_LST_ESR_OR + i;
      Size += sizeof (CONDITIONAL_MSR_DEVICE_DESCRIPTOR);
    }
  }
  return Size;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function return the register list according to the register ID.
 *
 *     @param[in]   RegisterLstID - value of the Register list ID.
 *     @param[out]  **RegisterHeader - pointer to the address of the register list.
 *     @return      none
 */
AGESA_STATUS
STATIC
MemNS3GetDeviceRegLstOr (
  IN       UINT32 RegisterLstID,
     OUT   VOID **RegisterHeader
  )
{
  if (RegisterLstID >= (sizeof (MemS3RegListOr) / sizeof (VOID *))) {
    ASSERT(FALSE); // RegisterListID exceeded size of Register list
    return AGESA_FATAL;
  }
  if (MemS3RegListOr[RegisterLstID] != NULL) {
    *RegisterHeader = MemS3RegListOr[RegisterLstID];
    return AGESA_SUCCESS;
  }
  ASSERT(FALSE); // Device register list error
  return AGESA_FATAL;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function that set PllLockTime to default state.
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
STATIC
MemNS3SetDfltPllLockTimeOr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT16 RegValue;

  RegValue = 0x190;
  MemNS3SetBitFieldNb (AccessS3SaveWidth16, Address, &RegValue, ConfigPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets bit 31 [DynModeChange] of F2x9C_xB
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SetDynModeChangeOr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT8 Dct;
  UINT32 RegValue;
  UINT32 Temp;
  UINT32 TempValue;

  IDS_SKIP_HOOK (IDS_BEFORE_S3_SPECIAL, &Address, ConfigPtr) {
    Temp = Address.Address.Register;
    Dct = ((Temp & 0x400) == 0) ? 0 : 1;

    // Switch Dct
    Address.Address.Function = FUNC_1;
    Address.Address.Register = 0x10C;
    TempValue = Dct;
    LibAmdPciWrite (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);

    Address.Address.Function = FUNC_2;
    Address.Address.Register = Temp;
    RegValue = 0x80000000;
    MemNS3SetCSRNb (AccessS3SaveWidth32, Address, &RegValue, ConfigPtr);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function saves the heap data of MR0 and chipsel enable map before S3
 *   entry
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SaveMR0Or (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  AGESA_STATUS Status;
  LOCATE_HEAP_PTR LocateHeapStructPtr;
  MR0_DATA_ARRAY_PTR pMR0Data;
  UINT32 Node;
  UINT32 Dct;

  Node = Address.Address.Device - 0x18;
  Dct = Address.Address.Function;

  // Get original MR0 from heap
  LocateHeapStructPtr.BufferHandle = AMD_MEM_S3_MR0_DATA_HANDLE;
  LocateHeapStructPtr.BufferPtr = NULL;
  Status = HeapLocateBuffer (&LocateHeapStructPtr, ConfigPtr);
  ASSERT (Status == AGESA_SUCCESS);
  pMR0Data = (MR0_DATA_ARRAY_PTR) (LocateHeapStructPtr.BufferPtr);
  ASSERT (pMR0Data != NULL);

  IDS_HDT_CONSOLE (MEM_FLOW, "\tSave MR0 for S3 resume\n\t\tNode: %d, Dct: %d, ChipSelEnMap: %04X, MR0: %04X\n", Node, Dct, (*pMR0Data)[Node][Dct].ChipSelEnMap, (*pMR0Data)[Node][Dct].MR0Value);
  *(UINT32*) Value = (*pMR0Data)[Node][Dct].MR0Value;
  *(UINT32*) Value <<= 16;
  *(UINT32*) Value |= (*pMR0Data)[Node][Dct].ChipSelEnMap;
}

/**
 *
 *
 *   This function send an MRS command to set MR0[PPD] after exit self-refresh
 *
 *     @param[in]   AccessWidth - Access width of the register.
 *     @param[in]   Address - address in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3RestoreMR0SetPPDOr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT32 Node;
  UINT32 Dct;
  UINT16 ChipSelEnMap;
  UINT16 MR0Value;
  UINT32 TempValue;
  UINT32 PowerDownMode;
  UINT32 PchgPDModeSel;
  UINT8 ChipSel;

  Node = Address.Address.Device - 0x18;
  Dct = Address.Address.Function;
  ChipSelEnMap = (UINT16) ((*(UINT32*) Value) & 0xFFFF);
  MR0Value = (UINT16) (((*(UINT32*) Value) >> 16) & 0xFFFF);

  Address.Address.Register = BFPowerDownMode;
  MemNS3GetBitFieldNb (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);
  PowerDownMode = TempValue;

  Address.Address.Register = BFPchgPDModeSel;
  MemNS3GetBitFieldNb (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);
  PchgPDModeSel = TempValue;

  // Check if Fast exit pre-charge powerdown mode is desired
  // D18F2x84[PowerDownMode] = 0
  // D18F2x94[PchgPDModeSel] = 0
  // MR0[PPD] = 1
  if (PowerDownMode == 0 && PchgPDModeSel == 0 && (MR0Value & 0x1000) == 0x1000) {
    for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
      if (((ChipSelEnMap >> ChipSel) & 1) != 0) {
        // if chip select present
        Address.Address.Register = BFMrsChipSel;
        MemNS3SetBitFieldNb (AccessS3SaveWidth32, Address, &ChipSel, ConfigPtr);
        // BA2=0,BA1=0,BA0=0
        TempValue = 0;
        Address.Address.Register = BFMrsBank;
        MemNS3SetBitFieldNb (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);
        Address.Address.Register = BFMrsAddress;
        MemNS3SetBitFieldNb (AccessS3SaveWidth16, Address, &MR0Value, ConfigPtr);
        // Bit swapping is not needed here because hardware bit swapping does not occur for commands
        // sent via D18F2x7C_dct[1:0][SendMrsCmd] when D18F2x7C_dct[1:0][EndDramInit] = 0
        TempValue = 1;
        Address.Address.Register = BFSendMrsCmd;
        MemNS3SetBitFieldNb (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);
        IDS_HDT_CONSOLE (MEM_FLOW, "\tIssue MRS Command after ESR\n\t\tNode: %d, Dct: %d, CS: %d, MR0: %08X\n", Node, Dct, ChipSel, MR0Value);
        Address.Address.Register = BFSendMrsCmd;
        MemNS3GetBitFieldNb (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);
        while (TempValue != 0) {
          MemNS3GetBitFieldNb (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);
        }
        WaitMicroseconds (500, ConfigPtr);
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function read the value of CSR register.
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
STATIC
MemNS3GetCSROr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT32 ExtendOffset;
  UINT32 ValueRead;
  UINT32 TempFunc;
  UINT32 TempValue;

  ValueRead = 0;
  ExtendOffset = Address.Address.Register;
  TempFunc = Address.Address.Function;

  // Switch Dct
  Address.Address.Function = FUNC_1;
  Address.Address.Register = 0x10C;
  TempValue = 0;
  if (ExtendOffset & 0x400) {
    TempValue = 1;
  }
  LibAmdPciWrite (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);
  Address.Address.Function = TempFunc;

  Address.Address.Register = 0x98;
  ExtendOffset &= 0x3FF;
  LibAmdPciWrite (AccessS3SaveWidth32, Address, &ExtendOffset, ConfigPtr);
  while (((ValueRead >> 31) & 1) == 0) {
    LibAmdPciRead (AccessS3SaveWidth32, Address, &ValueRead, ConfigPtr);
  }
  Address.Address.Register = 0x9C;
  LibAmdPciRead (AccessWidth, Address, Value, ConfigPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function write to a CSR register
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
STATIC
MemNS3SetCSROr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT32 ExtendOffset;
  UINT32 ValueRead;
  UINT32 ValueWrite;
  UINT32 TempFunc;
  UINT32 TempValue;

  ValueRead = 0;
  ExtendOffset = Address.Address.Register;
  TempFunc = Address.Address.Function;
  // Switch Dct
  Address.Address.Function = FUNC_1;
  Address.Address.Register = 0x10C;
  TempValue = 0;
  if (ExtendOffset & 0x400) {
    TempValue = 1;
  }
  LibAmdPciWrite (AccessS3SaveWidth32, Address, &TempValue, ConfigPtr);

  Address.Address.Function = TempFunc;
  Address.Address.Register = 0x9C;

  ExtendOffset &= 0x3FF;
  ExtendOffset |= 0x40000000;
  switch (AccessWidth) {
  case AccessS3SaveWidth8:
    ValueWrite = *(UINT8 *) Value;
    break;
  case AccessS3SaveWidth16:
    ValueWrite = *(UINT16 *) Value;
    break;
  case AccessS3SaveWidth32:
    ValueWrite = *(UINT32 *) Value;
    break;
  default:
    ASSERT (FALSE);
  }
  LibAmdPciWrite (AccessS3SaveWidth32, Address, &ValueWrite, ConfigPtr);
  Address.Address.Register = 0x98;
  LibAmdPciWrite (AccessS3SaveWidth32, Address, &ExtendOffset, ConfigPtr);
  while (((ValueRead >> 31) & 1) == 0) {
    LibAmdPciRead (AccessS3SaveWidth32, Address, &ValueRead, ConfigPtr);
  }
}
