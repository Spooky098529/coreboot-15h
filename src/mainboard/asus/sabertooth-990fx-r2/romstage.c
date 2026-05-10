/*
 * romstage.c - Pre-RAM initialization for ASUSTeK COMPUTER INC. SABERTOOTH 990FX R2.0
 *
 * Tested with -
 * CPU: AMD FX(tm)-8350 Eight-Core Processor (Family 15h AGESA)
 * NB:  990FX (RD890 CIMx)   SB: SB950 (SB800 CIMx)
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
#include <superio/ite/it8721f/it8721f.h>
#include <nb_cimx.h>
#include <sb_cimx.h>
#include <cbmem.h>
#include <vendorcode/amd/cimx/sb800/SB800.h>
#include <southbridge/amd/cimx/sb800/gpio_oem.h>
#include "ec.h"
#include "vrm_asp1000c.h"

/* Super I/O at 0x2e - confirmed via probe */
/* Single COM1 header on board connected to superio */
#define SERIAL_DEV PNP_DEV(0x2e, IT8721F_SP1)

/* Guard variable to prevent repeat hardware init */
static int platform_init_done = 0;

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/* Must come first to enable PCI MMCONF */
	amd_initmmio();

	/*
	 * Assert 2X16_SW (GPIO61/62) HIGH as early as possible.
	 *
	 * GPIO61 (AGPIO61/SATA_IS0#) and GPIO62 (AGPIO62/SATA_IS1#) are the
	 * 2X16_SW control signals for the on-board PCIe lane mux.  They select
	 * the GPP2 core lane to slot allocation:
	 *   HIGH = AAAA (NB port 11 gets all 16 GPP2 lanes -> PCIEX16_3 x16)
	 *   LOW  = AABB (GPP2 lanes split x8+x8 -> PCIEX16_3 and an x8 alt slot)
         * This is only really useful if you want x8+x8 to combine with the
         * the primary PCIEX16_1 for triple-SLI or if you have multiple x8 cards.
	 *
	 * Power-on hardware default: iomux=0 (SATA_IS0#/SATA_IS1# function).
	 * The SATA controller may assert SATA_IS0#/IS1# low (e.g. during
	 * device presence detection), driving 2X16_SW low on a cold boot.
	 * If the board's power sequencer releases PCIe PERST# before coreboot
	 * reaches ramstage, the PCIe endpoint in PCIEX16_3 power up with
	 * 2X16_SW=LOW (AABB mode), causing CIMx gpp2_configuration=AAAA
	 * training for port 11 to fail as the slot gets electrically switched
         * across two separate slots.
	 *
	 * This runs prior to any PCIe training and asserts the internal pull-up
         * and sets the values early so training happens with signals configured
         * correctly.
	 *
	 * 0x2A = bits[5,3,1]: pull-up enable, input-enable, output-config.
	 * TODO: Add logic to select the preferred port/lane allocation.
	 */
	FCH_IOMUX(61) = 2;   /* 2X16_SW: switch from SATA_IS0# to GPIO */
	FCH_GPIO(61) = 0x2A; /* 2X16_SW: pull-up HIGH, keep GPP2 in default lane config */
	FCH_IOMUX(62) = 2;   /* 2X16_SW: switch from SATA_IS1# to GPIO */
	FCH_GPIO(62) = 0x2A; /* 2X16_SW: pull-up HIGH, keep GPP2 in default lane config */

	post_code(0x31);

	/* Halt if there was a built-in self-test failure */
	post_code(0x33);
	report_bist_failure(bist);

	/* Enable Super I/O UART for early console */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	post_code(0x34);

	kb3722q_ec_init();
	asp1000c_init(); /* must run before AGESA changes CPU voltage */

	console_init();

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

	kb3722q_post_memory();

#if CONFIG(BOARD_VRM_REPORT)
	{
		int vcore_mv;
		if (asp1000c_get_vcore_mv(&vcore_mv) == 0)
			printk(BIOS_INFO,
			       "VRM: CPU VCore = %d mV (after AGESA)\n",
			       vcore_mv);
	}
#endif

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
