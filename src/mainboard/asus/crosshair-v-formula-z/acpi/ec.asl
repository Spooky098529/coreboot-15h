/*
 * acpi/ec.asl -- Legacy ISA devices under LPC bridge (desktop board)
 *
 * ASUSTeK COMPUTER INC. SABERTOOTH 990FX R2.0
 *
 * Note: the board has an ENE KB3722Q discrete EC (component O2U2) that
 * masters the VRM SMBus and controls POST LEDs.  It uses ACPI EC ports
 * 0x62/0x66 but is not OS-visible; all communication is firmware-internal.
 * The IT8721F SuperIO at 0x2E provides keyboard, mouse, serial and
 * hardware monitor functions exposed to the OS.
 *
 * These ISA device declarations provide legacy compatibility for the
 * i8042 keyboard controller and PS/2 mouse on the LPC bus.
 */

/* i8042 Keyboard Controller */
Device(KBC0) {
	Name(_HID, EisaId("PNP0303"))
	Method(_STA, 0) { Return(0x0F) }
	Name(_CRS, ResourceTemplate() {
		IO(Decode16, 0x0060, 0x0060, 0x01, 0x01)
		IO(Decode16, 0x0064, 0x0064, 0x01, 0x01)
		IRQNoFlags() { 1 }
	})
}

/* PS/2 Mouse */
Device(PS2M) {
	Name(_HID, EisaId("PNP0F13"))
	Method(_STA, 0) { Return(0x0F) }
	Name(_CRS, ResourceTemplate() {
		IRQNoFlags() { 12 }
	})
}



