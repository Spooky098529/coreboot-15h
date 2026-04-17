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
 * AMD SB700 CIMx PCI interrupt link devices.
 *
 * Provides OSFL(), CIRQ(), and PCI interrupt link devices INTA-INTH
 * (PNP0C0F).  Include this file inside a Scope(\_SB) block, before
 * Device(PCI0).
 */

Method(OSFL, 0){
	if(LNotEqual(OSVR, Ones)) {Return(OSVR)}	/* OS version was already detected */
	if(CondRefOf(\_OSI))
	{
		Store(1, OSVR)			/* Assume some form of XP */
		if (\_OSI("Windows 2006"))      /* Vista */
		{
			Store(2, OSVR)
		}
	} else {
		If(WCMP(\_OS,"Linux")) {
			Store(3, OSVR)		/* Linux */
		} Else {
			Store(4, OSVR)		/* Gotta be WinCE */
		}
	}
	Return(OSVR)
}

Method(CIRQ, 0x00, NotSerialized){
	Store(0, PINA)
	Store(0, PINB)
	Store(0, PINC)
	Store(0, PIND)
	Store(0, PINE)
	Store(0, PINF)
	Store(0, PING)
	Store(0, PINH)
}

Name(IRQB, ResourceTemplate(){
	IRQ(Level,ActiveLow,Shared){15}
})

Name(IRQP, ResourceTemplate(){
	IRQ(Level,ActiveLow,Exclusive){3, 4, 5, 7, 10, 11, 12, 15}
})

Name(PITF, ResourceTemplate(){
	IRQ(Level,ActiveLow,Exclusive){9}
})

Device(INTA) {
	Name(_HID, EISAID("PNP0C0F"))
	Name(_UID, 1)

	Method(_STA, 0) {
		if (PINA) {
			Return(0x0B) /* sata is invisible */
		} else {
			Return(0x09) /* sata is disabled */
		}
	} /* End Method(_SB.INTA._STA) */

	Method(_DIS ,0) {
		/* DBGO("\\_SB\\LNKA\\_DIS\n") */
		Store(0, PINA)
	} /* End Method(_SB.INTA._DIS) */

	Method(_PRS ,0) {
		/* DBGO("\\_SB\\LNKA\\_PRS\n") */
		Return(IRQP)
	} /* Method(_SB.INTA._PRS) */

	Method(_CRS ,0) {
		/* DBGO("\\_SB\\LNKA\\_CRS\n") */
		CreateWordField(IRQB, 0x1, IRQN)
		ShiftLeft(1, PINA, IRQN)
		Return(IRQB)
	} /* Method(_SB.INTA._CRS) */

	Method(_SRS, 1) {
		/* DBGO("\\_SB\\LNKA\\_CRS\n") */
		CreateWordField(ARG0, 1, IRQM)

		/* Use lowest available IRQ */
		FindSetRightBit(IRQM, Local0)
		if (Local0) {
			Decrement(Local0)
		}
		Store(Local0, PINA)
	} /* End Method(_SB.INTA._SRS) */
} /* End Device(INTA) */

Device(INTB) {
	Name(_HID, EISAID("PNP0C0F"))
	Name(_UID, 2)

	Method(_STA, 0) {
		if (PINB) {
			Return(0x0B)
		} else {
			Return(0x09)
		}
	}

	Method(_DIS ,0) {
		Store(0, PINB)
	}

	Method(_PRS ,0) {
		Return(IRQP)
	}

	Method(_CRS ,0) {
		CreateWordField(IRQB, 0x1, IRQN)
		ShiftLeft(1, PINB, IRQN)
		Return(IRQB)
	}

	Method(_SRS, 1) {
		CreateWordField(ARG0, 1, IRQM)
		FindSetRightBit(IRQM, Local0)
		if (Local0) {
			Decrement(Local0)
		}
		Store(Local0, PINB)
	}
} /* End Device(INTB) */

Device(INTC) {
	Name(_HID, EISAID("PNP0C0F"))
	Name(_UID, 3)

	Method(_STA, 0) {
		if (PINC) {
			Return(0x0B)
		} else {
			Return(0x09)
		}
	}

	Method(_DIS ,0) {
		Store(0, PINC)
	}

	Method(_PRS ,0) {
		Return(IRQP)
	}

	Method(_CRS ,0) {
		CreateWordField(IRQB, 0x1, IRQN)
		ShiftLeft(1, PINC, IRQN)
		Return(IRQB)
	}

	Method(_SRS, 1) {
		CreateWordField(ARG0, 1, IRQM)
		FindSetRightBit(IRQM, Local0)
		if (Local0) {
			Decrement(Local0)
		}
		Store(Local0, PINC)
	}
} /* End Device(INTC) */

