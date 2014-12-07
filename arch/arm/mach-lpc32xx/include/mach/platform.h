/*
 * arch/arm/mach-lpc32xx/include/mach/platform.h
 *
 * Author: Kevin Wells <kevin.wells@nxp.com>
 *
 * Copyright (C) 2010 NXP Semiconductors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ASM_ARCH_PLATFORM_H
#define __ASM_ARCH_PLATFORM_H

#define _SBF(f, v)				((v) << (f))
#define _BIT(n)					_SBF(n, 1)

/*
 * AHB 0 physical base addresses
 */
#define LPC32XX_SLC_BASE			0x20020000
#define LPC32XX_SSP0_BASE			0x20084000
#define LPC32XX_SPI1_BASE			0x20088000
#define LPC32XX_SSP1_BASE			0x2008C000
#define LPC32XX_SPI2_BASE			0x20090000
#define LPC32XX_I2S0_BASE			0x20094000
#define LPC32XX_SD_BASE				0x20098000
#define LPC32XX_I2S1_BASE			0x2009C000
#define LPC32XX_MLC_BASE			0x200A8000
#define LPC32XX_AHB0_START			LPC32XX_SLC_BASE
#define LPC32XX_AHB0_SIZE			0x00089000

/*
 * AHB 1 physical base addresses
 */
#define LPC32XX_DMA_BASE			0x31000000
#define LPC32XX_USB_BASE			0x31020000
#define LPC32XX_USBH_BASE			0x31020000
#define LPC32XX_USB_OTG_BASE			0x31020000
#define LPC32XX_OTG_I2C_BASE			0x31020300
#define LPC32XX_LCD_BASE			0x31040000
#define LPC32XX_ETHERNET_BASE			0x31060000
#define LPC32XX_EMC_BASE			0x31080000
#define LPC32XX_ETB_CFG_BASE			0x310C0000
#define LPC32XX_ETB_DATA_BASE			0x310E0000
#define LPC32XX_AHB1_START			LPC32XX_DMA_BASE
#define LPC32XX_AHB1_SIZE			0x000E1000

/*
 * FAB physical base addresses
 */
#define LPC32XX_CLK_PM_BASE			0x40004000
#define LPC32XX_MIC_BASE			0x40008000
#define LPC32XX_SIC1_BASE			0x4000C000
#define LPC32XX_SIC2_BASE			0x40010000
#define LPC32XX_HS_UART1_BASE			0x40014000
#define LPC32XX_HS_UART2_BASE			0x40018000
#define LPC32XX_HS_UART7_BASE			0x4001C000
#define LPC32XX_RTC_BASE			0x40024000
#define LPC32XX_RTC_RAM_BASE			0x40024080
#define LPC32XX_GPIO_BASE			0x40028000
#define LPC32XX_PWM3_BASE			0x4002C000
#define LPC32XX_PWM4_BASE			0x40030000
#define LPC32XX_MSTIM_BASE			0x40034000
#define LPC32XX_HSTIM_BASE			0x40038000
#define LPC32XX_WDTIM_BASE			0x4003C000
#define LPC32XX_DEBUG_CTRL_BASE			0x40040000
#define LPC32XX_TIMER0_BASE			0x40044000
#define LPC32XX_ADC_BASE			0x40048000
#define LPC32XX_TIMER1_BASE			0x4004C000
#define LPC32XX_KSCAN_BASE			0x40050000
#define LPC32XX_UART_CTRL_BASE			0x40054000
#define LPC32XX_TIMER2_BASE			0x40058000
#define LPC32XX_PWM1_BASE			0x4005C000
#define LPC32XX_PWM2_BASE			0x4005C004
#define LPC32XX_TIMER3_BASE			0x40060000

/*
 * APB physical base addresses
 */
#define LPC32XX_UART3_BASE			0x40080000
#define LPC32XX_UART4_BASE			0x40088000
#define LPC32XX_UART5_BASE			0x40090000
#define LPC32XX_UART6_BASE			0x40098000
#define LPC32XX_I2C1_BASE			0x400A0000
#define LPC32XX_I2C2_BASE			0x400A8000

/*
 * FAB and APB base and sizing
 */
#define LPC32XX_FABAPB_START			LPC32XX_CLK_PM_BASE
#define LPC32XX_FABAPB_SIZE			0x000A5000

/*
 * Internal memory bases and sizes
 */
#define LPC32XX_IRAM_BASE			0x08000000
#define LPC32XX_IROM_BASE			0x0C000000

/*
 * External Static Memory Bank Address Space Bases
 */
#define LPC32XX_EMC_CS0_BASE			0xE0000000
#define LPC32XX_EMC_CS1_BASE			0xE1000000
#define LPC32XX_EMC_CS2_BASE			0xE2000000
#define LPC32XX_EMC_CS3_BASE			0xE3000000

/*
 * External SDRAM Memory Bank Address Space Bases
 */
#define LPC32XX_EMC_DYCS0_BASE			0x80000000
#define LPC32XX_EMC_DYCS1_BASE			0xA0000000

/*
 * Clock and crystal information
 */
#define LPC32XX_MAIN_OSC_FREQ			13000000
#define LPC32XX_CLOCK_OSC_FREQ			32768

/*
 * Clock and Power control register offsets
 */
#define _PMREG(x)				io_p2v(LPC32XX_CLK_PM_BASE +\
						(x))
