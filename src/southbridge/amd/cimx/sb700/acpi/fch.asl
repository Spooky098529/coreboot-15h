/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2026 the coreboot project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * AMD SB700 CIMx southbridge ACPI.
 *
 * Defines the southbridge devices and I/O register OperationRegions for
 * the CIMx SB700.  Include this file inside a Device(PCI0) block in
 * \_SB scope, after the northbridge includes.
 *
 * Board-configurable preprocessor defines (set before including):
 *   SB700_HPET_SIZE   - HPET reserved memory size (default 0x400)
 *   SB700_LPC_HAS_PNP0A05 - if defined, LIBR gets PNP0A05 HID
 */

#ifndef SB700_HPET_SIZE
#define SB700_HPET_SIZE 0x00000400
#endif

/* Operating System Capabilities Method */
Method(_OSC,4)
{
	/* Check for proper PCI/PCIe UUID */
	If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")))
	{
		/* Let OS control everything */
		Return (Arg3)
	} Else {
		CreateDWordField(Arg3,0,CDW1)
		Or(CDW1,4,CDW1)	// Unrecognized UUID
		Return(Arg3)
	}
}

Method(_BBN, 0) { /* Bus number = 0 */
	Return(0)
}
Method(_STA, 0) {
	/* DBGO("\\_SB\\PCI0\\_STA\n") */
	Return(0x0B)     /* Status is visible */
}

Method(_PRT,0) {
	If(PMOD){ Return(APR0) }	/* APIC mode */
	Return (PR0)			/* PIC Mode */
} /* end _PRT */

/* Describe the Southbridge devices */

Device(STCR) {
	Name(_ADR, 0x00110000)
	#include "acpi/sata.asl"
} /* end STCR */