Device(INTD) {
	Name(_HID, EISAID("PNP0C0F"))
	Name(_UID, 4)

	Method(_STA, 0) {
		if (PIND) {
			Return(0x0B)
		} else {
			Return(0x09)
		}
	}

	Method(_DIS ,0) {
		Store(0, PIND)
	}

	Method(_PRS ,0) {
		Return(IRQP)
	}

	Method(_CRS ,0) {
		CreateWordField(IRQB, 0x1, IRQN)
		ShiftLeft(1, PIND, IRQN)
		Return(IRQB)
	}

	Method(_SRS, 1) {
		CreateWordField(ARG0, 1, IRQM)
		FindSetRightBit(IRQM, Local0)
		if (Local0) {
			Decrement(Local0)
		}
		Store(Local0, PIND)
	}
} /* End Device(INTD) */

Device(INTE) {
	Name(_HID, EISAID("PNP0C0F"))
	Name(_UID, 5)

	Method(_STA, 0) {
		if (PINE) {
			Return(0x0B)
		} else {
			Return(0x09)
		}
	}

	Method(_DIS ,0) {
		Store(0, PINE)
	}

	Method(_PRS ,0) {
		Return(IRQP)
	}

	Method(_CRS ,0) {
		CreateWordField(IRQB, 0x1, IRQN)
		ShiftLeft(1, PINE, IRQN)
		Return(IRQB)
	}

	Method(_SRS, 1) {
		CreateWordField(ARG0, 1, IRQM)
		FindSetRightBit(IRQM, Local0)
		if (Local0) {
			Decrement(Local0)
		}
		Store(Local0, PINE)
	}
} /* End Device(INTE) */

Device(INTF) {
	Name(_HID, EISAID("PNP0C0F"))
	Name(_UID, 6)

	Method(_STA, 0) {
		if (PINF) {
			Return(0x0B)
		} else {
			Return(0x09)
		}
	}

	Method(_DIS ,0) {
		Store(0, PINF)
	}

	Method(_PRS ,0) {
		Return(PITF)
	}

	Method(_CRS ,0) {
		CreateWordField(IRQB, 0x1, IRQN)
		ShiftLeft(1, PINF, IRQN)
		Return(IRQB)
	}

	Method(_SRS, 1) {
		CreateWordField(ARG0, 1, IRQM)
		FindSetRightBit(IRQM, Local0)
		if (Local0) {
			Decrement(Local0)
		}
		Store(Local0, PINF)
	}
} /* End Device(INTF) */

Device(INTG) {
	Name(_HID, EISAID("PNP0C0F"))
	Name(_UID, 7)

	Method(_STA, 0) {
		if (PING) {
			Return(0x0B)
		} else {
			Return(0x09)
		}
	}

	Method(_DIS ,0) {
		Store(0, PING)
	}

	Method(_PRS ,0) {
		Return(IRQP)
	}

	Method(_CRS ,0) {
		CreateWordField(IRQB, 0x1, IRQN)
		ShiftLeft(1, PING, IRQN)
		Return(IRQB)
	}

	Method(_SRS, 1) {
		CreateWordField(ARG0, 1, IRQM)
		FindSetRightBit(IRQM, Local0)
		if (Local0) {
			Decrement(Local0)
		}
		Store(Local0, PING)
	}
} /* End Device(INTG) */

Device(INTH) {
	Name(_HID, EISAID("PNP0C0F"))
	Name(_UID, 8)

	Method(_STA, 0) {
		if (PINH) {
			Return(0x0B)
		} else {
			Return(0x09)
		}
	}

	Method(_DIS ,0) {
		Store(0, PINH)
	}

	Method(_PRS ,0) {
		Return(IRQP)
	}

	Method(_CRS ,0) {
		CreateWordField(IRQB, 0x1, IRQN)
		ShiftLeft(1, PINH, IRQN)
		Return(IRQB)
	}

	Method(_SRS, 1) {
		CreateWordField(ARG0, 1, IRQM)
		FindSetRightBit(IRQM, Local0)
		if (Local0) {
			Decrement(Local0)
		}
		Store(Local0, PINH)
	}
} /* End Device(INTH) */
