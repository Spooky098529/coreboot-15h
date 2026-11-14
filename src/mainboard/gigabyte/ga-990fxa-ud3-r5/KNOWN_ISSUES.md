# GA-990FXA-UD3 R5: known issues

Tested on rev 5.x with an FX-8350, SeaBIOS payload, Linux. Boot, CPU P-states,
fan control, SATA, USB 2.0 and the reset button work. The items below are
open or only partly understood.

## Reset button: fixed with a board-level workaround for a shared-code bug

The shared `southbridge/amd/cimx/sb800/late.c` ("A13+ errata" block in
`sb800_init()`) sets SB950 PMIO 0xC1 bit 2, taken from SB900 code as "RPR
2.14: Enable delayed SLP_S3/S5". On SB800/SB950 that bit is UserRst2EcEn,
which routes reset-button events to the integrated microcontroller (IMC):
"The bit should be set to 1 when IMC is enabled" (`SBCMN.c`). The IMC is off
on this board, and the vendor BIOS leaves the bit at 0.

With the bit set, the reset button only partly reset the southbridge: its
USB controllers and device 00:14.6 kept their state from the previous boot
(USB 2.0 was dead once), and a reset pressed about 20 min or more after
power-on made the IT8620E EC start the DualBIOS recovery, which overwrites
coreboot with the vendor BIOS (3 out of 3 times).

`mainboard_final()` now clears the bit again (`sb950_user_reset_to_sb()`).
Tested: resets a few minutes and 25 minutes after power-on both work, and the
southbridge comes back fully reset. This is based on a small number of
tests. The proper fix belongs in `late.c` (only set the bit when the IMC is
enabled); other SB800/SB950 boards in this tree probably have the same
partial reset.

## Legacy ISA interrupts are not delivered (PS/2 keyboard, RTC)

Under this port, Linux never receives IRQ 1 (keyboard), 8 (RTC), 9 (ACPI SCI)
or 12 (PS/2 mouse port), and the timer (IRQ 0) doesn't reach the IOAPIC.
It is the same with `noapic` (8259 mode), so the interrupts are lost before
either interrupt controller. The vendor BIOS doesn't have this problem.

Visible effects:
- **PS/2 keyboard doesn't work in Linux.** The keyboard controller and the
  keyboard respond (an `i8042.debug=1` trace shows the keyboard's ACKs, and
  SeaBIOS, which polls, initializes it), but IRQ 1 never fires. Use a USB
  keyboard.
- **`hwclock` fails** with "select() to /dev/rtc to wait for clock tick timed
  out", because the RTC's update interrupt never arrives. The kernel still
  sets the clock at boot. Workaround: `hwclock --directisa` (OpenRC:
  `clock_args="--directisa"` in `/etc/conf.d/hwclock`).
- Linux logs `..MP-BIOS bug: 8254 timer not connected to IO-APIC` and falls
  back to a virtual-wire timer. The comment in `acpi_tables.c` about leaving
  out the IRQ0 -> GSI2 override is a symptom of this, not a fix.

Lead (untested): the SB950 PCI IRQ routing table (ports 0xC00/0xC01) in
`mainboard.c`, copied from the Sabertooth port. Its APIC table sets index
0x09 (MISC0) to 0x00 "disabled"; coreboot's older native SB800 code writes
0xF7 there ("Enabled IRQ input"). The Sabertooth and Crosshair V ports use the
same tables and probably have the same problem.

PS/2 power is fine (GP17 is driven high, as the vendor BIOS does). IT8620E
LDN5 register 0xF0 reads 0x48 here and 0x08 under the vendor BIOS; setting
0x08 at runtime made no difference.

## IT8620E watchdog / DualBIOS handling is reverse-engineered

The IT8620E's EC firmware arms the LDN7 watchdog at power-on (timeout 56,
CR72 = 0x20, CR73 = 0x38). Gigabyte uses it for DualBIOS boot-failure
detection. This port, like the vendor BIOS:
1. romstage: leaves the watchdog running (only logs it), sends DualBIOS
   stage 6 (LDN7 CR EF).
2. ramstage (`mainboard_final`): sends stage 7, then turns the watchdog off
   (CR73 = CR74 = 0) and clears the "reached 0" status bit (CR71 bit 0),
   which zeroing a running counter sometimes sets. LDN7 CR70-7F and CRE0-EF
   then match the vendor BIOS's end state.

coreboot reaches step 2 about 5 s after romstage starts, well inside the
EC's limit. If a boot hangs, the EC falls back to the backup BIOS chip, which
restores the vendor BIOS (see below).

The disarm matters: without it, pressing the reset button after boot made
the EC treat the boot as failed and start from the backup BIOS chip. The
vendor BIOS there then overwrites coreboot in the main chip automatically
(coreboot has no UEFI firmware volumes). Software reboots were never
affected. Late resets also needed the southbridge fix above.

Not fully understood:
- The EC firmware is closed; the behaviour above is inferred from tests.
- LDN7 CR F0 and CR F6 still differ from the vendor's end state (vendor 0x10 /
  0x1c, coreboot 0x00 / 0x0e). Their meaning is unknown.
- The timeout unit (seconds or minutes) is unknown; 56 s is the worst case.

## SB800 CIMx sbAfterPciInit / sbMidPostInit never run

On this AGESA_LEGACY family 15h board, only `sb_Before_Pci_Init()` and
`sb_Late_Post()` are called. `sb_After_Pci_Init()` and `sb_Mid_Post_Init()`
are only called from `northbridge/amd/agesa/family1{2,4,5}/state_machine.c`,
which is not built here. The coreboot log confirms it. Other AGESA_LEGACY
fam15h + SB800/SB950 boards in this tree probably have the same gap.

So the following are skipped:
- `usbInitAfterPciInit`: per-EHCI USB PHY calibration (BAR+0xC0), FIFO
  thresholds (BAR+0xA4), rsync flops (BAR+0xB4), and the RPR settings in PCI
  config 0x50/0x54. Under the vendor BIOS, EHCI config 0x50-0x56 reads
  `40 xx 80 a0 01 00 00`; under this port it reads `00 xx 08 80 7b aa 0f`.
- `sataInitAfterPciEnum` and `sataInitMidPost`
- `gecInitAfterPciEnum`, `azaliaInitAfterPciEnum` and `hwmInit`

USB 2.0 and SATA work without them, so nothing visibly breaks, but the chip
is not set up to AMD's recommended settings. Possible fix (untested): call
`sb_After_Pci_Init()` after resource allocation and before device init, and
check that it doesn't conflict with coreboot's azalia verb loading and the
"post-CIMx" USB fixups in `southbridge/amd/cimx/sb800/late.c`.

## Other notes

- **USB 3.0 (VIA VL805):** the chip has no SPI flash. The vendor BIOS loads
  its firmware every boot. This port contains no vendor firmware, so the
  controller stays down until the firmware is loaded from the OS (done from
  Linux with a small userspace loader). Until then, Linux's xhci probe fails
  with -110, which delays boot by about 20 s (5 s in
  `quirk_usb_early_handoff`, the rest in the failing probe).
- **Not supported:** S3 suspend, XMP, a setup UI. Windows is untested.