Device(UOH1) {
	Name(_ADR, 0x00130000)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH1 */

Device(UOH2) {
	Name(_ADR, 0x00130001)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH2 */

Device(UOH3) {
	Name(_ADR, 0x00130002)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH3 */

Device(UOH4) {
	Name(_ADR, 0x00130003)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH4 */

Device(UOH5) {
	Name(_ADR, 0x00130004)
	Name(_PRW, Package() {0x0B, 3})
} /* end UOH5 */

Device(UEH1) {
	Name(_ADR, 0x00130005)
	Name(_PRW, Package() {0x0B, 3})
} /* end UEH1 */

Device(SBUS) {
	Name(_ADR, 0x00140000)
} /* end SBUS */

Device(AZHD) {
	Name(_ADR, 0x00140002)
	OperationRegion(AZPD, PCI_Config, 0x00, 0x100)
		Field(AZPD, AnyAcc, NoLock, Preserve) {
		offset (0x42),
		NSDI, 1,
		NSDO, 1,
		NSEN, 1,
		offset (0x44),
		IPCR, 4,
		offset (0x54),
		PWST, 2,
		, 6,
		PMEB, 1,
		, 6,
		PMST, 1,
		offset (0x62),
		MMCR, 1,
		offset (0x64),
		MMLA, 32,
		offset (0x68),
		MMHA, 32,
		offset (0x6C),
		MMDT, 16,
	}

	Method(_INI) {
		If(LEqual(OSVR,3)){   /* If we are running Linux */
			Store(zero, NSEN)
			Store(one, NSDO)
			Store(one, NSDI)
		}
	}
} /* end AZHD */

Device(LIBR) {
#ifdef SB700_LPC_HAS_PNP0A05
	Name (_HID, EisaId ("PNP0A05"))
#endif
	Name(_ADR, 0x00140003)

	/* Real Time Clock Device */
	Device(RTC0) {
		Name(_HID, EISAID("PNP0B00"))	/* AT Real Time Clock (not PIIX4 compatible) */
		Name(_CRS, ResourceTemplate() {
			IRQNoFlags(){8}
			IO(Decode16,0x0070, 0x0070, 0, 2)
		})
	} /* End Device(_SB.PCI0.LIBR.RTC0) */

	Device(TMR) {	/* Timer */
		Name(_HID,EISAID("PNP0100"))	/* System Timer */
		Name(_CRS, ResourceTemplate() {
			IRQNoFlags(){0}
			IO(Decode16, 0x0040, 0x0040, 0, 4)
		})
	} /* End Device(_SB.PCI0.LIBR.TMR) */

	Device(SPKR) {	/* Speaker */
		Name(_HID,EISAID("PNP0800"))	/* AT style speaker */
		Name(_CRS, ResourceTemplate() {
			IO(Decode16, 0x0061, 0x0061, 0, 1)
		})
	} /* End Device(_SB.PCI0.LIBR.SPKR) */

	Device(PIC) {
		Name(_HID,EISAID("PNP0000"))	/* AT Interrupt Controller */
		Name(_CRS, ResourceTemplate() {
			IRQNoFlags(){2}
			IO(Decode16,0x0020, 0x0020, 0, 2)
			IO(Decode16,0x00A0, 0x00A0, 0, 2)
		})
	} /* End Device(_SB.PCI0.LIBR.PIC) */

	Device(MAD) { /* 8257 DMA */
		Name(_HID,EISAID("PNP0200"))	/* Hardware Device ID */
		Name(_CRS, ResourceTemplate() {
			DMA(Compatibility,BusMaster,Transfer8){4}
			IO(Decode16, 0x0000, 0x0000, 0x10, 0x10)
			IO(Decode16, 0x0081, 0x0081, 0x01, 0x03)
			IO(Decode16, 0x0087, 0x0087, 0x01, 0x01)
			IO(Decode16, 0x0089, 0x0089, 0x01, 0x03)
			IO(Decode16, 0x008F, 0x008F, 0x01, 0x01)
			IO(Decode16, 0x00C0, 0x00C0, 0x10, 0x20)
		}) /* End Name(_SB.PCI0.LIBR.MAD._CRS) */
	} /* End Device(_SB.PCI0.LIBR.MAD) */

	Device(COPR) {
		Name(_HID,EISAID("PNP0C04"))	/* Math Coprocessor */
		Name(_CRS, ResourceTemplate() {
			IO(Decode16, 0x00F0, 0x00F0, 0, 0x10)
			IRQNoFlags(){13}
		})
	} /* End Device(_SB.PCI0.LIBR.COPR) */

	Device (PS2M) {
		Name (_HID, EisaId ("PNP0F13"))
		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0060, 0x0060, 0x00, 0x01)
			IO (Decode16, 0x0064, 0x0064, 0x00, 0x01)
			IRQNoFlags () {12}
		})
		Method (_STA, 0, NotSerialized) {
			And (FLG0, 0x04, Local0)
			If (LEqual (Local0, 0x04)) {
				Return (0x0F)
			} Else {
				Return (0x00)
			}
		}
	}

	Device (PS2K) {
		Name (_HID, EisaId ("PNP0303"))
		Method (_STA, 0, NotSerialized) {
			And (FLG0, 0x04, Local0)
			If (LEqual (Local0, 0x04)) {
				Return (0x0F)
			} Else {
				Return (0x00)
			}
		}
		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0060, 0x0060, 0x00, 0x01)
			IO (Decode16, 0x0064, 0x0064, 0x00, 0x01)
			IRQNoFlags () {1}
		})
	}

	Device(HPET) {
		Name(_HID,EISAID("PNP0103"))
		Name(CRS, ResourceTemplate() {
			IRQNoFlags () {0}
			IRQNoFlags () {2}
			IRQNoFlags () {8}
			Memory32Fixed(ReadOnly,0xFED00000, SB700_HPET_SIZE, MNT)
		})
		Method(_STA, 0, NotSerialized) {
			Return(0x0F)
		}
		Method(_CRS, 0, NotSerialized) {
			CreateDwordField(CRS, ^MNT._BAS, HPT)
			Store(HPBA, HPT)
			Return(CRS)
		}
	} /* End Device(_SB.PCI0.LIBR.HPET) */
} /* end LIBR */

Device(HPBR) {
	Name(_ADR, 0x00140004)
} /* end HostPciBr */

Device(ACAD) {
	Name(_ADR, 0x00140005)
} /* end Ac97audio */

Device(ACMD) {
	Name(_ADR, 0x00140006)
} /* end Ac97modem */

