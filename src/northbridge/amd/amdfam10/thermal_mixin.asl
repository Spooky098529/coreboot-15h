/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Include this file into a mainboard DSDT inside the PCI device
 * "Northbridge Miscellaneous Control (Northbridge function 3)" and it
 * will expose the temperature sensor of the processor as a thermal
 * zone.
 *
 * Families 10 through 14 and some family 15 CPUs are supported.
 *
 * If, for example, the NB Misc. Control device is on 0:18.3, include
 * the following:
 *
 * Scope (\_SB.PCI0) {
 *   Device (K10M) {
 *     Name (_ADR, 0x00180003)
 *     #include <northbridge/amd/amdfam10/thermal_mixin.asl>
 *   }
 * }
 *
 * Do not include this if the board is affected by erratum 319 as the
 * thermal sensor of Socket F/AM2+ processors may be unreliable.
 * (Erratum 319 affects AM2+ boards, AM3 and later should be fine)
 *
 * Per-board configuration (define before including this file):
 *
 *   K10TEMP_CRT_TENTHS_K
 *     Critical (emergency shutdown) temperature in tenths of Kelvin.
 *     Example for 105°C:  #define K10TEMP_CRT_TENTHS_K  3782
 */

#define K10TEMP_KELVIN_OFFSET	2732
#define K10TEMP_TLIMIT_OFFSET	520

OperationRegion (TCFG, PCI_Config, 0x64, 0x4)
Field (TCFG, ByteAcc, NoLock, Preserve) {
	HTCE, 1, /* Hardware thermal control enable */
	, 15,
	TLMT, 7, /* (LimitTmp - 52) / 0.5 */
	, 9,
}

OperationRegion (TCTL, PCI_Config, 0xa4, 0x4)
Field (TCTL, ByteAcc, NoLock, Preserve) {
	, 21,
	TNOW, 11, /* CurTmp / 0.125 */
}

ThermalZone (TZ00) {
	Name (_HID, EisaId ("PNP0C11"))
	Name (_STR, Unicode ("AMD CPU Core Thermal Sensor"))

	Method (_STA) {
		If (LEqual (HTCE, One)) {
			Return (0x0F)
		}
		Return (Zero)
	}

	Method (_TMP) {	/* Current temp in tenths degree Kelvin. */
		Multiply (TNOW, 10, Local0)
		ShiftRight (Local0, 3, Local0)
		Return (Add (Local0, K10TEMP_KELVIN_OFFSET))
	}

	Method (_CRT) {	/* Critical temp in tenths degree Kelvin. */
		/* Critical triggers an OS-level emergency shutdown.
		   Overrides at the board level can be set, but it's
		   set to 105°C as a default value for now.
		   TODO: Calculate this value with a ceiling of ~115C
			 - which is the max reading value */
#ifdef K10TEMP_CRT_TENTHS_K
		/* Allow board override of thermal zone critical temp */
		Return (K10TEMP_CRT_TENTHS_K)
#else
		Return (3782)	/* 105°C fallback: (0x6A * 5) + TLIMIT_OFFSET + KELVIN_OFFSET */
#endif
	}

	Method (_HOT) {	/* Hot temp in tenths degree Kelvin, hardware-specific + offset */

                /*
		 * Calculate _HOT from hardware register F3x64[HtcTmpLmt].
		 * If TLMT is zero the register wasn't programmed for this
		 * board/CPU, so fall back to 85°C (CRT - 20).
		 * This avoids false-positive critical-temperature OS shutdowns
		 * but also allows the tCaseMax/HtcTmpLmt to influence the value
		 * if set/read from the CPU registers. The actual value programmed
		 * into this register can be influenced by vendorcode or
		 * come directly from the boot CPU's register, as this value is
		 * ideally CPU model dependant and roughly aligns with tCaseMax.
                 * Should report HOT when the hardware is set to start throttling,
                 * so the OS can also take corrective power-saving action.
		 */
		If (LNotEqual (TLMT, Zero)) {
			Multiply (TLMT, 10, Local0)
			ShiftRight (Local0, 1, Local0)
			Add (Local0, K10TEMP_TLIMIT_OFFSET, Local0)
			Return (Add (Local0, K10TEMP_KELVIN_OFFSET))
		}

		Return (Subtract (_CRT, 200))
	}
}
