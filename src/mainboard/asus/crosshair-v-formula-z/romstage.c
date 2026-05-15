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

static void setup_gpio(void) {
	FCH_IOMUX(54) = 1;
	FCH_IOMUX(56) = 1;
	FCH_IOMUX(57) = 1;

	FCH_IOMUX(59) = 2;
	FCH_IOMUX(61) = 2;
	FCH_IOMUX(62) = 2;

	FCH_IOMUX(65) = 3;

	FCH_IOMUX(99) = 1;
	FCH_IOMUX(103) = 1;
	FCH_IOMUX(106) = 1;
	FCH_IOMUX(107) = 1;
	FCH_IOMUX(119) = 1;
	FCH_IOMUX(171) = 1;
	FCH_IOMUX(172) = 1;
	FCH_IOMUX(173) = 1;

	FCH_IOMUX(174) = 2;

	FCH_IOMUX(175) = 1;
	FCH_IOMUX(176) = 1;
	FCH_IOMUX(177) = 1;
	FCH_IOMUX(178) = 1;
	FCH_IOMUX(179) = 1;
	FCH_IOMUX(180) = 1;

	FCH_IOMUX(181) = 2;
	FCH_IOMUX(182) = 2;

	FCH_IOMUX(210) = 1;
	FCH_IOMUX(212) = 1;
	FCH_IOMUX(213) = 1;
	FCH_IOMUX(216) = 1;

	FCH_GPIO(54) = 0x2A;

	FCH_GPIO(56) = 0xAA;
	FCH_GPIO(57) = 0xAA;
	FCH_GPIO(59) = 0xAA;
	FCH_GPIO(61) = 0xAA;
	FCH_GPIO(62) = 0xAA;

	FCH_GPIO(103) = 0xA8;

	FCH_GPIO(106) = 0xA0;

	FCH_GPIO(167) = 0x3E;
	FCH_GPIO(168) = 0x3E;
	FCH_GPIO(169) = 0x3E;
	FCH_GPIO(170) = 0x3E;

	FCH_GPIO(171) = 0x02;
	FCH_GPIO(172) = 0x02;
	FCH_GPIO(173) = 0x02;
	FCH_GPIO(174) = 0x02;

	FCH_GPIO(175) = 0xC2;

	FCH_GPIO(176) = 0x2A;

	FCH_GPIO(177) = 0x3A;

	FCH_GPIO(178) = 0xAA;

	FCH_GPIO(179) = 0x02;

	FCH_GPIO(180) = 0x2A;
	FCH_GPIO(181) = 0x2A;
	FCH_GPIO(182) = 0x2A;

	FCH_GPIO(210) = 0xC2;
	FCH_GPIO(212) = 0xC2;
	FCH_GPIO(213) = 0xC2;

	FCH_GPIO(216) = 0xA2;
}

static void cpu_limit_tdp(void) {
	u32 val;
	int cpu_limit;

	// D18F4x10C TDP Limit 2
	val = pci_read_config32(PCI_DEV(0, 0x18, 4), 0x10C);
	cpu_limit = val & (0xfff);
	printk(BIOS_INFO, "CPU TDP Limit: %d W\n", cpu_limit);

	if(CONFIG_BOARD_CPU_TDP_LIMIT < 1) return;

	if(CONFIG_BOARD_CPU_TDP_LIMIT > cpu_limit) {
		printk(BIOS_ERR, "The requested CPU TDP Limit (%d) is invalid\n", CONFIG_BOARD_CPU_TDP_LIMIT);
		return;
	}

	val &= ~(0xfff);
	val |= (CONFIG_BOARD_CPU_TDP_LIMIT & 0xfff);
	pci_write_config32(PCI_DEV(0, 0x18, 4), 0x10C, val);
	printk(BIOS_INFO, "New CPU TDP Limit: %d W\n", CONFIG_BOARD_CPU_TDP_LIMIT);
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/* Must come first to enable PCI MMCONF */
	amd_initmmio();

	setup_gpio();

	post_code(0x31);

	/* Halt if there was a built-in self-test failure */
	post_code(0x33);
	report_bist_failure(bist);

	/* Enable Super I/O UART for early console */
	//ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
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

	cpu_limit_tdp();

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