Name (CRS, ResourceTemplate ()
{
	WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
		0x0000,             // Granularity
		0x0000,             // Range Minimum
		0x00FF,             // Range Maximum
		0x0000,             // Translation Offset
		0x0100,             // Length
		,,)
	IO (Decode16,
		0x0CF8,             // Range Minimum
		0x0CF8,             // Range Maximum
		0x01,               // Alignment
		0x08,               // Length
		)

	WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,             // Granularity
		0x0000,             // Range Minimum
		0x03AF,             // Range Maximum
		0x0000,             // Translation Offset
		0x03B0,             // Length
		,, , TypeStatic)
	WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,             // Granularity
		0x03E0,             // Range Minimum
		0x0CF7,             // Range Maximum
		0x0000,             // Translation Offset
		0x0918,             // Length
		,, , TypeStatic)

	WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,             // Granularity
		0x03B0,             // Range Minimum
		0x03BB,             // Range Maximum
		0x0000,             // Translation Offset
		0x000C,             // Length
		,, , TypeStatic)
	WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,             // Granularity
		0x03C0,             // Range Minimum
		0x03DF,             // Range Maximum
		0x0000,             // Translation Offset
		0x0020,             // Length
		,, , TypeStatic)
	WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
		0x0000,             // Granularity
		0x0D00,             // Range Minimum
		0xFFFF,             // Range Maximum
		0x0000,             // Translation Offset
		0xF300,             // Length
		,, , TypeStatic)
	Memory32Fixed(READONLY, 0x000A0000, 0x00020000, VGAM) 	// VGA memory space

	Memory32Fixed (ReadOnly,
			0xE0000000,         // Address Base
			0x10000000,         // Address Length, (1MB each Bus, 256 Buses by default)
			MMIO)
})

Method (_CRS, 0, NotSerialized)
{
	CreateDWordField (CRS, \_SB.PCI0.MMIO._BAS, BAS1)
	CreateDWordField (CRS, \_SB.PCI0.MMIO._LEN, LEN1)

	/*
	 * Declare memory between TOM1 and 4GB as available
	 * for PCI MMIO.
	 * Use ShiftLeft to avoid 64bit constant (for XP).
	 * This will work even if the OS does 32bit arithmetic, as
	 * 32bit (0x00000000 - TOM1) will wrap and give the same
	 * result as 64bit (0x100000000 - TOM1).
	 */
	Store(TOM1, BAS1)
	ShiftLeft(0x10000000, 4, Local0)
	Subtract(Local0, TOM1, Local0)
	Store(Local0, LEN1)

	Return (CRS)
}

/*
 *
 *               FIRST METHOD CALLED UPON BOOT
 *
 *  1. If debugging, print current OS and ACPI interpreter.
 *  2. Get PCI Interrupt routing from ACPI VSM, this
 *     value is based on user choice in BIOS setup.
 */
Method(_INI, 0) {
	/* DBGO("\\_SB\\_INI\n") */
	/* DBGO("   DSDT.ASL code from ") */
	/* DBGO(__DATE__) */
	/* DBGO(" ") */
	/* DBGO(__TIME__) */
	/* DBGO("\n   Sleep states supported: ") */
	/* DBGO("\n") */
	/* DBGO("   \\_OS=") */
	/* DBGO(\_OS) */
	/* DBGO("\n   \\_REV=") */
	/* DBGO(\_REV) */
	/* DBGO("\n") */

	/* Determine the OS we're running on */
	OSFL()
} /* End Method(_SB._INI) */