#define LPC32XX_CLKPWR_DEBUG_CTRL		_PMREG(0x000)
#define LPC32XX_CLKPWR_BOOTMAP			_PMREG(0x014)
#define LPC32XX_CLKPWR_P01_ER			_PMREG(0x018)
#define LPC32XX_CLKPWR_USBCLK_PDIV		_PMREG(0x01C)
#define LPC32XX_CLKPWR_INT_ER			_PMREG(0x020)
#define LPC32XX_CLKPWR_INT_RS			_PMREG(0x024)
#define LPC32XX_CLKPWR_INT_SR			_PMREG(0x028)
#define LPC32XX_CLKPWR_INT_AP			_PMREG(0x02C)
#define LPC32XX_CLKPWR_PIN_ER			_PMREG(0x030)
#define LPC32XX_CLKPWR_PIN_RS			_PMREG(0x034)
#define LPC32XX_CLKPWR_PIN_SR			_PMREG(0x038)
#define LPC32XX_CLKPWR_PIN_AP			_PMREG(0x03C)
#define LPC32XX_CLKPWR_HCLK_DIV			_PMREG(0x040)
#define LPC32XX_CLKPWR_PWR_CTRL			_PMREG(0x044)
#define LPC32XX_CLKPWR_PLL397_CTRL		_PMREG(0x048)
#define LPC32XX_CLKPWR_MAIN_OSC_CTRL		_PMREG(0x04C)
#define LPC32XX_CLKPWR_SYSCLK_CTRL		_PMREG(0x050)
#define LPC32XX_CLKPWR_LCDCLK_CTRL		_PMREG(0x054)
#define LPC32XX_CLKPWR_HCLKPLL_CTRL		_PMREG(0x058)
#define LPC32XX_CLKPWR_ADC_CLK_CTRL_1		_PMREG(0x060)
#define LPC32XX_CLKPWR_USB_CTRL			_PMREG(0x064)
#define LPC32XX_CLKPWR_SDRAMCLK_CTRL		_PMREG(0x068)
#define LPC32XX_CLKPWR_DDR_LAP_NOM		_PMREG(0x06C)
#define LPC32XX_CLKPWR_DDR_LAP_COUNT		_PMREG(0x070)
#define LPC32XX_CLKPWR_DDR_LAP_DELAY		_PMREG(0x074)
#define LPC32XX_CLKPWR_SSP_CLK_CTRL		_PMREG(0x078)
#define LPC32XX_CLKPWR_I2S_CLK_CTRL		_PMREG(0x07C)
#define LPC32XX_CLKPWR_MS_CTRL			_PMREG(0x080)
#define LPC32XX_CLKPWR_MACCLK_CTRL		_PMREG(0x090)
#define LPC32XX_CLKPWR_TEST_CLK_SEL		_PMREG(0x0A4)
#define LPC32XX_CLKPWR_SFW_INT			_PMREG(0x0A8)
#define LPC32XX_CLKPWR_I2C_CLK_CTRL		_PMREG(0x0AC)
#define LPC32XX_CLKPWR_KEY_CLK_CTRL		_PMREG(0x0B0)
#define LPC32XX_CLKPWR_ADC_CLK_CTRL		_PMREG(0x0B4)
#define LPC32XX_CLKPWR_PWM_CLK_CTRL		_PMREG(0x0B8)
#define LPC32XX_CLKPWR_TIMER_CLK_CTRL		_PMREG(0x0BC)
#define LPC32XX_CLKPWR_TIMERS_PWMS_CLK_CTRL_1	_PMREG(0x0C0)
#define LPC32XX_CLKPWR_SPI_CLK_CTRL		_PMREG(0x0C4)
#define LPC32XX_CLKPWR_NAND_CLK_CTRL		_PMREG(0x0C8)
#define LPC32XX_CLKPWR_UART3_CLK_CTRL		_PMREG(0x0D0)
#define LPC32XX_CLKPWR_UART4_CLK_CTRL		_PMREG(0x0D4)
#define LPC32XX_CLKPWR_UART5_CLK_CTRL		_PMREG(0x0D8)
#define LPC32XX_CLKPWR_UART6_CLK_CTRL		_PMREG(0x0DC)
#define LPC32XX_CLKPWR_IRDA_CLK_CTRL		_PMREG(0x0E0)
#define LPC32XX_CLKPWR_UART_CLK_CTRL		_PMREG(0x0E4)
#define LPC32XX_CLKPWR_DMA_CLK_CTRL		_PMREG(0x0E8)
#define LPC32XX_CLKPWR_AUTOCLOCK		_PMREG(0x0EC)
#define LPC32XX_CLKPWR_DEVID(x)			_PMREG(0x130 + (x))

/*
 * clkpwr_debug_ctrl register definitions
*/
#define LPC32XX_CLKPWR_VFP_CLOCK_ENABLE_BIT	_BIT(4)

/*
 * clkpwr_bootmap register definitions
 */
#define LPC32XX_CLKPWR_BOOTMAP_SEL_BIT		_BIT(1)

/*
 * clkpwr_start_gpio register bit definitions
 */
