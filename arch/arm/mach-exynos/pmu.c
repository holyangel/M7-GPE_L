/* linux/arch/arm/mach-exynos4/pmu.c
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * EXYNOS4210 - CPU PMU(Power Management Unit) support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/io.h>
#include <linux/kernel.h>

#include <mach/regs-clock.h>
#include <mach/pmu.h>

static struct exynos4_pmu_conf *exynos4_pmu_config;

static struct exynos4_pmu_conf exynos4210_pmu_config[] = {
	/* { .reg = address, .val = { AFTR, LPA, SLEEP } */
	{ S5P_ARM_CORE0_LOWPWR,			{ 0x0, 0x0, 0x2 } },
	{ S5P_DIS_IRQ_CORE0,			{ 0x0, 0x0, 0x0 } },
	{ S5P_DIS_IRQ_CENTRAL0,			{ 0x0, 0x0, 0x0 } },
	{ S5P_ARM_CORE1_LOWPWR,			{ 0x0, 0x0, 0x2 } },
	{ S5P_DIS_IRQ_CORE1,			{ 0x0, 0x0, 0x0 } },
	{ S5P_DIS_IRQ_CENTRAL1,			{ 0x0, 0x0, 0x0 } },
	{ S5P_ARM_COMMON_LOWPWR,		{ 0x0, 0x0, 0x2 } },
	{ S5P_L2_0_LOWPWR,			{ 0x2, 0x2, 0x3 } },
	{ S5P_L2_1_LOWPWR,			{ 0x2, 0x2, 0x3 } },
	{ S5P_CMU_ACLKSTOP_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_SCLKSTOP_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_LOWPWR,			{ 0x1, 0x1, 0x0 } },
	{ S5P_APLL_SYSCLK_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_MPLL_SYSCLK_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_VPLL_SYSCLK_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_EPLL_SYSCLK_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_GPS_ALIVE_LOWPWR,	{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_GPSALIVE_LOWPWR,	{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_CAM_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_TV_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_MFC_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_G3D_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_LCD0_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_LCD1_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_MAUDIO_LOWPWR,	{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_CLKSTOP_GPS_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_CAM_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_TV_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_MFC_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_G3D_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_LCD0_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_LCD1_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_MAUDIO_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_GPS_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_TOP_BUS_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_TOP_RETENTION_LOWPWR,		{ 0x1, 0x0, 0x1 } },
	{ S5P_TOP_PWR_LOWPWR,			{ 0x3, 0x0, 0x3 } },
	{ S5P_LOGIC_RESET_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_ONENAND_MEM_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_MODIMIF_MEM_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_G2D_ACP_MEM_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_USBOTG_MEM_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_HSMMC_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_CSSYS_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_SECSS_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_PCIE_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_SATA_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_DRAM_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_MAUDIO_LOWPWR,	{ 0x1, 0x1, 0x0 } },
	{ S5P_PAD_RETENTION_GPIO_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_UART_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_MMCA_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_MMCB_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_EBIA_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_EBIB_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_ISOLATION_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_ALV_SEL_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_XUSBXTI_LOWPWR,			{ 0x1, 0x1, 0x0 } },
	{ S5P_XXTI_LOWPWR,			{ 0x1, 0x1, 0x0 } },
	{ S5P_EXT_REGULATOR_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_GPIO_MODE_LOWPWR,			{ 0x1, 0x0, 0x0 } },
	{ S5P_GPIO_MODE_MAUDIO_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CAM_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_TV_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_MFC_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_G3D_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_LCD0_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_LCD1_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_MAUDIO_LOWPWR,			{ 0x7, 0x7, 0x0 } },
	{ S5P_GPS_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_GPS_ALIVE_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ PMU_TABLE_END,},
};

static struct exynos4_pmu_conf exynos4212_pmu_config[] = {
	{ S5P_ARM_CORE0_LOWPWR,			{ 0x0, 0x0, 0x2 } },
	{ S5P_DIS_IRQ_CORE0,			{ 0x0, 0x0, 0x0 } },
	{ S5P_DIS_IRQ_CENTRAL0,			{ 0x0, 0x0, 0x0 } },
	{ S5P_ARM_CORE1_LOWPWR,			{ 0x0, 0x0, 0x2 } },
	{ S5P_DIS_IRQ_CORE1,			{ 0x0, 0x0, 0x0 } },
	{ S5P_DIS_IRQ_CENTRAL1,			{ 0x0, 0x0, 0x0 } },
	{ S5P_ISP_ARM_LOWPWR,			{ 0x1, 0x0, 0x0 } },
	{ S5P_DIS_IRQ_ISP_ARM_LOCAL_LOWPWR,	{ 0x0, 0x0, 0x0 } },
	{ S5P_DIS_IRQ_ISP_ARM_CENTRAL_LOWPWR,	{ 0x0, 0x0, 0x0 } },
	{ S5P_ARM_COMMON_LOWPWR,		{ 0x0, 0x0, 0x2 } },
	{ S5P_L2_0_LOWPWR,			{ 0x0, 0x0, 0x3 } },
	/* XXX_OPTION register should be set other field */
	{ S5P_ARM_L2_0_OPTION,			{ 0x10, 0x10, 0x0 } },
	{ S5P_L2_1_LOWPWR,			{ 0x0, 0x0, 0x3 } },
	{ S5P_ARM_L2_1_OPTION,			{ 0x10, 0x10, 0x0 } },
	{ S5P_CMU_ACLKSTOP_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_SCLKSTOP_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_LOWPWR,			{ 0x1, 0x1, 0x0 } },
	{ S5P_DRAM_FREQ_DOWN_LOWPWR,		{ 0x1, 0x1, 0x1 } },
	{ S5P_DDRPHY_DLLOFF_LOWPWR,		{ 0x1, 0x1, 0x1 } },
	{ S5P_LPDDR_PHY_DLL_LOCK_LOWPWR,	{ 0x1, 0x1, 0x1 } },
	{ S5P_CMU_ACLKSTOP_COREBLK_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_SCLKSTOP_COREBLK_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_COREBLK_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_APLL_SYSCLK_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_MPLL_SYSCLK_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_VPLL_SYSCLK_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_EPLL_SYSCLK_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_MPLLUSER_SYSCLK_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_GPS_ALIVE_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_GPSALIVE_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_CAM_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_TV_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_MFC_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_G3D_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_LCD0_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_ISP_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_MAUDIO_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_CLKSTOP_GPS_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_CAM_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_TV_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_MFC_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_G3D_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_LCD0_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_ISP_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_RESET_MAUDIO_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_CMU_RESET_GPS_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_TOP_BUS_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_TOP_RETENTION_LOWPWR,		{ 0x1, 0x0, 0x1 } },
	{ S5P_TOP_PWR_LOWPWR,			{ 0x3, 0x0, 0x3 } },
	{ S5P_TOP_BUS_COREBLK_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_TOP_RETENTION_COREBLK_LOWPWR,	{ 0x1, 0x0, 0x1 } },
	{ S5P_TOP_PWR_COREBLK_LOWPWR,		{ 0x3, 0x0, 0x3 } },
	{ S5P_LOGIC_RESET_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_OSCCLK_GATE_LOWPWR,		{ 0x1, 0x0, 0x1 } },
	{ S5P_LOGIC_RESET_COREBLK_LOWPWR,	{ 0x1, 0x1, 0x0 } },
	{ S5P_OSCCLK_GATE_COREBLK_LOWPWR,	{ 0x1, 0x0, 0x1 } },
	{ S5P_ONENAND_MEM_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_ONENAND_MEM_OPTION,		{ 0x10, 0x10, 0x0 } },
	{ S5P_HSI_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_HSI_MEM_OPTION,			{ 0x10, 0x10, 0x0 } },
	{ S5P_G2D_ACP_MEM_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_G2D_ACP_MEM_OPTION,		{ 0x10, 0x10, 0x0 } },
	{ S5P_USBOTG_MEM_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_USBOTG_MEM_OPTION,		{ 0x10, 0x10, 0x0 } },
	{ S5P_HSMMC_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_HSMMC_MEM_OPTION,			{ 0x10, 0x10, 0x0 } },
	{ S5P_CSSYS_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_CSSYS_MEM_OPTION,			{ 0x10, 0x10, 0x0 } },
	{ S5P_SECSS_MEM_LOWPWR,			{ 0x3, 0x0, 0x0 } },
	{ S5P_SECSS_MEM_OPTION,			{ 0x10, 0x10, 0x0 } },
	{ S5P_ROTATOR_MEM_LOWPWR,		{ 0x3, 0x0, 0x0 } },
	{ S5P_ROTATOR_MEM_OPTION,		{ 0x10, 0x10, 0x0 } },
	{ S5P_PAD_RETENTION_DRAM_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_MAUDIO_LOWPWR,	{ 0x1, 0x1, 0x0 } },
	{ S5P_PAD_RETENTION_GPIO_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_UART_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_MMCA_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_MMCB_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_EBIA_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_EBIB_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_GPIO_COREBLK_LOWPWR,{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_ISOLATION_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_ISOLATION_COREBLK_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_PAD_RETENTION_ALV_SEL_LOWPWR,	{ 0x1, 0x0, 0x0 } },
	{ S5P_XUSBXTI_LOWPWR,			{ 0x1, 0x1, 0x0 } },
	{ S5P_XXTI_LOWPWR,			{ 0x1, 0x1, 0x0 } },
	{ S5P_EXT_REGULATOR_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_GPIO_MODE_LOWPWR,			{ 0x1, 0x0, 0x0 } },
	{ S5P_GPIO_MODE_COREBLK_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_GPIO_MODE_MAUDIO_LOWPWR,		{ 0x1, 0x1, 0x0 } },
	{ S5P_TOP_ASB_RESET_LOWPWR,		{ 0x1, 0x1, 0x1 } },
	{ S5P_TOP_ASB_ISOLATION_LOWPWR,		{ 0x1, 0x0, 0x1 } },
	{ S5P_CAM_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_TV_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_MFC_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_G3D_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_LCD0_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_ISP_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_MAUDIO_LOWPWR,			{ 0x7, 0x7, 0x0 } },
	{ S5P_GPS_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_GPS_ALIVE_LOWPWR,			{ 0x7, 0x0, 0x0 } },
	{ S5P_CMU_SYSCLK_ISP_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ S5P_CMU_SYSCLK_GPS_LOWPWR,		{ 0x1, 0x0, 0x0 } },
	{ PMU_TABLE_END,},
};

void exynos4_sys_powerdown_conf(enum sys_powerdown mode)
{
	unsigned int i;

	for (i = 0; (exynos4_pmu_config[i].reg != PMU_TABLE_END) ; i++)
		__raw_writel(exynos4_pmu_config[i].val[mode],
				exynos4_pmu_config[i].reg);
}

static int __init exynos4_pmu_init(void)
{
	exynos4_pmu_config = exynos4210_pmu_config;

	if (soc_is_exynos4210()) {
		exynos4_pmu_config = exynos4210_pmu_config;
		pr_info("EXYNOS4210 PMU Initialize\n");
	} else if (soc_is_exynos4212()) {
		exynos4_pmu_config = exynos4212_pmu_config;
		pr_info("EXYNOS4212 PMU Initialize\n");
	} else {
		pr_info("EXYNOS4: PMU not supported\n");
	}

	return 0;
}
arch_initcall(exynos4_pmu_init);