/* SB700 I/O OperationRegions — placed at root scope via Scope(\) */
Scope(\){

	/* PIC IRQ mapping registers, C00h-C01h */
	OperationRegion(PRQM, SystemIO, 0x00000C00, 0x00000002)
		Field(PRQM, ByteAcc, NoLock, Preserve) {
		PRQI, 0x00000008,
		PRQD, 0x00000008,  /* Offset: 1h */
	}
	IndexField(PRQI, PRQD, ByteAcc, NoLock, Preserve) {
		PINA, 0x00000008,	/* Index 0 */
		PINB, 0x00000008,	/* Index 1 */
		PINC, 0x00000008,	/* Index 2 */
		PIND, 0x00000008,	/* Index 3 */
		AINT, 0x00000008,	/* Index 4 */
		SINT, 0x00000008,	/* Index 5 */
		    , 0x00000008,	/* Index 6 */
		AAUD, 0x00000008,	/* Index 7 */
		AMOD, 0x00000008,	/* Index 8 */
		PINE, 0x00000008,	/* Index 9 */
		PINF, 0x00000008,	/* Index A */
		PING, 0x00000008,	/* Index B */
		PINH, 0x00000008,	/* Index C */
	}

	/* PCI Error control register */
	OperationRegion(PERC, SystemIO, 0x00000C14, 0x00000001)
		Field(PERC, ByteAcc, NoLock, Preserve) {
		SENS, 0x00000001,
		PENS, 0x00000001,
		SENE, 0x00000001,
		PENE, 0x00000001,
	}

	/* Client Management index/data registers */
	OperationRegion(CMT, SystemIO, 0x00000C50, 0x00000002)
		Field(CMT, ByteAcc, NoLock, Preserve) {
		CMTI, 8,
		/* Client Management Data register */
		G64E, 1,
		G64O, 1,
		G32O, 2,
		    , 2,
		GPSL, 2,
	}

	/* GPM Port register */
	OperationRegion(GPT, SystemIO, 0x00000C52, 0x00000001)
		Field(GPT, ByteAcc, NoLock, Preserve) {
		GPB0,1,
		GPB1,1,
		GPB2,1,
		GPB3,1,
		GPB4,1,
		GPB5,1,
		GPB6,1,
		GPB7,1,
	}

	/* Flash ROM program enable register */
	OperationRegion(FRE, SystemIO, 0x00000C6F, 0x00000001)
		Field(FRE, ByteAcc, NoLock, Preserve) {
		    , 0x00000006,
		FLRE, 0x00000001,
	}

	/* PM2 index/data registers */
	OperationRegion(PM2R, SystemIO, 0x00000CD0, 0x00000002)
		Field(PM2R, ByteAcc, NoLock, Preserve) {
		PM2I, 0x00000008,
		PM2D, 0x00000008,
	}

	/* Power Management I/O registers */
	OperationRegion(PIOR, SystemIO, 0x00000CD6, 0x00000002)
		Field(PIOR, ByteAcc, NoLock, Preserve) {
		PIOI, 0x00000008,
		PIOD, 0x00000008,
	}
	IndexField (PIOI, PIOD, ByteAcc, NoLock, Preserve) {
		Offset(0x00),	/* MiscControl */
		, 1,
		T1EE, 1,
		T2EE, 1,
		Offset(0x01),	/* MiscStatus */
		, 1,
		T1E, 1,
		T2E, 1,
		Offset(0x04),	/* SmiWakeUpEventEnable3 */
		, 7,
		SSEN, 1,
		Offset(0x07),	/* SmiWakeUpEventStatus3 */
		, 7,
		CSSM, 1,
		Offset(0x10),	/* AcpiEnable */
		, 6,
		PWDE, 1,
		Offset(0x1C),	/* ProgramIoEnable */
		, 3,
		MKME, 1,
		IO3E, 1,
		IO2E, 1,
		IO1E, 1,
		IO0E, 1,
		Offset(0x1D),	/* IOMonitorStatus */
		, 3,
		MKMS, 1,
		IO3S, 1,
		IO2S, 1,
		IO1S, 1,
		IO0S,1,
		Offset(0x20),	/* AcpiPmEvtBlk */
		APEB, 16,
		Offset(0x36),	/* GEvtLevelConfig */
		, 6,
		ELC6, 1,
		ELC7, 1,
		Offset(0x37),	/* GPMLevelConfig0 */
		, 3,
		PLC0, 1,
		PLC1, 1,
		PLC2, 1,
		PLC3, 1,
		PLC8, 1,
		Offset(0x38),	/* GPMLevelConfig1 */
		, 1,
		 PLC4, 1,
		 PLC5, 1,
		, 1,
		 PLC6, 1,
		 PLC7, 1,
		Offset(0x3B),	/* PMEStatus1 */
		GP0S, 1,
		GM4S, 1,
		GM5S, 1,
		APS, 1,
		GM6S, 1,
		GM7S, 1,
		GP2S, 1,
		STSS, 1,
		Offset(0x55),	/* SoftPciRst */
		SPRE, 1,
		, 1,
		, 1,
		PNAT, 1,
		PWMK, 1,
		PWNS, 1,

		/* 	Offset(0x61), */	/*  Options_1 */
		/* 		,7,  */
		/* 		R617,1, */

		Offset(0x65),	/* UsbPMControl */
		, 4,
		URRE, 1,
		Offset(0x68),	/* MiscEnable68 */
		, 3,
		TMTE, 1,
		, 1,
		Offset(0x92),	/* GEVENTIN */
		, 7,
		E7IS, 1,
		Offset(0x96),	/* GPM98IN */
		G8IS, 1,
		G9IS, 1,
		Offset(0x9A),	/* EnhanceControl */
		,7,
		HPDE, 1,
		Offset(0xA8),	/* PIO7654Enable */
		IO4E, 1,
		IO5E, 1,
		IO6E, 1,
		IO7E, 1,
		Offset(0xA9),	/* PIO7654Status */
		IO4S, 1,
		IO5S, 1,
		IO6S, 1,
		IO7S, 1,
	}

	/* PM1 Event Block
	 * First word is PM1_Status, Second word is PM1_Enable
	 */
	OperationRegion(P1EB, SystemIO, APEB, 0x04)
		Field(P1EB, ByteAcc, NoLock, Preserve) {
		TMST, 1,
		,    3,
		BMST,    1,
		GBST,   1,
		Offset(0x01),
		PBST, 1,
		, 1,
		RTST, 1,
		, 3,
		PWST, 1,
		SPWS, 1,
		Offset(0x02),
		TMEN, 1,
		, 4,
		GBEN, 1,
		Offset(0x03),
		PBEN, 1,
		, 1,
		RTEN, 1,
		, 3,
		PWDA, 1,
	}

	OperationRegion (GRAM, SystemMemory, 0x0400, 0x0100)
		Field (GRAM, ByteAcc, Lock, Preserve)
		{
			Offset (0x10),
			FLG0,   8
		}
} /* End Scope(\) */