#define LPC32XX_CLKPWR_GPIOSRC_P1IO23_BIT	_BIT(31)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO22_BIT	_BIT(30)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO21_BIT	_BIT(29)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO20_BIT	_BIT(28)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO19_BIT	_BIT(27)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO18_BIT	_BIT(26)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO17_BIT	_BIT(25)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO16_BIT	_BIT(24)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO15_BIT	_BIT(23)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO14_BIT	_BIT(22)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO13_BIT	_BIT(21)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO12_BIT	_BIT(20)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO11_BIT	_BIT(19)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO10_BIT	_BIT(18)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO9_BIT	_BIT(17)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO8_BIT	_BIT(16)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO7_BIT	_BIT(15)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO6_BIT	_BIT(14)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO5_BIT	_BIT(13)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO4_BIT	_BIT(12)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO3_BIT	_BIT(11)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO2_BIT	_BIT(10)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO1_BIT	_BIT(9)
#define LPC32XX_CLKPWR_GPIOSRC_P1IO0_BIT	_BIT(8)
#define LPC32XX_CLKPWR_GPIOSRC_P0IO7_BIT	_BIT(7)
#define LPC32XX_CLKPWR_GPIOSRC_P0IO6_BIT	_BIT(6)
#define LPC32XX_CLKPWR_GPIOSRC_P0IO5_BIT	_BIT(5)
#define LPC32XX_CLKPWR_GPIOSRC_P0IO4_BIT	_BIT(4)
#define LPC32XX_CLKPWR_GPIOSRC_P0IO3_BIT	_BIT(3)
#define LPC32XX_CLKPWR_GPIOSRC_P0IO2_BIT	_BIT(2)
#define LPC32XX_CLKPWR_GPIOSRC_P0IO1_BIT	_BIT(1)
#define LPC32XX_CLKPWR_GPIOSRC_P0IO0_BIT	_BIT(0)

/*
 * clkpwr_usbclk_pdiv register definitions
 */
#define LPC32XX_CLKPWR_USBPDIV_PLL_MASK		0xF

/*
 * clkpwr_start_int, clkpwr_start_raw_sts_int, clkpwr_start_sts_int,
 * clkpwr_start_pol_int, register bit definitions
 */
#define LPC32XX_CLKPWR_INTSRC_ADC_BIT		_BIT(31)
#define LPC32XX_CLKPWR_INTSRC_TS_P_BIT		_BIT(30)
#define LPC32XX_CLKPWR_INTSRC_TS_AUX_BIT	_BIT(29)
#define LPC32XX_CLKPWR_INTSRC_USBAHNEEDCLK_BIT	_BIT(26)
#define LPC32XX_CLKPWR_INTSRC_MSTIMER_BIT	_BIT(25)
#define LPC32XX_CLKPWR_INTSRC_RTC_BIT		_BIT(24)
#define LPC32XX_CLKPWR_INTSRC_USBNEEDCLK_BIT	_BIT(23)
#define LPC32XX_CLKPWR_INTSRC_USB_BIT		_BIT(22)
#define LPC32XX_CLKPWR_INTSRC_I2C_BIT		_BIT(21)
#define LPC32XX_CLKPWR_INTSRC_USBOTGTIMER_BIT	_BIT(20)
#define LPC32XX_CLKPWR_INTSRC_USBATXINT_BIT	_BIT(19)
#define LPC32XX_CLKPWR_INTSRC_KEY_BIT		_BIT(16)
#define LPC32XX_CLKPWR_INTSRC_MAC_BIT		_BIT(7)
#define LPC32XX_CLKPWR_INTSRC_P0P1_BIT		_BIT(6)
#define LPC32XX_CLKPWR_INTSRC_GPIO_05_BIT	_BIT(5)
#define LPC32XX_CLKPWR_INTSRC_GPIO_04_BIT	_BIT(4)
#define LPC32XX_CLKPWR_INTSRC_GPIO_03_BIT	_BIT(3)
#define LPC32XX_CLKPWR_INTSRC_GPIO_02_BIT	_BIT(2)
#define LPC32XX_CLKPWR_INTSRC_GPIO_01_BIT	_BIT(1)
#define LPC32XX_CLKPWR_INTSRC_GPIO_00_BIT	_BIT(0)

/*
 * clkpwr_start_pin, clkpwr_start_raw_sts_pin, clkpwr_start_sts_pin,
 * clkpwr_start_pol_pin register bit definitions
 */
#define LPC32XX_CLKPWR_EXTSRC_U7_RX_BIT		_BIT(31)
#define LPC32XX_CLKPWR_EXTSRC_U7_HCTS_BIT	_BIT(30)
#define LPC32XX_CLKPWR_EXTSRC_U6_IRRX_BIT	_BIT(28)
#define LPC32XX_CLKPWR_EXTSRC_U5_RX_BIT		_BIT(26)
#define LPC32XX_CLKPWR_EXTSRC_GPI_28_BIT	_BIT(25)
#define LPC32XX_CLKPWR_EXTSRC_U3_RX_BIT		_BIT(24)
#define LPC32XX_CLKPWR_EXTSRC_U2_HCTS_BIT	_BIT(23)
#define LPC32XX_CLKPWR_EXTSRC_U2_RX_BIT		_BIT(22)
#define LPC32XX_CLKPWR_EXTSRC_U1_RX_BIT		_BIT(21)
#define LPC32XX_CLKPWR_EXTSRC_MSDIO_INT_BIT	_BIT(18)
#define LPC32XX_CLKPWR_EXTSRC_MSDIO_SRT_BIT	_BIT(17)
#define LPC32XX_CLKPWR_EXTSRC_GPI_06_BIT	_BIT(16)
#define LPC32XX_CLKPWR_EXTSRC_GPI_05_BIT	_BIT(15)
#define LPC32XX_CLKPWR_EXTSRC_GPI_04_BIT	_BIT(14)
#define LPC32XX_CLKPWR_EXTSRC_GPI_03_BIT	_BIT(13)
#define LPC32XX_CLKPWR_EXTSRC_GPI_02_BIT	_BIT(12)
#define LPC32XX_CLKPWR_EXTSRC_GPI_01_BIT	_BIT(11)
#define LPC32XX_CLKPWR_EXTSRC_GPI_00_BIT	_BIT(10)
#define LPC32XX_CLKPWR_EXTSRC_SYSCLKEN_BIT	_BIT(9)
#define LPC32XX_CLKPWR_EXTSRC_SPI1_DATIN_BIT	_BIT(8)
#define LPC32XX_CLKPWR_EXTSRC_GPI_07_BIT	_BIT(7)
#define LPC32XX_CLKPWR_EXTSRC_SPI2_DATIN_BIT	_BIT(6)
#define LPC32XX_CLKPWR_EXTSRC_GPI_19_BIT	_BIT(5)
#define LPC32XX_CLKPWR_EXTSRC_GPI_09_BIT	_BIT(4)
#define LPC32XX_CLKPWR_EXTSRC_GPI_08_BIT	_BIT(3)

