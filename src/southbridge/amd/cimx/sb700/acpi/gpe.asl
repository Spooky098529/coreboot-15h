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
 * AMD SB700 CIMx GPE (General Purpose Event) handlers.
 *
 * Include this file at root DefinitionBlock scope.
 * Board-specific USB overcurrent GPE handlers (_L06, _L07, _L0E,
 * _L13-_L17, _L19, _L1A) are provided by the board's acpi/usb.asl.
 */

Scope(\_GPE) {	/* Start Scope GPE */
	/*  General event 0  */
	Method(_L00) {
	      //DBGO("\\_GPE\\_L00\n")
	}

	/*  General event 1  */
	Method(_L01) {
	      //DBGO("\\_GPE\\_L01\n")
	}

	/*  General event 2  */
	Method(_L02) {
	      //DBGO("\\_GPE\\_L02\n")
	}

	/*  General event 3  */
	Method(_L03) {
		//DBGO("\\_GPE\\_L00\n")
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  General event 4  */
	Method(_L04) {
	      //DBGO("\\_GPE\\_L04\n")
	}

	/*  General event 5  */
	Method(_L05) {
	      //DBGO("\\_GPE\\_L05\n")
	}

	/* _L06 General event 6 - Used for GPM6, moved to USB.asl */
	/* _L07 General event 7 - Used for GPM7, moved to USB.asl */

	/*  Legacy PM event  */
	Method(_L08) {
		//DBGO("\\_GPE\\_L08\n")
	}

	/*  Temp warning (TWarn) event  */
	Method(_L09) {
		//DBGO("\\_GPE\\_L09\n")
		Notify (\_TZ.TZ00, 0x80)
	}

	/*  Reserved  */
	Method(_L0A) {
	      //DBGO("\\_GPE\\_L0A\n")
	}

	/*  USB controller PME#  */
	Method(_L0B) {
		//DBGO("\\_GPE\\_L0B\n")
		Notify(\_SB.PCI0.UOH1, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH2, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH3, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH4, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH5, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UEH1, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  AC97 controller PME#  */
	Method(_L0C) {
	      //DBGO("\\_GPE\\_L0C\n")
	}

	/*  OtherTherm PME#  */
	Method(_L0D) {
	      //DBGO("\\_GPE\\_L0D\n")
	}

	/* _L0E GPM9 SCI event - Moved to USB.asl */

	/*  PCIe HotPlug event  */
	Method(_L0F) {
		//DBGO("\\_GPE\\_L0F\n")
	}

	/*  ExtEvent0 SCI event  */
	Method(_L10) {
		//DBGO("\\_GPE\\_L10\n")
	}


	/*  ExtEvent1 SCI event  */
	Method(_L11) {
		//DBGO("\\_GPE\\_L11\n")
	}

	/*  PCIe PME# event  */
	Method(_L12) {
	      //DBGO("\\_GPE\\_L12\n")
	}

	/* _L13 GPM0 SCI event - Moved to USB.asl */
	/* _L14 GPM1 SCI event - Moved to USB.asl */
	/* _L15 GPM2 SCI event - Moved to USB.asl */
	/* _L16 GPM3 SCI event - Moved to USB.asl */
	/* _L17 GPM8 SCI event - Moved to USB.asl */

	/*  GPIO0 or GEvent8 event  */
	Method(_L18) {
		//DBGO("\\_GPE\\_L18\n")
		Notify(\_SB.PCI0.PBR2, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR4, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBRb, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBRc, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBRd, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/* _L19 GPM4 SCI event - Moved to USB.asl */
	/* _L1A GPM5 SCI event - Moved to USB.asl */

	/*  Azalia SCI event  */
	Method(_L1B) {
		//DBGO("\\_GPE\\_L1B\n")
		Notify(\_SB.PCI0.AZHD, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  GPM6 SCI event - Reassigned to _L06 */
	Method(_L1C) {
	      //DBGO("\\_GPE\\_L1C\n")
	}

	/*  GPM7 SCI event - Reassigned to _L07 */
	Method(_L1D) {
	      //DBGO("\\_GPE\\_L1D\n")
	}

	/*  GPIO2 or GPIO66 SCI event  */
	Method(_L1E) {
		//DBGO("\\_GPE\\_L1E\n")
	}

	/* _L1F SATA SCI event - Moved to sata.asl */

} 	/* End Scope GPE */
