/*
 * acpi/mainboard.asl -- Global ACPI variables and OS detection
 *
 */

Name(LOMH, 0x0)
Name(PBAD, 0x0)
Name(PBLN, 0x0)
Name(PCBA, CONFIG_MMCONF_BASE_ADDRESS)  /* MMCONFIG base */
Name(HPTS, 0xFED00000)  /* HPET base */
Name(OSVR, 3)  /* OS default: Linux */
Name(OSV, Ones)
Name(PMOD, One)  /* PIC mode: 1=APIC */

Name(PICM, 0)
Method(_PIC, 1, NotSerialized) {
	If (Arg0) {
		\_SB.CIRQ()  /* Clear PIC-mode PIRQ routing when switching to APIC */
	}
	PICM = Arg0
	PMOD = Arg0
}

Scope(\_SB) {
        /* The below may need to be expanded for conditional OS-detection,
           if OS workarounds are needed */
	Method(OSFL, 0) {
		if (LNotEqual(OSVR, Ones)) { Return(OSVR) }
		if (CondRefOf(\_OS)) {
			Store(3, OSVR)  /* default Linux */
			if (LEqual(\_OS, "Microsoft Windows NT")) {
				Store(0, OSVR)
			}
			if (LEqual(\_OS, "Microsoft WindowsME: Millennium Edition")) {
				Store(1, OSVR)
			}
			if (LEqual(\_OS, "Linux")) {
				Store(3, OSVR)
			}
		}
		Return(OSVR)
	}
	Device(HPET) {
		Name(_HID, EisaId("PNP0103"))
		Name(_UID, 0x00)
		Method(_STA, 0, NotSerialized) { Return(0x0F) }
		Name(_CRS, ResourceTemplate() {
			Memory32Fixed(ReadOnly, 0xFED00000, 0x400)
			IRQNoFlags() { 0 }  /* PIT legacy */
			IRQNoFlags() { 8 }  /* RTC legacy */
		})
	}
}

Scope(\_SI) {
	Method(_SST, 1) { }
}