/*
 * clkpwr_hclk_div register definitions
 */
#define LPC32XX_CLKPWR_HCLKDIV_DDRCLK_STOP	(0x0 << 7)
#define LPC32XX_CLKPWR_HCLKDIV_DDRCLK_NORM	(0x1 << 7)
#define LPC32XX_CLKPWR_HCLKDIV_DDRCLK_HALF	(0x2 << 7)
#define LPC32XX_CLKPWR_HCLKDIV_PCLK_DIV(n)	(((n) & 0x1F) << 2)
#define LPC32XX_CLKPWR_HCLKDIV_DIV_2POW(n)	((n) & 0x3)

/*
 * clkpwr_pwr_ctrl register definitions
 */
#define LPC32XX_CLKPWR_CTRL_FORCE_PCLK		_BIT(10)
#define LPC32XX_CLKPWR_SDRAM_SELF_RFSH		_BIT(9)
#define LPC32XX_CLKPWR_UPD_SDRAM_SELF_RFSH	_BIT(8)
#define LPC32XX_CLKPWR_AUTO_SDRAM_SELF_RFSH	_BIT(7)
#define LPC32XX_CLKPWR_HIGHCORE_STATE_BIT	_BIT(5)
#define LPC32XX_CLKPWR_SYSCLKEN_STATE_BIT	_BIT(4)
#define LPC32XX_CLKPWR_SYSCLKEN_GPIO_EN		_BIT(3)
#define LPC32XX_CLKPWR_SELECT_RUN_MODE		_BIT(2)
#define LPC32XX_CLKPWR_HIGHCORE_GPIO_EN		_BIT(1)
#define LPC32XX_CLKPWR_STOP_MODE_CTRL		_BIT(0)

/*
 * clkpwr_pll397_ctrl register definitions
 */
#define LPC32XX_CLKPWR_PLL397_MSLOCK_STS	_BIT(10)
#define LPC32XX_CLKPWR_PLL397_BYPASS		_BIT(9)
#define LPC32XX_CLKPWR_PLL397_BIAS_NORM		0x000
#define LPC32XX_CLKPWR_PLL397_BIAS_N12_5	0x040
#define LPC32XX_CLKPWR_PLL397_BIAS_N25		0x080
#define LPC32XX_CLKPWR_PLL397_BIAS_N37_5	0x0C0
#define LPC32XX_CLKPWR_PLL397_BIAS_P12_5	0x100
#define LPC32XX_CLKPWR_PLL397_BIAS_P25		0x140
#define LPC32XX_CLKPWR_PLL397_BIAS_P37_5	0x180
#define LPC32XX_CLKPWR_PLL397_BIAS_P50		0x1C0
#define LPC32XX_CLKPWR_PLL397_BIAS_MASK		0x1C0
#define LPC32XX_CLKPWR_SYSCTRL_PLL397_DIS	_BIT(1)
#define LPC32XX_CLKPWR_SYSCTRL_PLL397_STS	_BIT(0)

/*
 * clkpwr_main_osc_ctrl register definitions
 */
#define LPC32XX_CLKPWR_MOSC_ADD_CAP(n)		(((n) & 0x7F) << 2)
#define LPC32XX_CLKPWR_MOSC_CAP_MASK		(0x7F << 2)
#define LPC32XX_CLKPWR_TEST_MODE		_BIT(1)
#define LPC32XX_CLKPWR_MOSC_DISABLE		_BIT(0)

/*
 * clkpwr_sysclk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_SYSCTRL_BP_TRIG(n)	(((n) & 0x3FF) << 2)
#define LPC32XX_CLKPWR_SYSCTRL_BP_MASK		(0x3FF << 2)
#define LPC32XX_CLKPWR_SYSCTRL_USEPLL397	_BIT(1)
#define LPC32XX_CLKPWR_SYSCTRL_SYSCLKMUX	_BIT(0)

/*
 * clkpwr_lcdclk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_TFT12	0x000
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_TFT16	0x040
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_TFT15	0x080
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_TFT24	0x0C0
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_STN4M	0x100
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_STN8C	0x140
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_DSTN4M	0x180
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_DSTN8C	0x1C0
#define LPC32XX_CLKPWR_LCDCTRL_LCDTYPE_MSK	0x01C0
#define LPC32XX_CLKPWR_LCDCTRL_CLK_EN		0x020
#define LPC32XX_CLKPWR_LCDCTRL_SET_PSCALE(n)	((n - 1) & 0x1F)
#define LPC32XX_CLKPWR_LCDCTRL_PSCALE_MSK	0x001F

/*
 * clkpwr_hclkpll_ctrl register definitions
 */
