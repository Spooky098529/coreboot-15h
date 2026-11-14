# GA-990FXA-UD3 R5: known issues

Tested on rev 5.x with an FX-8350, SeaBIOS payload, Linux. Boot, CPU P-states,
fan control, SATA and USB 2.0 work. The items below are open.

## PS/2 keyboard does not work

The PS/2 keyboard gets power (GP17 is driven high, as the vendor BIOS does),
but it does not work in SeaBIOS, GRUB or Linux. Its Caps Lock LED flashes
periodically. Use a USB keyboard instead.

Not yet investigated:
- The vendor BIOS writes IT8620E LDN5 (KBC) register 0xF0 = 0x08. This port
  never writes it, and its meaning is unknown (no datasheet).
- `CONFIG_DRIVERS_PS2_KEYBOARD` is unset, so `pc_keyboard_init()` does nothing.
- `CONFIG_SEABIOS_PS2_TIMEOUT` is 0.

## IT8620E watchdog / DualBIOS handling is reverse-engineered

The IT8620E's EC firmware arms the LDN7 watchdog at power-on (timeout 56,
CR72 = 0x20, CR73 = 0x38). Gigabyte uses it for DualBIOS boot-failure
detection. This port, like the vendor BIOS:
1. romstage: leaves the watchdog running (only logs it), sends DualBIOS
   stage 6 (LDN7 CR EF).
2. ramstage (`mainboard_final`): sends stage 7, then turns the watchdog off
   (CR73 = CR74 = 0), which is the vendor BIOS's end state.

coreboot reaches step 2 about 5 s after romstage starts, well inside the
EC's limit. If a boot hangs, the EC falls back to the backup BIOS chip, which
restores the vendor BIOS (see below).

The disarm matters: without it, pressing the **reset button** after boot
made the EC treat the boot as failed and start from the backup BIOS chip. The
vendor BIOS there then overwrites coreboot in the main chip automatically
(coreboot has no UEFI firmware volumes). With it, reset works normally.
Software reboots were never affected. Tested on hardware.

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
  with -110, and there is a ~5 s `quirk_usb_early_handoff` delay at boot.
- **Not supported:** S3 suspend, XMP, a setup UI. Windows is untested.
