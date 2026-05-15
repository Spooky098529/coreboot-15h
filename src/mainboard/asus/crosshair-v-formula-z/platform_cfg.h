/*
 * platform_cfg.h - Board-specific platform configuration
 */

#ifndef PLATFORM_CFG_H
#define PLATFORM_CFG_H

/* Compute BIOS_SIZE from Kconfig ROM size */
#define BIOS_SIZE ((CONFIG_COREBOOT_ROMSIZE_KB >> 10) - 1)

/* Clock */
#define SPREAD_SPECTRUM 0

/* HPET - confirmed by MMIO probe */
#define HPET_TIMER 1
#define HPET_MIN_TICKS 0x37EE /* from ACPI HPET table, matches vendor */

/* RTC century register (CMOS offset) */
#define RTC_CENTURY 0x32

/* USB - bitmask from probed USB controllers */
#define USB_CONFIG 0x7F

/* PCI clock */
#define PCI_CLOCK_CTRL 0x07 /* enable 3 PCI clocks */

/* SATA */
#define SATA_CONTROLLER CIMX_OPTION_ENABLED
#define SATA_MODE CONFIG_SB800_SATA_MODE
#define IDE_LEGACY_MODE 0
#define IDE_NATIVE_MODE 1
#define SATA_IDE_MODE IDE_LEGACY_MODE
#define EXTERNAL_CLOCK 0x00
#define INTERNAL_CLOCK 0x01
#define SATA_CLOCK_SOURCE INTERNAL_CLOCK
#define SATA_PORT_MULT_CAP_RESERVED 1

/* HD Audio / Azalia */
#define AZALIA_AUTO 0
#define AZALIA_DISABLE 1
#define AZALIA_ENABLE 2
#define AZALIA_CONTROLLER AZALIA_AUTO
#define AZALIA_PIN_CONFIG 1
#define AZALIA_SDIN_PIN 0x02

/* GPP */
#define GPP_CONTROLLER CIMX_OPTION_ENABLED
#define GPP_CFGMODE GPP_CFGMODE_X1111
#define NB_SB_GEN2 TRUE
#define SB_GPP_GEN2 TRUE
#define SB_GPP_UNHIDE_PORTS FALSE
#define GEC_CONFIG 0

/* ACPI PM profile - 1=Desktop, 2=Mobile (from SMBIOS chassis type 3) */
#define FADT_PM_PROFILE 1

/* HT link configuration */
#define RD890_TRANSMITTER_DEEMPHASIS 0x02

/* NB configuration */
#define MAX_NB_COUNT 1

#define RD890_MAINBOARD_OVERRIDE_CFG

#endif /* PLATFORM_CFG_H */