#define LPC32XX_CLKPWR_HCLKPLL_POWER_UP		_BIT(16)
#define LPC32XX_CLKPWR_HCLKPLL_CCO_BYPASS	_BIT(15)
#define LPC32XX_CLKPWR_HCLKPLL_POSTDIV_BYPASS	_BIT(14)
#define LPC32XX_CLKPWR_HCLKPLL_FDBK_SEL_FCLK	_BIT(13)
#define LPC32XX_CLKPWR_HCLKPLL_POSTDIV_2POW(n)	(((n) & 0x3) << 11)
#define LPC32XX_CLKPWR_HCLKPLL_PREDIV_PLUS1(n)	(((n) & 0x3) << 9)
#define LPC32XX_CLKPWR_HCLKPLL_PLLM(n)		(((n) & 0xFF) << 1)
#define LPC32XX_CLKPWR_HCLKPLL_PLL_STS		_BIT(0)

/*
 * clkpwr_adc_clk_ctrl_1 register definitions
 */
#define LPC32XX_CLKPWR_ADCCTRL1_RTDIV(n)	(((n) & 0xFF) << 0)
#define LPC32XX_CLKPWR_ADCCTRL1_PCLK_SEL	_BIT(8)

/*
 * clkpwr_usb_ctrl register definitions
 */
#define LPC32XX_CLKPWR_USBCTRL_HCLK_EN		_BIT(24)
#define LPC32XX_CLKPWR_USBCTRL_USBI2C_EN	_BIT(23)
#define LPC32XX_CLKPWR_USBCTRL_USBDVND_EN	_BIT(22)
#define LPC32XX_CLKPWR_USBCTRL_USBHSTND_EN	_BIT(21)
#define LPC32XX_CLKPWR_USBCTRL_PU_ADD		(0x0 << 19)
#define LPC32XX_CLKPWR_USBCTRL_BUS_KEEPER	(0x1 << 19)
#define LPC32XX_CLKPWR_USBCTRL_PD_ADD		(0x3 << 19)
#define LPC32XX_CLKPWR_USBCTRL_CLK_EN2		_BIT(18)
#define LPC32XX_CLKPWR_USBCTRL_CLK_EN1		_BIT(17)
#define LPC32XX_CLKPWR_USBCTRL_PLL_PWRUP	_BIT(16)
#define LPC32XX_CLKPWR_USBCTRL_CCO_BYPASS	_BIT(15)
#define LPC32XX_CLKPWR_USBCTRL_POSTDIV_BYPASS	_BIT(14)
#define LPC32XX_CLKPWR_USBCTRL_FDBK_SEL_FCLK	_BIT(13)
#define LPC32XX_CLKPWR_USBCTRL_POSTDIV_2POW(n)	(((n) & 0x3) << 11)
#define LPC32XX_CLKPWR_USBCTRL_PREDIV_PLUS1(n)	(((n) & 0x3) << 9)
#define LPC32XX_CLKPWR_USBCTRL_FDBK_PLUS1(n)	(((n) & 0xFF) << 1)
#define LPC32XX_CLKPWR_USBCTRL_PLL_STS		_BIT(0)

/*
 * clkpwr_sdramclk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_SDRCLK_FASTSLEW_CLK	_BIT(22)
#define LPC32XX_CLKPWR_SDRCLK_FASTSLEW		_BIT(21)
#define LPC32XX_CLKPWR_SDRCLK_FASTSLEW_DAT	_BIT(20)
#define LPC32XX_CLKPWR_SDRCLK_SW_DDR_RESET	_BIT(19)
#define LPC32XX_CLKPWR_SDRCLK_HCLK_DLY(n)	(((n) & 0x1F) << 14)
#define LPC32XX_CLKPWR_SDRCLK_DLY_ADDR_STS	_BIT(13)
#define LPC32XX_CLKPWR_SDRCLK_SENS_FACT(n)	(((n) & 0x7) << 10)
#define LPC32XX_CLKPWR_SDRCLK_USE_CAL		_BIT(9)
#define LPC32XX_CLKPWR_SDRCLK_DO_CAL		_BIT(8)
#define LPC32XX_CLKPWR_SDRCLK_CAL_ON_RTC	_BIT(7)
#define LPC32XX_CLKPWR_SDRCLK_DQS_DLY(n)	(((n) & 0x1F) << 2)
#define LPC32XX_CLKPWR_SDRCLK_USE_DDR		_BIT(1)
#define LPC32XX_CLKPWR_SDRCLK_CLK_DIS		_BIT(0)

/*
 * clkpwr_ssp_blk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_SSPCTRL_DMA_SSP1RX	_BIT(5)
#define LPC32XX_CLKPWR_SSPCTRL_DMA_SSP1TX	_BIT(4)
#define LPC32XX_CLKPWR_SSPCTRL_DMA_SSP0RX	_BIT(3)
#define LPC32XX_CLKPWR_SSPCTRL_DMA_SSP0TX	_BIT(2)
#define LPC32XX_CLKPWR_SSPCTRL_SSPCLK1_EN	_BIT(1)
#define LPC32XX_CLKPWR_SSPCTRL_SSPCLK0_EN	_BIT(0)

/*
 * clkpwr_i2s_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_I2SCTRL_I2S1_RX_FOR_TX	_BIT(6)
#define LPC32XX_CLKPWR_I2SCTRL_I2S1_TX_FOR_RX	_BIT(5)
#define LPC32XX_CLKPWR_I2SCTRL_I2S1_USE_DMA	_BIT(4)
#define LPC32XX_CLKPWR_I2SCTRL_I2S0_RX_FOR_TX	_BIT(3)
#define LPC32XX_CLKPWR_I2SCTRL_I2S0_TX_FOR_RX	_BIT(2)
#define LPC32XX_CLKPWR_I2SCTRL_I2SCLK1_EN	_BIT(1)
#define LPC32XX_CLKPWR_I2SCTRL_I2SCLK0_EN	_BIT(0)

/*
 * clkpwr_ms_ctrl register definitions
 */
