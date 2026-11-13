/*
 * acpi/ec.asl -- Legacy ISA devices under LPC bridge (desktop board)
 *
 * GIGABYTE GA-990FXA-UD3 R5: the ITE IT8620E Super I/O at 0x2E provides the
 * PS/2 combo port (LDN5 keyboard, LDN6 mouse), COM1 and the hardware
 * monitor. There is no separate ACPI embedded controller.
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