/* SB700 PCIe config space and SATA BAR5 OperationRegions */
Scope(\_SB) {
	/* PCIe Configuration Space for CONFIG_MMCONF_BUS_NUMBER busses */
	OperationRegion(PCFG, SystemMemory, PCBA, PCLN)
		Field(PCFG, ByteAcc, NoLock, Preserve) {
		/* Byte offsets are computed using the following technique:
		 * ((bus number + 1) * ((device number * 8) * 4096)) + register offset
		 * The 8 comes from 8 functions per device, and 4096 bytes per function config space
		*/
		Offset(0x00088024),	/* Byte offset to SATA register 24h - Bus 0, Device 17, Function 0 */
		STB5, 32,
		Offset(0x00098042),	/* Byte offset to OHCI0 register 42h - Bus 0, Device 19, Function 0 */
		PT0D, 1,
		PT1D, 1,
		PT2D, 1,
		PT3D, 1,
		PT4D, 1,
		PT5D, 1,
		PT6D, 1,
		PT7D, 1,
		PT8D, 1,
		PT9D, 1,
		Offset(0x000A0004),	/* Byte offset to SMBUS	register 4h - Bus 0, Device 20, Function 0 */
		SBIE, 1,
		SBME, 1,
		Offset(0x000A0008),	/* Byte offset to SMBUS	register 8h - Bus 0, Device 20, Function 0 */
		SBRI, 8,
		Offset(0x000A0014),	/* Byte offset to SMBUS	register 14h - Bus 0, Device 20, Function 0 */
		SBB1, 32,
		Offset(0x000A0078),	/* Byte offset to SMBUS	register 78h - Bus 0, Device 20, Function 0 */
		,14,
		P92E, 1,		/* Port92 decode enable */
	}

	OperationRegion(SB5, SystemMemory, STB5, 0x1000)
		Field(SB5, AnyAcc, NoLock, Preserve){
		/* Port 0 */
		Offset(0x120),		/* Port 0 Task file status */
		P0ER, 1,
		, 2,
		P0DQ, 1,
		, 3,
		P0BY, 1,
		Offset(0x128),		/* Port 0 Serial ATA status */
		P0DD, 4,
		, 4,
		P0IS, 4,
		Offset(0x12C),		/* Port 0 Serial ATA control */
		P0DI, 4,
		Offset(0x130),		/* Port 0 Serial ATA error */
		, 16,
		P0PR, 1,

		/* Port 1 */
		offset(0x1A0),		/* Port 1 Task file status */
		P1ER, 1,
		, 2,
		P1DQ, 1,
		, 3,
		P1BY, 1,
		Offset(0x1A8),		/* Port 1 Serial ATA status */
		P1DD, 4,
		, 4,
		P1IS, 4,
		Offset(0x1AC),		/* Port 1 Serial ATA control */
		P1DI, 4,
		Offset(0x1B0),		/* Port 1 Serial ATA error */
		, 16,
		P1PR, 1,

		/* Port 2 */
		Offset(0x220),		/* Port 2 Task file status */
		P2ER, 1,
		, 2,
		P2DQ, 1,
		, 3,
		P2BY, 1,
		Offset(0x228),		/* Port 2 Serial ATA status */
		P2DD, 4,
		, 4,
		P2IS, 4,
		Offset(0x22C),		/* Port 2 Serial ATA control */
		P2DI, 4,
		Offset(0x230),		/* Port 2 Serial ATA error */
		, 16,
		P2PR, 1,

		/* Port 3 */
		Offset(0x2A0),		/* Port 3 Task file status */
		P3ER, 1,
		, 2,
		P3DQ, 1,
		, 3,
		P3BY, 1,
		Offset(0x2A8),		/* Port 3 Serial ATA status */
		P3DD, 4,
		, 4,
		P3IS, 4,
		Offset(0x2AC),		/* Port 3 Serial ATA control */
		P3DI, 4,
		Offset(0x2B0),		/* Port 3 Serial ATA error */
		, 16,
		P3PR, 1,
	}
} /* End Scope(\_SB) */