#define LPC32XX_CLKPWR_MSCARD_MSDIO_PIN_DIS	_BIT(10)
#define LPC32XX_CLKPWR_MSCARD_MSDIO_PU_EN	_BIT(9)
#define LPC32XX_CLKPWR_MSCARD_MSDIO23_DIS	_BIT(8)
#define LPC32XX_CLKPWR_MSCARD_MSDIO1_DIS	_BIT(7)
#define LPC32XX_CLKPWR_MSCARD_MSDIO0_DIS	_BIT(6)
#define LPC32XX_CLKPWR_MSCARD_SDCARD_EN		_BIT(5)
#define LPC32XX_CLKPWR_MSCARD_SDCARD_DIV(n)	((n) & 0xF)

/*
 * clkpwr_macclk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_MACCTRL_NO_ENET_PIS	0x00
#define LPC32XX_CLKPWR_MACCTRL_USE_MII_PINS	0x08
#define LPC32XX_CLKPWR_MACCTRL_USE_RMII_PINS	0x18
#define LPC32XX_CLKPWR_MACCTRL_PINS_MSK		0x18
#define LPC32XX_CLKPWR_MACCTRL_DMACLK_EN	_BIT(2)
#define LPC32XX_CLKPWR_MACCTRL_MMIOCLK_EN	_BIT(1)
#define LPC32XX_CLKPWR_MACCTRL_HRCCLK_EN	_BIT(0)

/*
 * clkpwr_test_clk_sel register definitions
 */
#define LPC32XX_CLKPWR_TESTCLK1_SEL_PERCLK	(0x0 << 5)
#define LPC32XX_CLKPWR_TESTCLK1_SEL_RTC		(0x1 << 5)
#define LPC32XX_CLKPWR_TESTCLK1_SEL_MOSC	(0x2 << 5)
#define LPC32XX_CLKPWR_TESTCLK1_SEL_MASK	(0x3 << 5)
#define LPC32XX_CLKPWR_TESTCLK_TESTCLK1_EN	_BIT(4)
#define LPC32XX_CLKPWR_TESTCLK2_SEL_HCLK	(0x0 << 1)
#define LPC32XX_CLKPWR_TESTCLK2_SEL_PERCLK	(0x1 << 1)
#define LPC32XX_CLKPWR_TESTCLK2_SEL_USBCLK	(0x2 << 1)
#define LPC32XX_CLKPWR_TESTCLK2_SEL_MOSC	(0x5 << 1)
#define LPC32XX_CLKPWR_TESTCLK2_SEL_PLL397	(0x7 << 1)
#define LPC32XX_CLKPWR_TESTCLK2_SEL_MASK	(0x7 << 1)
#define LPC32XX_CLKPWR_TESTCLK_TESTCLK2_EN	_BIT(0)

/*
 * clkpwr_sw_int register definitions
 */
#define LPC32XX_CLKPWR_SW_INT(n)		(_BIT(0) | (((n) & 0x7F) << 1))
#define LPC32XX_CLKPWR_SW_GET_ARG(n)		(((n) & 0xFE) >> 1)

/*
 * clkpwr_i2c_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_I2CCLK_USBI2CHI_DRIVE	_BIT(4)
#define LPC32XX_CLKPWR_I2CCLK_I2C2HI_DRIVE	_BIT(3)
#define LPC32XX_CLKPWR_I2CCLK_I2C1HI_DRIVE	_BIT(2)
#define LPC32XX_CLKPWR_I2CCLK_I2C2CLK_EN	_BIT(1)
#define LPC32XX_CLKPWR_I2CCLK_I2C1CLK_EN	_BIT(0)

/*
 * clkpwr_key_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_KEYCLKCTRL_CLK_EN	0x1

/*
 * clkpwr_adc_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_ADC32CLKCTRL_CLK_EN	0x1

/*
 * clkpwr_pwm_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_PWMCLK_PWM2_DIV(n)	(((n) & 0xF) << 8)
#define LPC32XX_CLKPWR_PWMCLK_PWM1_DIV(n)	(((n) & 0xF) << 4)
#define LPC32XX_CLKPWR_PWMCLK_PWM2SEL_PCLK	0x8
#define LPC32XX_CLKPWR_PWMCLK_PWM2CLK_EN	0x4
#define LPC32XX_CLKPWR_PWMCLK_PWM1SEL_PCLK	0x2
#define LPC32XX_CLKPWR_PWMCLK_PWM1CLK_EN	0x1

/*
 * clkpwr_timer_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_PWMCLK_HSTIMER_EN	0x2
#define LPC32XX_CLKPWR_PWMCLK_WDOG_EN		0x1

/*
 * clkpwr_timers_pwms_clk_ctrl_1 register definitions
 */
