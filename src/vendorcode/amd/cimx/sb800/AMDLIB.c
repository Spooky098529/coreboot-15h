/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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

#include "SBPLATFORM.h"
#include "cbtypes.h"

UINT8
getNumberOfCpuCores (
  OUT VOID
  )
{
  UINT8 Result;
  Result = 1;
  Result = ReadNumberOfCpuCores ();
  return Result;
}

UINT32
readAlink (
  IN       UINT32 Index
  )
{
  UINT32 Data;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
  ReadIO (ALINK_ACCESS_DATA, AccWidthUint32, &Data);
  //Clear Index
  Index = 0;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
  return Data;
}

VOID
writeAlink (
  IN       UINT32 Index,
  IN       UINT32 Data
  )
{
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &Index);
  WriteIO (ALINK_ACCESS_DATA, AccWidthUint32 | S3_SAVE, &Data);
  //Clear Index
  Index = 0;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &Index);
}

VOID
rwAlink (
  IN       UINT32 Index,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask
  )
{
  UINT32 AccesType;
  AccesType = Index & 0xE0000000;
  if (AccesType == (AXINDC << 29)) {
    writeAlink ((SB_AX_INDXC_REG30 | AccesType), Index & 0x1FFFFFFF);
    Index = (SB_AX_DATAC_REG34 | AccesType);
  } else if (AccesType == (AXINDP << 29)) {
    writeAlink ((SB_AX_INDXP_REG38 | AccesType), Index & 0x1FFFFFFF);
    Index = (SB_AX_DATAP_REG3C | AccesType);
  }
  writeAlink (Index, (readAlink (Index) & AndMask) | OrMask );
}

/**
 *
 * HasDiscreteNB - A helper to etermine if a discrete NB (i.e. 990FX/RD890)
 * is used in the system, so we can tweak operation of the SB for PCIe and NB<->SB
 * link and prevent configuration which is only beneficial on APU systems
 * where the NB is integrated into the APU, and the SB runs the show.
 *
 * A list of NBs this detects:
 *
 *      Family           DeviceID
 *     -------------------------
 *      RD890TV           0x5956
 *      RX780             0x5957
 *      RD780             0x5958
 *      SR5690            0x5A10
 *      RD890             0x5A11
 *      SR5670            0x5A12
 *      SR5650            0x5A13
 *      990FX/990X/970    0x5A14
 *
 * This helper is only useful for runtime detection rather than gating functionality
 * based on whether or not the board builds support for the RD890 (for example).
 * This instead detects the presence at runtime and validates the NB matches
 * known NB chips.
 */
UINT8
HasDiscreteNB(void)
{
	UINT16 DevID;

	ReadPCI((AMD_NB_BDF << 16) + 2, AccWidthUint16, &DevID);
	return ((DevID == 0x5956) || (DevID == 0x5957) || (DevID == 0x5958) || (DevID == 0x5A10)
		|| (DevID == 0x5A11) || (DevID == 0x5A12) || (DevID == 0x5A13)
		|| (DevID == 0x5A14))
		       ? 1
		       : 0;
}
