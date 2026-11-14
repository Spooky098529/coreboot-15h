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

## IT8620E watchdog handling is a best guess

The IT8620E's EC firmware arms the LDN7 watchdog at power-on. The vendor
BIOS uses it for DualBIOS boot-failure detection. A timeout of 56 was
observed (CR72 = 0x20, CR73 = 0x38).

romstage re-arms it to the maximum (CR73 = CR74 = 0xFF, CR72 = 0x20) so it
cannot fire during boot. It then sends the same DualBIOS stage handshake as
the vendor BIOS (LDN7 CR EF: stage 6 in romstage, stage 7 in ramstage). It
never disarms the watchdog.

- After a vendor boot, CR73/74 read 0. After a coreboot boot they read 0xFFFF:
  the EC did not clear the value coreboot wrote.
- The register did not change over 150 s of sampling, so the counter is
  probably stopped. It is not proven, because it is unknown whether CR73/74
  read back the live count.
- The unit is also unknown. CR72 bit 7 = 0 means minutes on other ITE parts
  (65535 min is about 45 days). If it is seconds, 65535 s is about 18 h.
- If it does fire, KRST# resets the board, and the EC may then boot from the
  backup BIOS chip. The vendor BIOS there overwrites coreboot in the main chip
  automatically, because coreboot has no UEFI firmware volumes.

Possible fix (untested): write CR73 = CR74 = 0 after the stage-7 handshake,
so the end state matches the vendor's.

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