#define LPC32XX_CLKPWR_TMRPWMCLK_TIMER3_EN	0x20
#define LPC32XX_CLKPWR_TMRPWMCLK_TIMER2_EN	0x10
#define LPC32XX_CLKPWR_TMRPWMCLK_TIMER1_EN	0x08
#define LPC32XX_CLKPWR_TMRPWMCLK_TIMER0_EN	0x04
#define LPC32XX_CLKPWR_TMRPWMCLK_PWM4_EN	0x02
#define LPC32XX_CLKPWR_TMRPWMCLK_PWM3_EN	0x01

/*
 * clkpwr_spi_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_SPICLK_SET_SPI2DATIO	0x80
#define LPC32XX_CLKPWR_SPICLK_SET_SPI2CLK	0x40
#define LPC32XX_CLKPWR_SPICLK_USE_SPI2		0x20
#define LPC32XX_CLKPWR_SPICLK_SPI2CLK_EN	0x10
#define LPC32XX_CLKPWR_SPICLK_SET_SPI1DATIO	0x08
#define LPC32XX_CLKPWR_SPICLK_SET_SPI1CLK	0x04
#define LPC32XX_CLKPWR_SPICLK_USE_SPI1		0x02
#define LPC32XX_CLKPWR_SPICLK_SPI1CLK_EN	0x01

/*
 * clkpwr_nand_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_NANDCLK_INTSEL_MLC	0x20
#define LPC32XX_CLKPWR_NANDCLK_DMA_RNB		0x10
#define LPC32XX_CLKPWR_NANDCLK_DMA_INT		0x08
#define LPC32XX_CLKPWR_NANDCLK_SEL_SLC		0x04
#define LPC32XX_CLKPWR_NANDCLK_MLCCLK_EN	0x02
#define LPC32XX_CLKPWR_NANDCLK_SLCCLK_EN	0x01

/*
 * clkpwr_uart3_clk_ctrl, clkpwr_uart4_clk_ctrl, clkpwr_uart5_clk_ctrl
 * and clkpwr_uart6_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_UART_Y_DIV(y)		((y) & 0xFF)
#define LPC32XX_CLKPWR_UART_X_DIV(x)		(((x) & 0xFF) << 8)
#define LPC32XX_CLKPWR_UART_USE_HCLK		_BIT(16)

/*
 * clkpwr_irda_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_IRDA_Y_DIV(y)		((y) & 0xFF)
#define LPC32XX_CLKPWR_IRDA_X_DIV(x)		(((x) & 0xFF) << 8)

/*
 * clkpwr_uart_clk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_UARTCLKCTRL_UART6_EN	_BIT(3)
#define LPC32XX_CLKPWR_UARTCLKCTRL_UART5_EN	_BIT(2)
#define LPC32XX_CLKPWR_UARTCLKCTRL_UART4_EN	_BIT(1)
#define LPC32XX_CLKPWR_UARTCLKCTRL_UART3_EN	_BIT(0)

/*
 * clkpwr_dmaclk_ctrl register definitions
 */
#define LPC32XX_CLKPWR_DMACLKCTRL_CLK_EN	0x1

/*
 * clkpwr_autoclock register definitions
 */
#define LPC32XX_CLKPWR_AUTOCLK_USB_EN		0x40
#define LPC32XX_CLKPWR_AUTOCLK_IRAM_EN		0x02
#define LPC32XX_CLKPWR_AUTOCLK_IROM_EN		0x01

/*
 * Interrupt controller register offsets
 */
#define LPC32XX_INTC_MASK(x)			io_p2v((x) + 0x00)
#define LPC32XX_INTC_RAW_STAT(x)		io_p2v((x) + 0x04)
#define LPC32XX_INTC_STAT(x)			io_p2v((x) + 0x08)
#define LPC32XX_INTC_POLAR(x)			io_p2v((x) + 0x0C)
#define LPC32XX_INTC_ACT_TYPE(x)		io_p2v((x) + 0x10)
#define LPC32XX_INTC_TYPE(x)			io_p2v((x) + 0x14)

/*
 * Timer/counter register offsets
 */
#define LPC32XX_TIMER_IR(x)			io_p2v((x) + 0x00)
#define LPC32XX_TIMER_TCR(x)			io_p2v((x) + 0x04)
#define LPC32XX_TIMER_TC(x)			io_p2v((x) + 0x08)
#define LPC32XX_TIMER_PR(x)			io_p2v((x) + 0x0C)
#define LPC32XX_TIMER_PC(x)			io_p2v((x) + 0x10)
#define LPC32XX_TIMER_MCR(x)			io_p2v((x) + 0x14)
#define LPC32XX_TIMER_MR0(x)			io_p2v((x) + 0x18)
#define LPC32XX_TIMER_MR1(x)			io_p2v((x) + 0x1C)
#define LPC32XX_TIMER_MR2(x)			io_p2v((x) + 0x20)
#define LPC32XX_TIMER_MR3(x)			io_p2v((x) + 0x24)
#define LPC32XX_TIMER_CCR(x)			io_p2v((x) + 0x28)
#define LPC32XX_TIMER_CR0(x)			io_p2v((x) + 0x2C)
#define LPC32XX_TIMER_CR1(x)			io_p2v((x) + 0x30)
#define LPC32XX_TIMER_CR2(x)			io_p2v((x) + 0x34)
#define LPC32XX_TIMER_CR3(x)			io_p2v((x) + 0x38)
#define LPC32XX_TIMER_EMR(x)			io_p2v((x) + 0x3C)
#define LPC32XX_TIMER_CTCR(x)			io_p2v((x) + 0x70)

