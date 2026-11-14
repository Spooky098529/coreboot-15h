/*
 * romstage.c - Pre-RAM initialization for Gigabyte GA-990FXA-UD3 rev 5.x
 *
 * Derived from the ASUS Sabertooth 990FX R2.0 port (same 990FX + SB950 +
 * AGESA family 15h stack). Untested on hardware.
 * NB:  990FX (RD890 CIMx)   SB: SB950 (SB800 CIMx)   SIO: ITE IT8620E
 */

#include <lib.h>
#include <reset.h>
#include <stdint.h>
#include <arch/io.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <arch/stages.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include <cpu/amd/car.h>
#include <device/pnp_ops.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/family15/reset_test.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8620e/it8620e.h>
#include <nb_cimx.h>
#include <sb_cimx.h>
#include <cbmem.h>
#include <southbridge/amd/cimx/sb800/gpio_oem.h>
#include "it8620e_board.h"

/* IT8620E at 0x2E (id 0x8620 rev 0x04). COM1 (LDN1) -> COMA header. */
#define SERIAL_DEV PNP_DEV(0x2e, IT8620E_SP1)

/* Guard variable to prevent repeat hardware init */
static int platform_init_done = 0;

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/* Must come first to enable PCI MMCONF */
	amd_initmmio();

	post_code(0x31);

	/* Halt if there was a built-in self-test failure */
	post_code(0x33);
	report_bist_failure(bist);

	/* Enable Super I/O UART for early console */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	post_code(0x34);

	console_init();

	/*
	 * Gigabyte DualBIOS: log the EC's boot-fail watchdog (left running,
	 * disarmed at the end of ramstage), then report the romstage ("PEI")
	 * stage to the EC exactly as the vendor DualBiosPei does. If the EC
	 * decides boot failed it switches to the backup chip, whose BIOS then
	 * restores itself over coreboot in the main chip.
	 */
	it8620e_log_wdt();
	it8620e_dualbios_stage(DUALBIOS_STAGE_PEI);

	/*
	 * No KB_RST#/PCIRST# (PMIO 0xBE bit1) or SB boot-fail timer (PMIO 0x44)
	 * board code: the vendor's live state equals the CIMx defaults, and
	 * AMD's RPR says to keep kb_pcirst_en set on SB800.
	 */

	/*
	 * SB950 IOMUX: the vendor BIOS leaves every pin at 0 except these four
	 * (dumps/board-dump-20261113-122222/sb_iomux.txt). CIMx only writes
	 * GEVENT21 (and only with the GbE MAC enabled, which it isn't), so set
	 * all four to the vendor values. All GPIOs are inputs under the vendor
	 * BIOS, so no GPIO output state is programmed.
	 */
	FCH_IOMUX(63)  = 2;	/* GPIO63 */
	FCH_IOMUX(99)  = 1;	/* GEVENT3 (IOMUX offset 0x63) */
	FCH_IOMUX(117) = 1;	/* GEVENT21 (IOMUX offset 0x75) */
	FCH_IOMUX(118) = 1;	/* GEVENT22 (IOMUX offset 0x76) */

	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	post_code(0x37);
	printk(BIOS_DEBUG, "Calling agesawrapper_amdinitreset\n");
	agesawrapper_amdinitreset();

	if (!cpu_init_detectedx && boot_cpu() && !platform_init_done) {
		post_code(0x38);
		/*
		 * SR5650/5670/5690/RD890 chipset: reading PCI config
		 * space hangs at POR.  Disable PCIe bridges first.
		 */
		printk(BIOS_DEBUG, "Calling sr56x0_rd890_disable_pcie_bridge\n");
		sr56x0_rd890_disable_pcie_bridge();
		post_code(0x39);
		printk(BIOS_DEBUG, "Calling nb_Poweron_Init\n");
		nb_Poweron_Init();
		post_code(0x3A);
		printk(BIOS_DEBUG, "Calling sb_Poweron_Init\n");
		sb_Poweron_Init();
		platform_init_done = 1;
	}

	post_code(0x3B);
	printk(BIOS_DEBUG, "Calling agesawrapper_amdinitearly\n");
	agesawrapper_amdinitearly();

	post_code(0x3C);
	printk(BIOS_DEBUG, "amdinitearly done, calling nb_Ht_Init\n");
	nb_Ht_Init();
	post_code(0x3D);
	/*
	 * Perform warm reset for HT, FIDVID, PLL, and microcode
	 * patch (errata) changes to take effect.
	 */
	if (!warm_reset_detect(0)) {
		printk(BIOS_INFO, "...WARM RESET...\n\n\n");
		distinguish_cpu_resets(0);
		board_reset();
		die("After soft_reset - shouldn't see this message!!!\n");
	}

	printk(BIOS_DEBUG, "Warm reset detected, continuing boot\n");
	post_code(0x40);
	printk(BIOS_DEBUG, "Calling agesawrapper_amdinitpost (memory init)\n");
	agesawrapper_amdinitpost();

	cbmem_initialize_empty();

	post_code(0x41);
	agesawrapper_amdinitenv();
	post_code(0x42);

	printk(BIOS_DEBUG, "Romstage completed successfully!\n");
	post_code(0x50);
	printk(BIOS_DEBUG, "Disabling cache as RAM... ");
	disable_cache_as_ram();
	printk(BIOS_DEBUG, "done\n");

	post_code(0x51);
	printk(BIOS_DEBUG, "Calling copy_and_run to jump to ramstage\n");
	copy_and_run();

	/* Should never reach here */
	post_code(0x54);
}