/*
 * ir register definitions
 */
#define LPC32XX_TIMER_CNTR_MTCH_BIT(n)		(1 << ((n) & 0x3))
#define LPC32XX_TIMER_CNTR_CAPT_BIT(n)		(1 << (4 + ((n) & 0x3)))

/*
 * tcr register definitions
 */
#define LPC32XX_TIMER_CNTR_TCR_EN		0x1
#define LPC32XX_TIMER_CNTR_TCR_RESET		0x2

/*
 * mcr register definitions
 */
#define LPC32XX_TIMER_CNTR_MCR_MTCH(n)		(0x1 << ((n) * 3))
#define LPC32XX_TIMER_CNTR_MCR_RESET(n)		(0x1 << (((n) * 3) + 1))
#define LPC32XX_TIMER_CNTR_MCR_STOP(n)		(0x1 << (((n) * 3) + 2))

/*
 * Standard UART register offsets
 */
#define LPC32XX_UART_DLL_FIFO(x)		io_p2v((x) + 0x00)
#define LPC32XX_UART_DLM_IER(x)			io_p2v((x) + 0x04)
#define LPC32XX_UART_IIR_FCR(x)			io_p2v((x) + 0x08)
#define LPC32XX_UART_LCR(x)			io_p2v((x) + 0x0C)
#define LPC32XX_UART_MODEM_CTRL(x)		io_p2v((x) + 0x10)
#define LPC32XX_UART_LSR(x)			io_p2v((x) + 0x14)
#define LPC32XX_UART_MODEM_STATUS(x)		io_p2v((x) + 0x18)
#define LPC32XX_UART_RXLEV(x)			io_p2v((x) + 0x1C)

/*
 * UART control structure offsets
 */
#define _UCREG(x)				io_p2v(\
						LPC32XX_UART_CTRL_BASE + (x))
#define LPC32XX_UARTCTL_CTRL			_UCREG(0x00)
#define LPC32XX_UARTCTL_CLKMODE			_UCREG(0x04)
#define LPC32XX_UARTCTL_CLOOP			_UCREG(0x08)

/*
 * ctrl register definitions
 */
#define LPC32XX_UART_U3_MD_CTRL_EN		_BIT(11)
#define LPC32XX_UART_IRRX6_INV_EN		_BIT(10)
#define LPC32XX_UART_HDPX_EN			_BIT(9)
#define LPC32XX_UART_UART6_IRDAMOD_BYPASS	_BIT(5)
#define LPC32XX_RT_IRTX6_INV_EN			_BIT(4)
#define LPC32XX_RT_IRTX6_INV_MIR_EN		_BIT(3)
#define LPC32XX_RT_RX_IRPULSE_3_16_115K		_BIT(2)
#define LPC32XX_RT_TX_IRPULSE_3_16_115K		_BIT(1)
#define LPC32XX_UART_U5_ROUTE_TO_USB		_BIT(0)

/*
 * clkmode register definitions
 */
#define LPC32XX_UART_ENABLED_CLOCKS(n)		(((n) >> 16) & 0x7F)
#define LPC32XX_UART_ENABLED_CLOCK(n, u)	(((n) >> (16 + (u))) & 0x1)
#define LPC32XX_UART_ENABLED_CLKS_ANY		_BIT(14)
#define LPC32XX_UART_CLKMODE_OFF		0x0
#define LPC32XX_UART_CLKMODE_ON			0x1
#define LPC32XX_UART_CLKMODE_AUTO		0x2
#define LPC32XX_UART_CLKMODE_MASK(u)		(0x3 << ((((u) - 3) * 2) + 4))
#define LPC32XX_UART_CLKMODE_LOAD(m, u)		((m) << ((((u) - 3) * 2) + 4))

/*
 * GPIO Module Register offsets
 */
#define _GPREG(x)				io_p2v(LPC32XX_GPIO_BASE + (x))
#define LPC32XX_GPIO_P_MUX_SET			_GPREG(0x100)
#define LPC32XX_GPIO_P_MUX_CLR			_GPREG(0x104)
#define LPC32XX_GPIO_P_MUX_STATE		_GPREG(0x108)
#define LPC32XX_GPIO_P3_MUX_SET			_GPREG(0x110)
#define LPC32XX_GPIO_P3_MUX_CLR			_GPREG(0x114)
#define LPC32XX_GPIO_P3_MUX_STATE		_GPREG(0x118)
#define LPC32XX_GPIO_P0_MUX_SET			_GPREG(0x120)
#define LPC32XX_GPIO_P0_MUX_CLR			_GPREG(0x124)
#define LPC32XX_GPIO_P0_MUX_STATE		_GPREG(0x128)
#define LPC32XX_GPIO_P1_MUX_SET			_GPREG(0x130)
#define LPC32XX_GPIO_P1_MUX_CLR			_GPREG(0x134)
#define LPC32XX_GPIO_P1_MUX_STATE		_GPREG(0x138)
#define LPC32XX_GPIO_P2_MUX_SET			_GPREG(0x028)
#define LPC32XX_GPIO_P2_MUX_CLR			_GPREG(0x02C)
#define LPC32XX_GPIO_P2_MUX_STATE		_GPREG(0x030)

#endif
