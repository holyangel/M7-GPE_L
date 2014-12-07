/*
	Copyright (C) 2004 - 2010 Ivo van Doorn <IvDoorn@gmail.com>
	Copyright (C) 2010 Willow Garage <http://www.willowgarage.com>
	Copyright (C) 2009 Alban Browaeys <prahal@yahoo.com>
	Copyright (C) 2009 Felix Fietkau <nbd@openwrt.org>
	Copyright (C) 2009 Luis Correia <luis.f.correia@gmail.com>
	Copyright (C) 2009 Mattias Nissler <mattias.nissler@gmx.de>
	Copyright (C) 2009 Mark Asselstine <asselsm@gmail.com>
	Copyright (C) 2009 Xose Vazquez Perez <xose.vazquez@gmail.com>
	Copyright (C) 2009 Bart Zolnierkiewicz <bzolnier@gmail.com>
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Module: rt2800
	Abstract: Data structures and registers for the rt2800 modules.
	Supported chipsets: RT2800E, RT2800ED & RT2800U.
 */

#ifndef RT2800_H
#define RT2800_H

/*
 * RF chip defines.
 *
 * RF2820 2.4G 2T3R
 * RF2850 2.4G/5G 2T3R
 * RF2720 2.4G 1T2R
 * RF2750 2.4G/5G 1T2R
 * RF3020 2.4G 1T1R
 * RF2020 2.4G B/G
 * RF3021 2.4G 1T2R
 * RF3022 2.4G 2T2R
 * RF3052 2.4G/5G 2T2R
 * RF2853 2.4G/5G 3T3R
 * RF3320 2.4G 1T1R(RT3350/RT3370/RT3390)
 * RF3322 2.4G 2T2R(RT3352/RT3371/RT3372/RT3391/RT3392)
 * RF3053 2.4G/5G 3T3R(RT3883/RT3563/RT3573/RT3593/RT3662)
 * RF5370 2.4G 1T1R
 * RF5390 2.4G 1T1R
 */
#define RF2820				0x0001
#define RF2850				0x0002
#define RF2720				0x0003
#define RF2750				0x0004
#define RF3020				0x0005
#define RF2020				0x0006
#define RF3021				0x0007
#define RF3022				0x0008
#define RF3052				0x0009
#define RF2853				0x000a
#define RF3320				0x000b
#define RF3322				0x000c
#define RF3053				0x000d
#define RF5370				0x5370
#define RF5372				0x5372
#define RF5390				0x5390

/*
 * Chipset revisions.
 */
#define REV_RT2860C			0x0100
#define REV_RT2860D			0x0101
#define REV_RT2872E			0x0200
#define REV_RT3070E			0x0200
#define REV_RT3070F			0x0201
#define REV_RT3071E			0x0211
#define REV_RT3090E			0x0211
#define REV_RT3390E			0x0211
#define REV_RT5390F			0x0502

/*
 * Signal information.
 * Default offset is required for RSSI <-> dBm conversion.
 */
#define DEFAULT_RSSI_OFFSET		120

/*
 * Register layout information.
 */
#define CSR_REG_BASE			0x1000
#define CSR_REG_SIZE			0x0800
#define EEPROM_BASE			0x0000
#define EEPROM_SIZE			0x0110
#define BBP_BASE			0x0000
#define BBP_SIZE			0x0080
#define RF_BASE				0x0004
#define RF_SIZE				0x0010

/*
 * Number of TX queues.
 */
#define NUM_TX_QUEUES			4

/*
 * Registers.
 */

/*
 * E2PROM_CSR: PCI EEPROM control register.
 * RELOAD: Write 1 to reload eeprom content.
 * TYPE: 0: 93c46, 1:93c66.
 * LOAD_STATUS: 1:loading, 0:done.
 */
#define E2PROM_CSR			0x0004
#define E2PROM_CSR_DATA_CLOCK		FIELD32(0x00000001)
#define E2PROM_CSR_CHIP_SELECT		FIELD32(0x00000002)
#define E2PROM_CSR_DATA_IN		FIELD32(0x00000004)
#define E2PROM_CSR_DATA_OUT		FIELD32(0x00000008)
#define E2PROM_CSR_TYPE			FIELD32(0x00000030)
#define E2PROM_CSR_LOAD_STATUS		FIELD32(0x00000040)
#define E2PROM_CSR_RELOAD		FIELD32(0x00000080)

/*
 * AUX_CTRL: Aux/PCI-E related configuration
 */
#define AUX_CTRL			0x10c
#define AUX_CTRL_WAKE_PCIE_EN		FIELD32(0x00000002)
#define AUX_CTRL_FORCE_PCIE_CLK		FIELD32(0x00000400)

/*
 * OPT_14: Unknown register used by rt3xxx devices.
 */
#define OPT_14_CSR			0x0114
#define OPT_14_CSR_BIT0			FIELD32(0x00000001)

/*
 * INT_SOURCE_CSR: Interrupt source register.
 * Write one to clear corresponding bit.
 * TX_FIFO_STATUS: FIFO Statistics is full, sw should read TX_STA_FIFO
 */
#define INT_SOURCE_CSR			0x0200
#define INT_SOURCE_CSR_RXDELAYINT	FIELD32(0x00000001)
#define INT_SOURCE_CSR_TXDELAYINT	FIELD32(0x00000002)
#define INT_SOURCE_CSR_RX_DONE		FIELD32(0x00000004)
#define INT_SOURCE_CSR_AC0_DMA_DONE	FIELD32(0x00000008)
#define INT_SOURCE_CSR_AC1_DMA_DONE	FIELD32(0x00000010)
#define INT_SOURCE_CSR_AC2_DMA_DONE	FIELD32(0x00000020)
#define INT_SOURCE_CSR_AC3_DMA_DONE	FIELD32(0x00000040)
#define INT_SOURCE_CSR_HCCA_DMA_DONE	FIELD32(0x00000080)
#define INT_SOURCE_CSR_MGMT_DMA_DONE	FIELD32(0x00000100)
#define INT_SOURCE_CSR_MCU_COMMAND	FIELD32(0x00000200)
#define INT_SOURCE_CSR_RXTX_COHERENT	FIELD32(0x00000400)
#define INT_SOURCE_CSR_TBTT		FIELD32(0x00000800)
#define INT_SOURCE_CSR_PRE_TBTT		FIELD32(0x00001000)
#define INT_SOURCE_CSR_TX_FIFO_STATUS	FIELD32(0x00002000)
#define INT_SOURCE_CSR_AUTO_WAKEUP	FIELD32(0x00004000)
#define INT_SOURCE_CSR_GPTIMER		FIELD32(0x00008000)
#define INT_SOURCE_CSR_RX_COHERENT	FIELD32(0x00010000)
#define INT_SOURCE_CSR_TX_COHERENT	FIELD32(0x00020000)

/*
 * INT_MASK_CSR: Interrupt MASK register. 1: the interrupt is mask OFF.
 */
#define INT_MASK_CSR			0x0204
#define INT_MASK_CSR_RXDELAYINT		FIELD32(0x00000001)
#define INT_MASK_CSR_TXDELAYINT		FIELD32(0x00000002)
#define INT_MASK_CSR_RX_DONE		FIELD32(0x00000004)
#define INT_MASK_CSR_AC0_DMA_DONE	FIELD32(0x00000008)
#define INT_MASK_CSR_AC1_DMA_DONE	FIELD32(0x00000010)
#define INT_MASK_CSR_AC2_DMA_DONE	FIELD32(0x00000020)
#define INT_MASK_CSR_AC3_DMA_DONE	FIELD32(0x00000040)
#define INT_MASK_CSR_HCCA_DMA_DONE	FIELD32(0x00000080)
#define INT_MASK_CSR_MGMT_DMA_DONE	FIELD32(0x00000100)
#define INT_MASK_CSR_MCU_COMMAND	FIELD32(0x00000200)
#define INT_MASK_CSR_RXTX_COHERENT	FIELD32(0x00000400)
#define INT_MASK_CSR_TBTT		FIELD32(0x00000800)
#define INT_MASK_CSR_PRE_TBTT		FIELD32(0x00001000)
#define INT_MASK_CSR_TX_FIFO_STATUS	FIELD32(0x00002000)
#define INT_MASK_CSR_AUTO_WAKEUP	FIELD32(0x00004000)
#define INT_MASK_CSR_GPTIMER		FIELD32(0x00008000)
#define INT_MASK_CSR_RX_COHERENT	FIELD32(0x00010000)
#define INT_MASK_CSR_TX_COHERENT	FIELD32(0x00020000)

/*
 * WPDMA_GLO_CFG
 */
#define WPDMA_GLO_CFG 			0x0208
#define WPDMA_GLO_CFG_ENABLE_TX_DMA	FIELD32(0x00000001)
#define WPDMA_GLO_CFG_TX_DMA_BUSY    	FIELD32(0x00000002)
#define WPDMA_GLO_CFG_ENABLE_RX_DMA	FIELD32(0x00000004)
#define WPDMA_GLO_CFG_RX_DMA_BUSY	FIELD32(0x00000008)
#define WPDMA_GLO_CFG_WP_DMA_BURST_SIZE	FIELD32(0x00000030)
#define WPDMA_GLO_CFG_TX_WRITEBACK_DONE	FIELD32(0x00000040)
#define WPDMA_GLO_CFG_BIG_ENDIAN	FIELD32(0x00000080)
#define WPDMA_GLO_CFG_RX_HDR_SCATTER	FIELD32(0x0000ff00)
#define WPDMA_GLO_CFG_HDR_SEG_LEN	FIELD32(0xffff0000)

/*
 * WPDMA_RST_IDX
 */
#define WPDMA_RST_IDX 			0x020c
#define WPDMA_RST_IDX_DTX_IDX0		FIELD32(0x00000001)
#define WPDMA_RST_IDX_DTX_IDX1		FIELD32(0x00000002)
#define WPDMA_RST_IDX_DTX_IDX2		FIELD32(0x00000004)
#define WPDMA_RST_IDX_DTX_IDX3		FIELD32(0x00000008)
#define WPDMA_RST_IDX_DTX_IDX4		FIELD32(0x00000010)
#define WPDMA_RST_IDX_DTX_IDX5		FIELD32(0x00000020)
#define WPDMA_RST_IDX_DRX_IDX0		FIELD32(0x00010000)

/*
 * DELAY_INT_CFG
 */
#define DELAY_INT_CFG			0x0210
#define DELAY_INT_CFG_RXMAX_PTIME	FIELD32(0x000000ff)
#define DELAY_INT_CFG_RXMAX_PINT	FIELD32(0x00007f00)
#define DELAY_INT_CFG_RXDLY_INT_EN	FIELD32(0x00008000)
#define DELAY_INT_CFG_TXMAX_PTIME	FIELD32(0x00ff0000)
#define DELAY_INT_CFG_TXMAX_PINT	FIELD32(0x7f000000)
#define DELAY_INT_CFG_TXDLY_INT_EN	FIELD32(0x80000000)

/*
 * WMM_AIFSN_CFG: Aifsn for each EDCA AC
 * AIFSN0: AC_VO
 * AIFSN1: AC_VI
 * AIFSN2: AC_BE
 * AIFSN3: AC_BK
 */
#define WMM_AIFSN_CFG			0x0214
#define WMM_AIFSN_CFG_AIFSN0		FIELD32(0x0000000f)
#define WMM_AIFSN_CFG_AIFSN1		FIELD32(0x000000f0)
#define WMM_AIFSN_CFG_AIFSN2		FIELD32(0x00000f00)
#define WMM_AIFSN_CFG_AIFSN3		FIELD32(0x0000f000)

/*
 * WMM_CWMIN_CSR: CWmin for each EDCA AC
 * CWMIN0: AC_VO
 * CWMIN1: AC_VI
 * CWMIN2: AC_BE
 * CWMIN3: AC_BK
 */
#define WMM_CWMIN_CFG			0x0218
#define WMM_CWMIN_CFG_CWMIN0		FIELD32(0x0000000f)
#define WMM_CWMIN_CFG_CWMIN1		FIELD32(0x000000f0)
#define WMM_CWMIN_CFG_CWMIN2		FIELD32(0x00000f00)
#define WMM_CWMIN_CFG_CWMIN3		FIELD32(0x0000f000)

/*
 * WMM_CWMAX_CSR: CWmax for each EDCA AC
 * CWMAX0: AC_VO
 * CWMAX1: AC_VI
 * CWMAX2: AC_BE
 * CWMAX3: AC_BK
 */
#define WMM_CWMAX_CFG			0x021c
#define WMM_CWMAX_CFG_CWMAX0		FIELD32(0x0000000f)
#define WMM_CWMAX_CFG_CWMAX1		FIELD32(0x000000f0)
#define WMM_CWMAX_CFG_CWMAX2		FIELD32(0x00000f00)
#define WMM_CWMAX_CFG_CWMAX3		FIELD32(0x0000f000)

/*
 * AC_TXOP0: AC_VO/AC_VI TXOP register
 * AC0TXOP: AC_VO in unit of 32us
 * AC1TXOP: AC_VI in unit of 32us
 */
#define WMM_TXOP0_CFG			0x0220
#define WMM_TXOP0_CFG_AC0TXOP		FIELD32(0x0000ffff)
#define WMM_TXOP0_CFG_AC1TXOP		FIELD32(0xffff0000)

/*
 * AC_TXOP1: AC_BE/AC_BK TXOP register
 * AC2TXOP: AC_BE in unit of 32us
 * AC3TXOP: AC_BK in unit of 32us
 */
#define WMM_TXOP1_CFG			0x0224
#define WMM_TXOP1_CFG_AC2TXOP		FIELD32(0x0000ffff)
#define WMM_TXOP1_CFG_AC3TXOP		FIELD32(0xffff0000)

/*
 * GPIO_CTRL_CFG:
 * GPIOD: GPIO direction, 0: Output, 1: Input
 */
#define GPIO_CTRL_CFG			0x0228
#define GPIO_CTRL_CFG_BIT0		FIELD32(0x00000001)
#define GPIO_CTRL_CFG_BIT1		FIELD32(0x00000002)
#define GPIO_CTRL_CFG_BIT2		FIELD32(0x00000004)
#define GPIO_CTRL_CFG_BIT3		FIELD32(0x00000008)
#define GPIO_CTRL_CFG_BIT4		FIELD32(0x00000010)
#define GPIO_CTRL_CFG_BIT5		FIELD32(0x00000020)
#define GPIO_CTRL_CFG_BIT6		FIELD32(0x00000040)
#define GPIO_CTRL_CFG_BIT7		FIELD32(0x00000080)
#define GPIO_CTRL_CFG_GPIOD_BIT0	FIELD32(0x00000100)
#define GPIO_CTRL_CFG_GPIOD_BIT1	FIELD32(0x00000200)
#define GPIO_CTRL_CFG_GPIOD_BIT2	FIELD32(0x00000400)
#define GPIO_CTRL_CFG_GPIOD_BIT3	FIELD32(0x00000800)
#define GPIO_CTRL_CFG_GPIOD_BIT4	FIELD32(0x00001000)
#define GPIO_CTRL_CFG_GPIOD_BIT5	FIELD32(0x00002000)
#define GPIO_CTRL_CFG_GPIOD_BIT6	FIELD32(0x00004000)
#define GPIO_CTRL_CFG_GPIOD_BIT7	FIELD32(0x00008000)

/*
 * MCU_CMD_CFG
 */
#define MCU_CMD_CFG			0x022c

/*
 * AC_VO register offsets
 */
#define TX_BASE_PTR0			0x0230
#define TX_MAX_CNT0			0x0234
#define TX_CTX_IDX0			0x0238
#define TX_DTX_IDX0			0x023c

/*
 * AC_VI register offsets
 */
#define TX_BASE_PTR1			0x0240
#define TX_MAX_CNT1			0x0244
#define TX_CTX_IDX1			0x0248
#define TX_DTX_IDX1			0x024c

/*
 * AC_BE register offsets
 */
#define TX_BASE_PTR2			0x0250
#define TX_MAX_CNT2			0x0254
#define TX_CTX_IDX2			0x0258
#define TX_DTX_IDX2			0x025c

/*
 * AC_BK register offsets
 */
#define TX_BASE_PTR3			0x0260
#define TX_MAX_CNT3			0x0264
#define TX_CTX_IDX3			0x0268
#define TX_DTX_IDX3			0x026c

/*
 * HCCA register offsets
 */
#define TX_BASE_PTR4			0x0270
#define TX_MAX_CNT4			0x0274
#define TX_CTX_IDX4			0x0278
#define TX_DTX_IDX4			0x027c

/*
 * MGMT register offsets
 */
#define TX_BASE_PTR5			0x0280
#define TX_MAX_CNT5			0x0284
#define TX_CTX_IDX5			0x0288
#define TX_DTX_IDX5			0x028c

/*
 * RX register offsets
 */
#define RX_BASE_PTR			0x0290
#define RX_MAX_CNT			0x0294
#define RX_CRX_IDX			0x0298
#define RX_DRX_IDX			0x029c

/*
 * USB_DMA_CFG
 * RX_BULK_AGG_TIMEOUT: Rx Bulk Aggregation TimeOut in unit of 33ns.
 * RX_BULK_AGG_LIMIT: Rx Bulk Aggregation Limit in unit of 256 bytes.
 * PHY_CLEAR: phy watch dog enable.
 * TX_CLEAR: Clear USB DMA TX path.
 * TXOP_HALT: Halt TXOP count down when TX buffer is full.
 * RX_BULK_AGG_EN: Enable Rx Bulk Aggregation.
 * RX_BULK_EN: Enable USB DMA Rx.
 * TX_BULK_EN: Enable USB DMA Tx.
 * EP_OUT_VALID: OUT endpoint data valid.
 * RX_BUSY: USB DMA RX FSM busy.
 * TX_BUSY: USB DMA TX FSM busy.
 */
#define USB_DMA_CFG			0x02a0
#define USB_DMA_CFG_RX_BULK_AGG_TIMEOUT	FIELD32(0x000000ff)
#define USB_DMA_CFG_RX_BULK_AGG_LIMIT	FIELD32(0x0000ff00)
#define USB_DMA_CFG_PHY_CLEAR		FIELD32(0x00010000)
#define USB_DMA_CFG_TX_CLEAR		FIELD32(0x00080000)
#define USB_DMA_CFG_TXOP_HALT		FIELD32(0x00100000)
#define USB_DMA_CFG_RX_BULK_AGG_EN	FIELD32(0x00200000)
#define USB_DMA_CFG_RX_BULK_EN		FIELD32(0x00400000)
#define USB_DMA_CFG_TX_BULK_EN		FIELD32(0x00800000)
#define USB_DMA_CFG_EP_OUT_VALID	FIELD32(0x3f000000)
#define USB_DMA_CFG_RX_BUSY		FIELD32(0x40000000)
#define USB_DMA_CFG_TX_BUSY		FIELD32(0x80000000)

/*
 * US_CYC_CNT
 * BT_MODE_EN: Bluetooth mode enable
 * CLOCK CYCLE: Clock cycle count in 1us.
 * PCI:0x21, PCIE:0x7d, USB:0x1e
 */
#define US_CYC_CNT			0x02a4
#define US_CYC_CNT_BT_MODE_EN		FIELD32(0x00000100)
#define US_CYC_CNT_CLOCK_CYCLE		FIELD32(0x000000ff)

/*
 * PBF_SYS_CTRL
 * HOST_RAM_WRITE: enable Host program ram write selection
 */
#define PBF_SYS_CTRL			0x0400
#define PBF_SYS_CTRL_READY		FIELD32(0x00000080)
#define PBF_SYS_CTRL_HOST_RAM_WRITE	FIELD32(0x00010000)

/*
 * HOST-MCU shared memory
 */
#define HOST_CMD_CSR			0x0404
#define HOST_CMD_CSR_HOST_COMMAND	FIELD32(0x000000ff)

/*
 * PBF registers
 * Most are for debug. Driver doesn't touch PBF register.
 */
#define PBF_CFG				0x0408
#define PBF_MAX_PCNT			0x040c
#define PBF_CTRL			0x0410
#define PBF_INT_STA			0x0414
#define PBF_INT_ENA			0x0418

/*
 * BCN_OFFSET0:
 */
#define BCN_OFFSET0			0x042c
#define BCN_OFFSET0_BCN0		FIELD32(0x000000ff)
#define BCN_OFFSET0_BCN1		FIELD32(0x0000ff00)
#define BCN_OFFSET0_BCN2		FIELD32(0x00ff0000)
#define BCN_OFFSET0_BCN3		FIELD32(0xff000000)

/*
 * BCN_OFFSET1:
 */
#define BCN_OFFSET1			0x0430
#define BCN_OFFSET1_BCN4		FIELD32(0x000000ff)
#define BCN_OFFSET1_BCN5		FIELD32(0x0000ff00)
#define BCN_OFFSET1_BCN6		FIELD32(0x00ff0000)
#define BCN_OFFSET1_BCN7		FIELD32(0xff000000)

/*
 * TXRXQ_PCNT: PBF register
 * PCNT_TX0Q: Page count for TX hardware queue 0
 * PCNT_TX1Q: Page count for TX hardware queue 1
 * PCNT_TX2Q: Page count for TX hardware queue 2
 * PCNT_RX0Q: Page count for RX hardware queue
 */
#define TXRXQ_PCNT			0x0438
#define TXRXQ_PCNT_TX0Q			FIELD32(0x000000ff)
#define TXRXQ_PCNT_TX1Q			FIELD32(0x0000ff00)
#define TXRXQ_PCNT_TX2Q			FIELD32(0x00ff0000)
#define TXRXQ_PCNT_RX0Q			FIELD32(0xff000000)

/*
 * PBF register
 * Debug. Driver doesn't touch PBF register.
 */
#define PBF_DBG				0x043c

/*
 * RF registers
 */
#define	RF_CSR_CFG			0x0500
#define RF_CSR_CFG_DATA			FIELD32(0x000000ff)
#define RF_CSR_CFG_REGNUM		FIELD32(0x00003f00)
#define RF_CSR_CFG_WRITE		FIELD32(0x00010000)
#define RF_CSR_CFG_BUSY			FIELD32(0x00020000)

/*
 * EFUSE_CSR: RT30x0 EEPROM
 */
#define EFUSE_CTRL			0x0580
#define EFUSE_CTRL_ADDRESS_IN		FIELD32(0x03fe0000)
#define EFUSE_CTRL_MODE			FIELD32(0x000000c0)
#define EFUSE_CTRL_KICK			FIELD32(0x40000000)
#define EFUSE_CTRL_PRESENT		FIELD32(0x80000000)

/*
 * EFUSE_DATA0
 */
#define EFUSE_DATA0			0x0590

/*
 * EFUSE_DATA1
 */
#define EFUSE_DATA1			0x0594

/*
 * EFUSE_DATA2
 */
#define EFUSE_DATA2			0x0598

/*
 * EFUSE_DATA3
 */
#define EFUSE_DATA3			0x059c

/*
 * LDO_CFG0
 */
#define LDO_CFG0			0x05d4
#define LDO_CFG0_DELAY3			FIELD32(0x000000ff)
#define LDO_CFG0_DELAY2			FIELD32(0x0000ff00)
#define LDO_CFG0_DELAY1			FIELD32(0x00ff0000)
#define LDO_CFG0_BGSEL			FIELD32(0x03000000)
#define LDO_CFG0_LDO_CORE_VLEVEL	FIELD32(0x1c000000)
#define LD0_CFG0_LDO25_LEVEL		FIELD32(0x60000000)
#define LDO_CFG0_LDO25_LARGEA		FIELD32(0x80000000)

/*
 * GPIO_SWITCH
 */
#define GPIO_SWITCH			0x05dc
#define GPIO_SWITCH_0			FIELD32(0x00000001)
#define GPIO_SWITCH_1			FIELD32(0x00000002)
#define GPIO_SWITCH_2			FIELD32(0x00000004)
#define GPIO_SWITCH_3			FIELD32(0x00000008)
#define GPIO_SWITCH_4			FIELD32(0x00000010)
#define GPIO_SWITCH_5			FIELD32(0x00000020)
#define GPIO_SWITCH_6			FIELD32(0x00000040)
#define GPIO_SWITCH_7			FIELD32(0x00000080)

/*
 * MAC Control/Status Registers(CSR).
 * Some values are set in TU, whereas 1 TU == 1024 us.
 */

/*
 * MAC_CSR0: ASIC revision number.
 * ASIC_REV: 0
 * ASIC_VER: 2860 or 2870
 */
#define MAC_CSR0			0x1000
#define MAC_CSR0_REVISION		FIELD32(0x0000ffff)
#define MAC_CSR0_CHIPSET		FIELD32(0xffff0000)

/*
 * MAC_SYS_CTRL:
 */
#define MAC_SYS_CTRL			0x1004
#define MAC_SYS_CTRL_RESET_CSR		FIELD32(0x00000001)
#define MAC_SYS_CTRL_RESET_BBP		FIELD32(0x00000002)
#define MAC_SYS_CTRL_ENABLE_TX		FIELD32(0x00000004)
#define MAC_SYS_CTRL_ENABLE_RX		FIELD32(0x00000008)
#define MAC_SYS_CTRL_CONTINUOUS_TX	FIELD32(0x00000010)
#define MAC_SYS_CTRL_LOOPBACK		FIELD32(0x00000020)
#define MAC_SYS_CTRL_WLAN_HALT		FIELD32(0x00000040)
#define MAC_SYS_CTRL_RX_TIMESTAMP	FIELD32(0x00000080)

/*
 * MAC_ADDR_DW0: STA MAC register 0
 */
#define MAC_ADDR_DW0			0x1008
#define MAC_ADDR_DW0_BYTE0		FIELD32(0x000000ff)
#define MAC_ADDR_DW0_BYTE1		FIELD32(0x0000ff00)
#define MAC_ADDR_DW0_BYTE2		FIELD32(0x00ff0000)
#define MAC_ADDR_DW0_BYTE3		FIELD32(0xff000000)

/*
 * MAC_ADDR_DW1: STA MAC register 1
 * UNICAST_TO_ME_MASK:
 * Used to mask off bits from byte 5 of the MAC address
 * to determine the UNICAST_TO_ME bit for RX frames.
 * The full mask is complemented by BSS_ID_MASK:
 *    MASK = BSS_ID_MASK & UNICAST_TO_ME_MASK
 */
#define MAC_ADDR_DW1			0x100c
#define MAC_ADDR_DW1_BYTE4		FIELD32(0x000000ff)
#define MAC_ADDR_DW1_BYTE5		FIELD32(0x0000ff00)
#define MAC_ADDR_DW1_UNICAST_TO_ME_MASK	FIELD32(0x00ff0000)

/*
 * MAC_BSSID_DW0: BSSID register 0
 */
#define MAC_BSSID_DW0			0x1010
#define MAC_BSSID_DW0_BYTE0		FIELD32(0x000000ff)
#define MAC_BSSID_DW0_BYTE1		FIELD32(0x0000ff00)
#define MAC_BSSID_DW0_BYTE2		FIELD32(0x00ff0000)
#define MAC_BSSID_DW0_BYTE3		FIELD32(0xff000000)

/*
 * MAC_BSSID_DW1: BSSID register 1
 * BSS_ID_MASK:
 *     0: 1-BSSID mode (BSS index = 0)
 *     1: 2-BSSID mode (BSS index: Byte5, bit 0)
 *     2: 4-BSSID mode (BSS index: byte5, bit 0 - 1)
 *     3: 8-BSSID mode (BSS index: byte5, bit 0 - 2)
 * This mask is used to mask off bits 0, 1 and 2 of byte 5 of the
 * BSSID. This will make sure that those bits will be ignored
 * when determining the MY_BSS of RX frames.
 */
#define MAC_BSSID_DW1			0x1014
#define MAC_BSSID_DW1_BYTE4		FIELD32(0x000000ff)
#define MAC_BSSID_DW1_BYTE5		FIELD32(0x0000ff00)
#define MAC_BSSID_DW1_BSS_ID_MASK	FIELD32(0x00030000)
#define MAC_BSSID_DW1_BSS_BCN_NUM	FIELD32(0x001c0000)

/*
 * MAX_LEN_CFG: Maximum frame length register.
 * MAX_MPDU: rt2860b max 16k bytes
 * MAX_PSDU: Maximum PSDU length
 *	(power factor) 0:2^13, 1:2^14, 2:2^15, 3:2^16
 */
#define MAX_LEN_CFG			0x1018
#define MAX_LEN_CFG_MAX_MPDU		FIELD32(0x00000fff)
#define MAX_LEN_CFG_MAX_PSDU		FIELD32(0x00003000)
#define MAX_LEN_CFG_MIN_PSDU		FIELD32(0x0000c000)
#define MAX_LEN_CFG_MIN_MPDU		FIELD32(0x000f0000)

/*
 * BBP_CSR_CFG: BBP serial control register
 * VALUE: Register value to program into BBP
 * REG_NUM: Selected BBP register
 * READ_CONTROL: 0 write BBP, 1 read BBP
 * BUSY: ASIC is busy executing BBP commands
 * BBP_PAR_DUR: 0 4 MAC clocks, 1 8 MAC clocks
 * BBP_RW_MODE: 0 serial, 1 parallel
 */
#define BBP_CSR_CFG			0x101c
#define BBP_CSR_CFG_VALUE		FIELD32(0x000000ff)
#define BBP_CSR_CFG_REGNUM		FIELD32(0x0000ff00)
#define BBP_CSR_CFG_READ_CONTROL	FIELD32(0x00010000)
#define BBP_CSR_CFG_BUSY		FIELD32(0x00020000)
#define BBP_CSR_CFG_BBP_PAR_DUR		FIELD32(0x00040000)
#define BBP_CSR_CFG_BBP_RW_MODE		FIELD32(0x00080000)

/*
 * RF_CSR_CFG0: RF control register
 * REGID_AND_VALUE: Register value to program into RF
 * BITWIDTH: Selected RF register
 * STANDBYMODE: 0 high when standby, 1 low when standby
 * SEL: 0 RF_LE0 activate, 1 RF_LE1 activate
 * BUSY: ASIC is busy executing RF commands
 */
#define RF_CSR_CFG0			0x1020
#define RF_CSR_CFG0_REGID_AND_VALUE	FIELD32(0x00ffffff)
#define RF_CSR_CFG0_BITWIDTH		FIELD32(0x1f000000)
#define RF_CSR_CFG0_REG_VALUE_BW	FIELD32(0x1fffffff)
#define RF_CSR_CFG0_STANDBYMODE		FIELD32(0x20000000)
#define RF_CSR_CFG0_SEL			FIELD32(0x40000000)
#define RF_CSR_CFG0_BUSY		FIELD32(0x80000000)

/*
 * RF_CSR_CFG1: RF control register
 * REGID_AND_VALUE: Register value to program into RF
 * RFGAP: Gap between BB_CONTROL_RF and RF_LE
 *        0: 3 system clock cycle (37.5usec)
 *        1: 5 system clock cycle (62.5usec)
 */
#define RF_CSR_CFG1			0x1024
#define RF_CSR_CFG1_REGID_AND_VALUE	FIELD32(0x00ffffff)
#define RF_CSR_CFG1_RFGAP		FIELD32(0x1f000000)

/*
 * RF_CSR_CFG2: RF control register
 * VALUE: Register value to program into RF
 */
#define RF_CSR_CFG2			0x1028
#define RF_CSR_CFG2_VALUE		FIELD32(0x00ffffff)

/*
 * LED_CFG: LED control
 * ON_PERIOD: LED active time (ms) during TX (only used for LED mode 1)
 * OFF_PERIOD: LED inactive time (ms) during TX (only used for LED mode 1)
 * SLOW_BLINK_PERIOD: LED blink interval in seconds (only used for LED mode 2)
 * color LED's:
 *   0: off
 *   1: blinking upon TX2
 *   2: periodic slow blinking
 *   3: always on
 * LED polarity:
 *   0: active low
 *   1: active high
 */
#define LED_CFG				0x102c
#define LED_CFG_ON_PERIOD		FIELD32(0x000000ff)
#define LED_CFG_OFF_PERIOD		FIELD32(0x0000ff00)
#define LED_CFG_SLOW_BLINK_PERIOD	FIELD32(0x003f0000)
#define LED_CFG_R_LED_MODE		FIELD32(0x03000000)
#define LED_CFG_G_LED_MODE		FIELD32(0x0c000000)
#define LED_CFG_Y_LED_MODE		FIELD32(0x30000000)
#define LED_CFG_LED_POLAR		FIELD32(0x40000000)

/*
 * AMPDU_BA_WINSIZE: Force BlockAck window size
 * FORCE_WINSIZE_ENABLE:
 *   0: Disable forcing of BlockAck window size
 *   1: Enable forcing of BlockAck window size, overwrites values BlockAck
 *      window size values in the TXWI
 * FORCE_WINSIZE: BlockAck window size
 */
#define AMPDU_BA_WINSIZE		0x1040
#define AMPDU_BA_WINSIZE_FORCE_WINSIZE_ENABLE FIELD32(0x00000020)
#define AMPDU_BA_WINSIZE_FORCE_WINSIZE	FIELD32(0x0000001f)

/*
 * XIFS_TIME_CFG: MAC timing
 * CCKM_SIFS_TIME: unit 1us. Applied after CCK RX/TX
 * OFDM_SIFS_TIME: unit 1us. Applied after OFDM RX/TX
 * OFDM_XIFS_TIME: unit 1us. Applied after OFDM RX
 *	when MAC doesn't reference BBP signal BBRXEND
 * EIFS: unit 1us
 * BB_RXEND_ENABLE: reference RXEND signal to begin XIFS defer
 *
 */
#define XIFS_TIME_CFG			0x1100
#define XIFS_TIME_CFG_CCKM_SIFS_TIME	FIELD32(0x000000ff)
#define XIFS_TIME_CFG_OFDM_SIFS_TIME	FIELD32(0x0000ff00)
#define XIFS_TIME_CFG_OFDM_XIFS_TIME	FIELD32(0x000f0000)
#define XIFS_TIME_CFG_EIFS		FIELD32(0x1ff00000)
#define XIFS_TIME_CFG_BB_RXEND_ENABLE	FIELD32(0x20000000)

/*
 * BKOFF_SLOT_CFG:
 */
#define BKOFF_SLOT_CFG			0x1104
#define BKOFF_SLOT_CFG_SLOT_TIME	FIELD32(0x000000ff)
#define BKOFF_SLOT_CFG_CC_DELAY_TIME	FIELD32(0x0000ff00)

/*
 * NAV_TIME_CFG:
 */
#define NAV_TIME_CFG			0x1108
#define NAV_TIME_CFG_SIFS		FIELD32(0x000000ff)
#define NAV_TIME_CFG_SLOT_TIME		FIELD32(0x0000ff00)
#define NAV_TIME_CFG_EIFS		FIELD32(0x01ff0000)
#define NAV_TIME_ZERO_SIFS		FIELD32(0x02000000)

/*
 * CH_TIME_CFG: count as channel busy
 * EIFS_BUSY: Count EIFS as channel busy
 * NAV_BUSY: Count NAS as channel busy
 * RX_BUSY: Count RX as channel busy
 * TX_BUSY: Count TX as channel busy
 * TMR_EN: Enable channel statistics timer
 */
#define CH_TIME_CFG     	        0x110c
#define CH_TIME_CFG_EIFS_BUSY		FIELD32(0x00000010)
#define CH_TIME_CFG_NAV_BUSY		FIELD32(0x00000008)
#define CH_TIME_CFG_RX_BUSY		FIELD32(0x00000004)
#define CH_TIME_CFG_TX_BUSY		FIELD32(0x00000002)
#define CH_TIME_CFG_TMR_EN		FIELD32(0x00000001)

/*
 * PBF_LIFE_TIMER: TX/RX MPDU timestamp timer (free run) Unit: 1us
 */
#define PBF_LIFE_TIMER     	        0x1110

/*
 * BCN_TIME_CFG:
 * BEACON_INTERVAL: in unit of 1/16 TU
 * TSF_TICKING: Enable TSF auto counting
 * TSF_SYNC: Enable TSF sync, 00: disable, 01: infra mode, 10: ad-hoc mode
 * BEACON_GEN: Enable beacon generator
 */
#define BCN_TIME_CFG			0x1114
#define BCN_TIME_CFG_BEACON_INTERVAL	FIELD32(0x0000ffff)
#define BCN_TIME_CFG_TSF_TICKING	FIELD32(0x00010000)
#define BCN_TIME_CFG_TSF_SYNC		FIELD32(0x00060000)
#define BCN_TIME_CFG_TBTT_ENABLE	FIELD32(0x00080000)
#define BCN_TIME_CFG_BEACON_GEN		FIELD32(0x00100000)
#define BCN_TIME_CFG_TX_TIME_COMPENSATE	FIELD32(0xf0000000)

/*
 * TBTT_SYNC_CFG:
 * BCN_AIFSN: Beacon AIFSN after TBTT interrupt in slots
 * BCN_CWMIN: Beacon CWMin after TBTT interrupt in slots
 */
#define TBTT_SYNC_CFG			0x1118
#define TBTT_SYNC_CFG_TBTT_ADJUST	FIELD32(0x000000ff)
#define TBTT_SYNC_CFG_BCN_EXP_WIN	FIELD32(0x0000ff00)
#define TBTT_SYNC_CFG_BCN_AIFSN		FIELD32(0x000f0000)
#define TBTT_SYNC_CFG_BCN_CWMIN		FIELD32(0x00f00000)

/*
 * TSF_TIMER_DW0: Local lsb TSF timer, read-only
 */
#define TSF_TIMER_DW0			0x111c
#define TSF_TIMER_DW0_LOW_WORD		FIELD32(0xffffffff)

/*
 * TSF_TIMER_DW1: Local msb TSF timer, read-only
 */
#define TSF_TIMER_DW1			0x1120
#define TSF_TIMER_DW1_HIGH_WORD		FIELD32(0xffffffff)

/*
 * TBTT_TIMER: TImer remains till next TBTT, read-only
 */
#define TBTT_TIMER			0x1124

/*
 * INT_TIMER_CFG: timer configuration
 * PRE_TBTT_TIMER: leadtime to tbtt for pretbtt interrupt in units of 1/16 TU
 * GP_TIMER: period of general purpose timer in units of 1/16 TU
 */
#define INT_TIMER_CFG			0x1128
#define INT_TIMER_CFG_PRE_TBTT_TIMER	FIELD32(0x0000ffff)
#define INT_TIMER_CFG_GP_TIMER		FIELD32(0xffff0000)

/*
 * INT_TIMER_EN: GP-timer and pre-tbtt Int enable
 */
#define INT_TIMER_EN			0x112c
#define INT_TIMER_EN_PRE_TBTT_TIMER	FIELD32(0x00000001)
#define INT_TIMER_EN_GP_TIMER		FIELD32(0x00000002)

/*
 * CH_IDLE_STA: channel idle time (in us)
 */
#define CH_IDLE_STA			0x1130

/*
 * CH_BUSY_STA: channel busy time on primary channel (in us)
 */
#define CH_BUSY_STA			0x1134

/*
 * CH_BUSY_STA_SEC: channel busy time on secondary channel in HT40 mode (in us)
 */
#define CH_BUSY_STA_SEC			0x1138

/*
 * MAC_STATUS_CFG:
 * BBP_RF_BUSY: When set to 0, BBP and RF are stable.
 *	if 1 or higher one of the 2 registers is busy.
 */
#define MAC_STATUS_CFG			0x1200
#define MAC_STATUS_CFG_BBP_RF_BUSY	FIELD32(0x00000003)

/*
 * PWR_PIN_CFG:
 */
#define PWR_PIN_CFG			0x1204

/*
 * AUTOWAKEUP_CFG: Manual power control / status register
 * TBCN_BEFORE_WAKE: ForceWake has high privilege than PutToSleep when both set
 * AUTOWAKE: 0:sleep, 1:awake
 */
#define AUTOWAKEUP_CFG			0x1208
#define AUTOWAKEUP_CFG_AUTO_LEAD_TIME	FIELD32(0x000000ff)
#define AUTOWAKEUP_CFG_TBCN_BEFORE_WAKE	FIELD32(0x00007f00)
#define AUTOWAKEUP_CFG_AUTOWAKE		FIELD32(0x00008000)

/*
 * EDCA_AC0_CFG:
 */
#define EDCA_AC0_CFG			0x1300
#define EDCA_AC0_CFG_TX_OP		FIELD32(0x000000ff)
#define EDCA_AC0_CFG_AIFSN		FIELD32(0x00000f00)
#define EDCA_AC0_CFG_CWMIN		FIELD32(0x0000f000)
#define EDCA_AC0_CFG_CWMAX		FIELD32(0x000f0000)

/*
 * EDCA_AC1_CFG:
 */
#define EDCA_AC1_CFG			0x1304
#define EDCA_AC1_CFG_TX_OP		FIELD32(0x000000ff)
#define EDCA_AC1_CFG_AIFSN		FIELD32(0x00000f00)
#define EDCA_AC1_CFG_CWMIN		FIELD32(0x0000f000)
#define EDCA_AC1_CFG_CWMAX		FIELD32(0x000f0000)

/*
 * EDCA_AC2_CFG:
 */
#define EDCA_AC2_CFG			0x1308
#define EDCA_AC2_CFG_TX_OP		FIELD32(0x000000ff)
#define EDCA_AC2_CFG_AIFSN		FIELD32(0x00000f00)
#define EDCA_AC2_CFG_CWMIN		FIELD32(0x0000f000)
#define EDCA_AC2_CFG_CWMAX		FIELD32(0x000f0000)

/*
 * EDCA_AC3_CFG:
 */
#define EDCA_AC3_CFG			0x130c
#define EDCA_AC3_CFG_TX_OP		FIELD32(0x000000ff)
#define EDCA_AC3_CFG_AIFSN		FIELD32(0x00000f00)
#define EDCA_AC3_CFG_CWMIN		FIELD32(0x0000f000)
#define EDCA_AC3_CFG_CWMAX		FIELD32(0x000f0000)

/*
 * EDCA_TID_AC_MAP:
 */
#define EDCA_TID_AC_MAP			0x1310

/*
 * TX_PWR_CFG:
 */
#define TX_PWR_CFG_RATE0		FIELD32(0x0000000f)
#define TX_PWR_CFG_RATE1		FIELD32(0x000000f0)
#define TX_PWR_CFG_RATE2		FIELD32(0x00000f00)
#define TX_PWR_CFG_RATE3		FIELD32(0x0000f000)
#define TX_PWR_CFG_RATE4		FIELD32(0x000f0000)
#define TX_PWR_CFG_RATE5		FIELD32(0x00f00000)
#define TX_PWR_CFG_RATE6		FIELD32(0x0f000000)
#define TX_PWR_CFG_RATE7		FIELD32(0xf0000000)

/*
 * TX_PWR_CFG_0:
 */
#define TX_PWR_CFG_0			0x1314
#define TX_PWR_CFG_0_1MBS		FIELD32(0x0000000f)
#define TX_PWR_CFG_0_2MBS		FIELD32(0x000000f0)
#define TX_PWR_CFG_0_55MBS		FIELD32(0x00000f00)
#define TX_PWR_CFG_0_11MBS		FIELD32(0x0000f000)
#define TX_PWR_CFG_0_6MBS		FIELD32(0x000f0000)
#define TX_PWR_CFG_0_9MBS		FIELD32(0x00f00000)
#define TX_PWR_CFG_0_12MBS		FIELD32(0x0f000000)
#define TX_PWR_CFG_0_18MBS		FIELD32(0xf0000000)

/*
 * TX_PWR_CFG_1:
 */
#define TX_PWR_CFG_1			0x1318
#define TX_PWR_CFG_1_24MBS		FIELD32(0x0000000f)
#define TX_PWR_CFG_1_36MBS		FIELD32(0x000000f0)
#define TX_PWR_CFG_1_48MBS		FIELD32(0x00000f00)
#define TX_PWR_CFG_1_54MBS		FIELD32(0x0000f000)
#define TX_PWR_CFG_1_MCS0		FIELD32(0x000f0000)
#define TX_PWR_CFG_1_MCS1		FIELD32(0x00f00000)
#define TX_PWR_CFG_1_MCS2		FIELD32(0x0f000000)
#define TX_PWR_CFG_1_MCS3		FIELD32(0xf0000000)

/*
 * TX_PWR_CFG_2:
 */
#define TX_PWR_CFG_2			0x131c
#define TX_PWR_CFG_2_MCS4		FIELD32(0x0000000f)
#define TX_PWR_CFG_2_MCS5		FIELD32(0x000000f0)
#define TX_PWR_CFG_2_MCS6		FIELD32(0x00000f00)
#define TX_PWR_CFG_2_MCS7		FIELD32(0x0000f000)
#define TX_PWR_CFG_2_MCS8		FIELD32(0x000f0000)
#define TX_PWR_CFG_2_MCS9		FIELD32(0x00f00000)
#define TX_PWR_CFG_2_MCS10		FIELD32(0x0f000000)
#define TX_PWR_CFG_2_MCS11		FIELD32(0xf0000000)

/*
 * TX_PWR_CFG_3:
 */
#define TX_PWR_CFG_3			0x1320
#define TX_PWR_CFG_3_MCS12		FIELD32(0x0000000f)
#define TX_PWR_CFG_3_MCS13		FIELD32(0x000000f0)
#define TX_PWR_CFG_3_MCS14		FIELD32(0x00000f00)
#define TX_PWR_CFG_3_MCS15		FIELD32(0x0000f000)
#define TX_PWR_CFG_3_UKNOWN1		FIELD32(0x000f0000)
#define TX_PWR_CFG_3_UKNOWN2		FIELD32(0x00f00000)
#define TX_PWR_CFG_3_UKNOWN3		FIELD32(0x0f000000)
#define TX_PWR_CFG_3_UKNOWN4		FIELD32(0xf0000000)

/*
 * TX_PWR_CFG_4:
 */
#define TX_PWR_CFG_4			0x1324
#define TX_PWR_CFG_4_UKNOWN5		FIELD32(0x0000000f)
#define TX_PWR_CFG_4_UKNOWN6		FIELD32(0x000000f0)
#define TX_PWR_CFG_4_UKNOWN7		FIELD32(0x00000f00)
#define TX_PWR_CFG_4_UKNOWN8		FIELD32(0x0000f000)

/*
 * TX_PIN_CFG:
 */
#define TX_PIN_CFG			0x1328
#define TX_PIN_CFG_PA_PE_DISABLE	0xfcfffff0
#define TX_PIN_CFG_PA_PE_A0_EN		FIELD32(0x00000001)
#define TX_PIN_CFG_PA_PE_G0_EN		FIELD32(0x00000002)
#define TX_PIN_CFG_PA_PE_A1_EN		FIELD32(0x00000004)
#define TX_PIN_CFG_PA_PE_G1_EN		FIELD32(0x00000008)
#define TX_PIN_CFG_PA_PE_A0_POL		FIELD32(0x00000010)
#define TX_PIN_CFG_PA_PE_G0_POL		FIELD32(0x00000020)
#define TX_PIN_CFG_PA_PE_A1_POL		FIELD32(0x00000040)
#define TX_PIN_CFG_PA_PE_G1_POL		FIELD32(0x00000080)
#define TX_PIN_CFG_LNA_PE_A0_EN		FIELD32(0x00000100)
#define TX_PIN_CFG_LNA_PE_G0_EN		FIELD32(0x00000200)
#define TX_PIN_CFG_LNA_PE_A1_EN		FIELD32(0x00000400)
#define TX_PIN_CFG_LNA_PE_G1_EN		FIELD32(0x00000800)
#define TX_PIN_CFG_LNA_PE_A0_POL	FIELD32(0x00001000)
#define TX_PIN_CFG_LNA_PE_G0_POL	FIELD32(0x00002000)
#define TX_PIN_CFG_LNA_PE_A1_POL	FIELD32(0x00004000)
#define TX_PIN_CFG_LNA_PE_G1_POL	FIELD32(0x00008000)
#define TX_PIN_CFG_RFTR_EN		FIELD32(0x00010000)
#define TX_PIN_CFG_RFTR_POL		FIELD32(0x00020000)
#define TX_PIN_CFG_TRSW_EN		FIELD32(0x00040000)
#define TX_PIN_CFG_TRSW_POL		FIELD32(0x00080000)
#define TX_PIN_CFG_PA_PE_A2_EN		FIELD32(0x01000000)
#define TX_PIN_CFG_PA_PE_G2_EN		FIELD32(0x02000000)
#define TX_PIN_CFG_PA_PE_A2_POL		FIELD32(0x04000000)
#define TX_PIN_CFG_PA_PE_G2_POL		FIELD32(0x08000000)
#define TX_PIN_CFG_LNA_PE_A2_EN		FIELD32(0x10000000)
#define TX_PIN_CFG_LNA_PE_G2_EN		FIELD32(0x20000000)
#define TX_PIN_CFG_LNA_PE_A2_POL	FIELD32(0x40000000)
#define TX_PIN_CFG_LNA_PE_G2_POL	FIELD32(0x80000000)

/*
 * TX_BAND_CFG: 0x1 use upper 20MHz, 0x0 use lower 20MHz
 */
#define TX_BAND_CFG			0x132c
#define TX_BAND_CFG_HT40_MINUS		FIELD32(0x00000001)
#define TX_BAND_CFG_A			FIELD32(0x00000002)
#define TX_BAND_CFG_BG			FIELD32(0x00000004)

/*
 * TX_SW_CFG0:
 */
#define TX_SW_CFG0			0x1330

/*
 * TX_SW_CFG1:
 */
#define TX_SW_CFG1			0x1334

/*
 * TX_SW_CFG2:
 */
#define TX_SW_CFG2			0x1338

/*
 * TXOP_THRES_CFG:
 */
#define TXOP_THRES_CFG			0x133c

/*
 * TXOP_CTRL_CFG:
 * TIMEOUT_TRUN_EN: Enable/Disable TXOP timeout truncation
 * AC_TRUN_EN: Enable/Disable truncation for AC change
 * TXRATEGRP_TRUN_EN: Enable/Disable truncation for TX rate group change
 * USER_MODE_TRUN_EN: Enable/Disable truncation for user TXOP mode
 * MIMO_PS_TRUN_EN: Enable/Disable truncation for MIMO PS RTS/CTS
 * RESERVED_TRUN_EN: Reserved
 * LSIG_TXOP_EN: Enable/Disable L-SIG TXOP protection
 * EXT_CCA_EN: Enable/Disable extension channel CCA reference (Defer 40Mhz
 *	       transmissions if extension CCA is clear).
 * EXT_CCA_DLY: Extension CCA signal delay time (unit: us)
 * EXT_CWMIN: CwMin for extension channel backoff
 *	      0: Disabled
 *
 */
#define TXOP_CTRL_CFG			0x1340
#define TXOP_CTRL_CFG_TIMEOUT_TRUN_EN	FIELD32(0x00000001)
#define TXOP_CTRL_CFG_AC_TRUN_EN	FIELD32(0x00000002)
#define TXOP_CTRL_CFG_TXRATEGRP_TRUN_EN	FIELD32(0x00000004)
#define TXOP_CTRL_CFG_USER_MODE_TRUN_EN	FIELD32(0x00000008)
#define TXOP_CTRL_CFG_MIMO_PS_TRUN_EN	FIELD32(0x00000010)
#define TXOP_CTRL_CFG_RESERVED_TRUN_EN	FIELD32(0x00000020)
#define TXOP_CTRL_CFG_LSIG_TXOP_EN	FIELD32(0x00000040)
#define TXOP_CTRL_CFG_EXT_CCA_EN	FIELD32(0x00000080)
#define TXOP_CTRL_CFG_EXT_CCA_DLY	FIELD32(0x0000ff00)
#define TXOP_CTRL_CFG_EXT_CWMIN		FIELD32(0x000f0000)

/*
 * TX_RTS_CFG:
 * RTS_THRES: unit:byte
 * RTS_FBK_EN: enable rts rate fallback
 */
#define TX_RTS_CFG			0x1344
#define TX_RTS_CFG_AUTO_RTS_RETRY_LIMIT	FIELD32(0x000000ff)
#define TX_RTS_CFG_RTS_THRES		FIELD32(0x00ffff00)
#define TX_RTS_CFG_RTS_FBK_EN		FIELD32(0x01000000)

/*
 * TX_TIMEOUT_CFG:
 * MPDU_LIFETIME: expiration time = 2^(9+MPDU LIFE TIME) us
 * RX_ACK_TIMEOUT: unit:slot. Used for TX procedure
 * TX_OP_TIMEOUT: TXOP timeout value for TXOP truncation.
 *                it is recommended that:
 *                (SLOT_TIME) > (TX_OP_TIMEOUT) > (RX_ACK_TIMEOUT)
 */
#define TX_TIMEOUT_CFG			0x1348
#define TX_TIMEOUT_CFG_MPDU_LIFETIME	FIELD32(0x000000f0)
#define TX_TIMEOUT_CFG_RX_ACK_TIMEOUT	FIELD32(0x0000ff00)
#define TX_TIMEOUT_CFG_TX_OP_TIMEOUT	FIELD32(0x00ff0000)

/*
 * TX_RTY_CFG:
 * SHORT_RTY_LIMIT: short retry limit
 * LONG_RTY_LIMIT: long retry limit
 * LONG_RTY_THRE: Long retry threshoold
 * NON_AGG_RTY_MODE: Non-Aggregate MPDU retry mode
 *                   0:expired by retry limit, 1: expired by mpdu life timer
 * AGG_RTY_MODE: Aggregate MPDU retry mode
 *               0:expired by retry limit, 1: expired by mpdu life timer
 * TX_AUTO_FB_ENABLE: Tx retry PHY rate auto fallback enable
 */
#define TX_RTY_CFG			0x134c
#define TX_RTY_CFG_SHORT_RTY_LIMIT	FIELD32(0x000000ff)
#define TX_RTY_CFG_LONG_RTY_LIMIT	FIELD32(0x0000ff00)
#define TX_RTY_CFG_LONG_RTY_THRE	FIELD32(0x0fff0000)
#define TX_RTY_CFG_NON_AGG_RTY_MODE	FIELD32(0x10000000)
#define TX_RTY_CFG_AGG_RTY_MODE		FIELD32(0x20000000)
#define TX_RTY_CFG_TX_AUTO_FB_ENABLE	FIELD32(0x40000000)

/*
 * TX_LINK_CFG:
 * REMOTE_MFB_LIFETIME: remote MFB life time. unit: 32us
 * MFB_ENABLE: TX apply remote MFB 1:enable
 * REMOTE_UMFS_ENABLE: remote unsolicit  MFB enable
 *                     0: not apply remote remote unsolicit (MFS=7)
 * TX_MRQ_EN: MCS request TX enable
 * TX_RDG_EN: RDG TX enable
 * TX_CF_ACK_EN: Piggyback CF-ACK enable
 * REMOTE_MFB: remote MCS feedback
 * REMOTE_MFS: remote MCS feedback sequence number
 */
#define TX_LINK_CFG			0x1350
#define TX_LINK_CFG_REMOTE_MFB_LIFETIME	FIELD32(0x000000ff)
#define TX_LINK_CFG_MFB_ENABLE		FIELD32(0x00000100)
#define TX_LINK_CFG_REMOTE_UMFS_ENABLE	FIELD32(0x00000200)
#define TX_LINK_CFG_TX_MRQ_EN		FIELD32(0x00000400)
#define TX_LINK_CFG_TX_RDG_EN		FIELD32(0x00000800)
#define TX_LINK_CFG_TX_CF_ACK_EN	FIELD32(0x00001000)
#define TX_LINK_CFG_REMOTE_MFB		FIELD32(0x00ff0000)
#define TX_LINK_CFG_REMOTE_MFS		FIELD32(0xff000000)

/*
 * HT_FBK_CFG0:
 */
#define HT_FBK_CFG0			0x1354
#define HT_FBK_CFG0_HTMCS0FBK		FIELD32(0x0000000f)
#define HT_FBK_CFG0_HTMCS1FBK		FIELD32(0x000000f0)
#define HT_FBK_CFG0_HTMCS2FBK		FIELD32(0x00000f00)
#define HT_FBK_CFG0_HTMCS3FBK		FIELD32(0x0000f000)
#define HT_FBK_CFG0_HTMCS4FBK		FIELD32(0x000f0000)
#define HT_FBK_CFG0_HTMCS5FBK		FIELD32(0x00f00000)
#define HT_FBK_CFG0_HTMCS6FBK		FIELD32(0x0f000000)
#define HT_FBK_CFG0_HTMCS7FBK		FIELD32(0xf0000000)

/*
 * HT_FBK_CFG1:
 */
#define HT_FBK_CFG1			0x1358
#define HT_FBK_CFG1_HTMCS8FBK		FIELD32(0x0000000f)
#define HT_FBK_CFG1_HTMCS9FBK		FIELD32(0x000000f0)
#define HT_FBK_CFG1_HTMCS10FBK		FIELD32(0x00000f00)
#define HT_FBK_CFG1_HTMCS11FBK		FIELD32(0x0000f000)
#define HT_FBK_CFG1_HTMCS12FBK		FIELD32(0x000f0000)
#define HT_FBK_CFG1_HTMCS13FBK		FIELD32(0x00f00000)
#define HT_FBK_CFG1_HTMCS14FBK		FIELD32(0x0f000000)
#define HT_FBK_CFG1_HTMCS15FBK		FIELD32(0xf0000000)

/*
 * LG_FBK_CFG0:
 */
#define LG_FBK_CFG0			0x135c
#define LG_FBK_CFG0_OFDMMCS0FBK		FIELD32(0x0000000f)
#define LG_FBK_CFG0_OFDMMCS1FBK		FIELD32(0x000000f0)
#define LG_FBK_CFG0_OFDMMCS2FBK		FIELD32(0x00000f00)
#define LG_FBK_CFG0_OFDMMCS3FBK		FIELD32(0x0000f000)
#define LG_FBK_CFG0_OFDMMCS4FBK		FIELD32(0x000f0000)
#define LG_FBK_CFG0_OFDMMCS5FBK		FIELD32(0x00f00000)
#define LG_FBK_CFG0_OFDMMCS6FBK		FIELD32(0x0f000000)
#define LG_FBK_CFG0_OFDMMCS7FBK		FIELD32(0xf0000000)

/*
 * LG_FBK_CFG1:
 */
#define LG_FBK_CFG1			0x1360
#define LG_FBK_CFG0_CCKMCS0FBK		FIELD32(0x0000000f)
#define LG_FBK_CFG0_CCKMCS1FBK		FIELD32(0x000000f0)
#define LG_FBK_CFG0_CCKMCS2FBK		FIELD32(0x00000f00)
#define LG_FBK_CFG0_CCKMCS3FBK		FIELD32(0x0000f000)

/*
 * CCK_PROT_CFG: CCK Protection
 * PROTECT_RATE: Protection control frame rate for CCK TX(RTS/CTS/CFEnd)
 * PROTECT_CTRL: Protection control frame type for CCK TX
 *               0:none, 1:RTS/CTS, 2:CTS-to-self
 * PROTECT_NAV_SHORT: TXOP protection type for CCK TX with short NAV
 * PROTECT_NAV_LONG: TXOP protection type for CCK TX with long NAV
 * TX_OP_ALLOW_CCK: CCK TXOP allowance, 0:disallow
 * TX_OP_ALLOW_OFDM: CCK TXOP allowance, 0:disallow
 * TX_OP_ALLOW_MM20: CCK TXOP allowance, 0:disallow
 * TX_OP_ALLOW_MM40: CCK TXOP allowance, 0:disallow
 * TX_OP_ALLOW_GF20: CCK TXOP allowance, 0:disallow
 * TX_OP_ALLOW_GF40: CCK TXOP allowance, 0:disallow
 * RTS_TH_EN: RTS threshold enable on CCK TX
 */
#define CCK_PROT_CFG			0x1364
#define CCK_PROT_CFG_PROTECT_RATE	FIELD32(0x0000ffff)
#define CCK_PROT_CFG_PROTECT_CTRL	FIELD32(0x00030000)
#define CCK_PROT_CFG_PROTECT_NAV_SHORT	FIELD32(0x00040000)
#define CCK_PROT_CFG_PROTECT_NAV_LONG	FIELD32(0x00080000)
#define CCK_PROT_CFG_TX_OP_ALLOW_CCK	FIELD32(0x00100000)
#define CCK_PROT_CFG_TX_OP_ALLOW_OFDM	FIELD32(0x00200000)
#define CCK_PROT_CFG_TX_OP_ALLOW_MM20	FIELD32(0x00400000)
#define CCK_PROT_CFG_TX_OP_ALLOW_MM40	FIELD32(0x00800000)
#define CCK_PROT_CFG_TX_OP_ALLOW_GF20	FIELD32(0x01000000)
#define CCK_PROT_CFG_TX_OP_ALLOW_GF40	FIELD32(0x02000000)
#define CCK_PROT_CFG_RTS_TH_EN		FIELD32(0x04000000)

/*
 * OFDM_PROT_CFG: OFDM Protection
 */
#define OFDM_PROT_CFG			0x1368
#define OFDM_PROT_CFG_PROTECT_RATE	FIELD32(0x0000ffff)
#define OFDM_PROT_CFG_PROTECT_CTRL	FIELD32(0x00030000)
#define OFDM_PROT_CFG_PROTECT_NAV_SHORT	FIELD32(0x00040000)
#define OFDM_PROT_CFG_PROTECT_NAV_LONG	FIELD32(0x00080000)
#define OFDM_PROT_CFG_TX_OP_ALLOW_CCK	FIELD32(0x00100000)
#define OFDM_PROT_CFG_TX_OP_ALLOW_OFDM	FIELD32(0x00200000)
#define OFDM_PROT_CFG_TX_OP_ALLOW_MM20	FIELD32(0x00400000)
#define OFDM_PROT_CFG_TX_OP_ALLOW_MM40	FIELD32(0x00800000)
#define OFDM_PROT_CFG_TX_OP_ALLOW_GF20	FIELD32(0x01000000)
#define OFDM_PROT_CFG_TX_OP_ALLOW_GF40	FIELD32(0x02000000)
#define OFDM_PROT_CFG_RTS_TH_EN		FIELD32(0x04000000)

/*
 * MM20_PROT_CFG: MM20 Protection
 */
#define MM20_PROT_CFG			0x136c
#define MM20_PROT_CFG_PROTECT_RATE	FIELD32(0x0000ffff)
#define MM20_PROT_CFG_PROTECT_CTRL	FIELD32(0x00030000)
#define MM20_PROT_CFG_PROTECT_NAV_SHORT	FIELD32(0x00040000)
#define MM20_PROT_CFG_PROTECT_NAV_LONG	FIELD32(0x00080000)
#define MM20_PROT_CFG_TX_OP_ALLOW_CCK	FIELD32(0x00100000)
#define MM20_PROT_CFG_TX_OP_ALLOW_OFDM	FIELD32(0x00200000)
#define MM20_PROT_CFG_TX_OP_ALLOW_MM20	FIELD32(0x00400000)
#define MM20_PROT_CFG_TX_OP_ALLOW_MM40	FIELD32(0x00800000)
#define MM20_PROT_CFG_TX_OP_ALLOW_GF20	FIELD32(0x01000000)
#define MM20_PROT_CFG_TX_OP_ALLOW_GF40	FIELD32(0x02000000)
#define MM20_PROT_CFG_RTS_TH_EN		FIELD32(0x04000000)

/*
 * MM40_PROT_CFG: MM40 Protection
 */
#define MM40_PROT_CFG			0x1370
#define MM40_PROT_CFG_PROTECT_RATE	FIELD32(0x0000ffff)
#define MM40_PROT_CFG_PROTECT_CTRL	FIELD32(0x00030000)
#define MM40_PROT_CFG_PROTECT_NAV_SHORT	FIELD32(0x00040000)
#define MM40_PROT_CFG_PROTECT_NAV_LONG	FIELD32(0x00080000)
#define MM40_PROT_CFG_TX_OP_ALLOW_CCK	FIELD32(0x00100000)
#define MM40_PROT_CFG_TX_OP_ALLOW_OFDM	FIELD32(0x00200000)
#define MM40_PROT_CFG_TX_OP_ALLOW_MM20	FIELD32(0x00400000)
#define MM40_PROT_CFG_TX_OP_ALLOW_MM40	FIELD32(0x00800000)
#define MM40_PROT_CFG_TX_OP_ALLOW_GF20	FIELD32(0x01000000)
#define MM40_PROT_CFG_TX_OP_ALLOW_GF40	FIELD32(0x02000000)
#define MM40_PROT_CFG_RTS_TH_EN		FIELD32(0x04000000)

/*
 * GF20_PROT_CFG: GF20 Protection
 */
#define GF20_PROT_CFG			0x1374
#define GF20_PROT_CFG_PROTECT_RATE	FIELD32(0x0000ffff)
#define GF20_PROT_CFG_PROTECT_CTRL	FIELD32(0x00030000)
#define GF20_PROT_CFG_PROTECT_NAV_SHORT	FIELD32(0x00040000)
#define GF20_PROT_CFG_PROTECT_NAV_LONG	FIELD32(0x00080000)
#define GF20_PROT_CFG_TX_OP_ALLOW_CCK	FIELD32(0x00100000)
#define GF20_PROT_CFG_TX_OP_ALLOW_OFDM	FIELD32(0x00200000)
#define GF20_PROT_CFG_TX_OP_ALLOW_MM20	FIELD32(0x00400000)
#define GF20_PROT_CFG_TX_OP_ALLOW_MM40	FIELD32(0x00800000)
#define GF20_PROT_CFG_TX_OP_ALLOW_GF20	FIELD32(0x01000000)
#define GF20_PROT_CFG_TX_OP_ALLOW_GF40	FIELD32(0x02000000)
#define GF20_PROT_CFG_RTS_TH_EN		FIELD32(0x04000000)

/*
 * GF40_PROT_CFG: GF40 Protection
 */
#define GF40_PROT_CFG			0x1378
#define GF40_PROT_CFG_PROTECT_RATE	FIELD32(0x0000ffff)
#define GF40_PROT_CFG_PROTECT_CTRL	FIELD32(0x00030000)
#define GF40_PROT_CFG_PROTECT_NAV_SHORT	FIELD32(0x00040000)
#define GF40_PROT_CFG_PROTECT_NAV_LONG	FIELD32(0x00080000)
#define GF40_PROT_CFG_TX_OP_ALLOW_CCK	FIELD32(0x00100000)
#define GF40_PROT_CFG_TX_OP_ALLOW_OFDM	FIELD32(0x00200000)
#define GF40_PROT_CFG_TX_OP_ALLOW_MM20	FIELD32(0x00400000)
#define GF40_PROT_CFG_TX_OP_ALLOW_MM40	FIELD32(0x00800000)
#define GF40_PROT_CFG_TX_OP_ALLOW_GF20	FIELD32(0x01000000)
#define GF40_PROT_CFG_TX_OP_ALLOW_GF40	FIELD32(0x02000000)
#define GF40_PROT_CFG_RTS_TH_EN		FIELD32(0x04000000)

/*
 * EXP_CTS_TIME:
 */
#define EXP_CTS_TIME			0x137c

/*
 * EXP_ACK_TIME:
 */
#define EXP_ACK_TIME			0x1380

/*
 * RX_FILTER_CFG: RX configuration register.
 */
#define RX_FILTER_CFG			0x1400
#define RX_FILTER_CFG_DROP_CRC_ERROR	FIELD32(0x00000001)
#define RX_FILTER_CFG_DROP_PHY_ERROR	FIELD32(0x00000002)
#define RX_FILTER_CFG_DROP_NOT_TO_ME	FIELD32(0x00000004)
#define RX_FILTER_CFG_DROP_NOT_MY_BSSD	FIELD32(0x00000008)
#define RX_FILTER_CFG_DROP_VER_ERROR	FIELD32(0x00000010)
#define RX_FILTER_CFG_DROP_MULTICAST	FIELD32(0x00000020)
#define RX_FILTER_CFG_DROP_BROADCAST	FIELD32(0x00000040)
#define RX_FILTER_CFG_DROP_DUPLICATE	FIELD32(0x00000080)
#define RX_FILTER_CFG_DROP_CF_END_ACK	FIELD32(0x00000100)
#define RX_FILTER_CFG_DROP_CF_END	FIELD32(0x00000200)
#define RX_FILTER_CFG_DROP_ACK		FIELD32(0x00000400)
#define RX_FILTER_CFG_DROP_CTS		FIELD32(0x00000800)
#define RX_FILTER_CFG_DROP_RTS		FIELD32(0x00001000)
#define RX_FILTER_CFG_DROP_PSPOLL	FIELD32(0x00002000)
#define RX_FILTER_CFG_DROP_BA		FIELD32(0x00004000)
#define RX_FILTER_CFG_DROP_BAR		FIELD32(0x00008000)
#define RX_FILTER_CFG_DROP_CNTL		FIELD32(0x00010000)

/*
 * AUTO_RSP_CFG:
 * AUTORESPONDER: 0: disable, 1: enable
 * BAC_ACK_POLICY: 0:long, 1:short preamble
 * CTS_40_MMODE: Response CTS 40MHz duplicate mode
 * CTS_40_MREF: Response CTS 40MHz duplicate mode
 * AR_PREAMBLE: Auto responder preamble 0:long, 1:short preamble
 * DUAL_CTS_EN: Power bit value in control frame
 * ACK_CTS_PSM_BIT:Power bit value in control frame
 */
#define AUTO_RSP_CFG			0x1404
#define AUTO_RSP_CFG_AUTORESPONDER	FIELD32(0x00000001)
#define AUTO_RSP_CFG_BAC_ACK_POLICY	FIELD32(0x00000002)
#define AUTO_RSP_CFG_CTS_40_MMODE	FIELD32(0x00000004)
#define AUTO_RSP_CFG_CTS_40_MREF	FIELD32(0x00000008)
#define AUTO_RSP_CFG_AR_PREAMBLE	FIELD32(0x00000010)
#define AUTO_RSP_CFG_DUAL_CTS_EN	FIELD32(0x00000040)
#define AUTO_RSP_CFG_ACK_CTS_PSM_BIT	FIELD32(0x00000080)

/*
 * LEGACY_BASIC_RATE:
 */
#define LEGACY_BASIC_RATE		0x1408

/*
 * HT_BASIC_RATE:
 */
#define HT_BASIC_RATE			0x140c

/*
 * HT_CTRL_CFG:
 */
#define HT_CTRL_CFG			0x1410

/*
 * SIFS_COST_CFG:
 */
#define SIFS_COST_CFG			0x1414

/*
 * RX_PARSER_CFG:
 * Set NAV for all received frames
 */
#define RX_PARSER_CFG			0x1418

/*
 * TX_SEC_CNT0:
 */
#define TX_SEC_CNT0			0x1500

/*
 * RX_SEC_CNT0:
 */
#define RX_SEC_CNT0			0x1504

/*
 * CCMP_FC_MUTE:
 */
#define CCMP_FC_MUTE			0x1508

/*
 * TXOP_HLDR_ADDR0:
 */
#define TXOP_HLDR_ADDR0			0x1600

/*
 * TXOP_HLDR_ADDR1:
 */
#define TXOP_HLDR_ADDR1			0x1604

/*
 * TXOP_HLDR_ET:
 */
#define TXOP_HLDR_ET			0x1608

/*
 * QOS_CFPOLL_RA_DW0:
 */
#define QOS_CFPOLL_RA_DW0		0x160c

/*
 * QOS_CFPOLL_RA_DW1:
 */
#define QOS_CFPOLL_RA_DW1		0x1610

/*
 * QOS_CFPOLL_QC:
 */
#define QOS_CFPOLL_QC			0x1614

/*
 * RX_STA_CNT0: RX PLCP error count & RX CRC error count
 */
#define RX_STA_CNT0			0x1700
#define RX_STA_CNT0_CRC_ERR		FIELD32(0x0000ffff)
#define RX_STA_CNT0_PHY_ERR		FIELD32(0xffff0000)

/*
 * RX_STA_CNT1: RX False CCA count & RX LONG frame count
 */
#define RX_STA_CNT1			0x1704
#define RX_STA_CNT1_FALSE_CCA		FIELD32(0x0000ffff)
#define RX_STA_CNT1_PLCP_ERR		FIELD32(0xffff0000)

/*
 * RX_STA_CNT2:
 */
#define RX_STA_CNT2			0x1708
#define RX_STA_CNT2_RX_DUPLI_COUNT	FIELD32(0x0000ffff)
#define RX_STA_CNT2_RX_FIFO_OVERFLOW	FIELD32(0xffff0000)

/*
 * TX_STA_CNT0: TX Beacon count
 */
#define TX_STA_CNT0			0x170c
#define TX_STA_CNT0_TX_FAIL_COUNT	FIELD32(0x0000ffff)
#define TX_STA_CNT0_TX_BEACON_COUNT	FIELD32(0xffff0000)

/*
 * TX_STA_CNT1: TX tx count
 */
#define TX_STA_CNT1			0x1710
#define TX_STA_CNT1_TX_SUCCESS		FIELD32(0x0000ffff)
#define TX_STA_CNT1_TX_RETRANSMIT	FIELD32(0xffff0000)

/*
 * TX_STA_CNT2: TX tx count
 */
#define TX_STA_CNT2			0x1714
#define TX_STA_CNT2_TX_ZERO_LEN_COUNT	FIELD32(0x0000ffff)
#define TX_STA_CNT2_TX_UNDER_FLOW_COUNT	FIELD32(0xffff0000)

/*
 * TX_STA_FIFO: TX Result for specific PID status fifo register.
 *
 * This register is implemented as FIFO with 16 entries in the HW. Each
 * register read fetches the next tx result. If the FIFO is full because
 * it wasn't read fast enough after the according interrupt (TX_FIFO_STATUS)
 * triggered, the hw seems to simply drop further tx results.
 *
 * VALID: 1: this tx result is valid
 *        0: no valid tx result -> driver should stop reading
 * PID_TYPE: The PID latched from the PID field in the TXWI, can be used
 *           to match a frame with its tx result (even though the PID is
 *           only 4 bits wide).
 * PID_QUEUE: Part of PID_TYPE, this is the queue index number (0-3)
 * PID_ENTRY: Part of PID_TYPE, this is the queue entry index number (1-3)
 *            This identification number is calculated by ((idx % 3) + 1).
 * TX_SUCCESS: Indicates tx success (1) or failure (0)
 * TX_AGGRE: Indicates if the frame was part of an aggregate (1) or not (0)
 * TX_ACK_REQUIRED: Indicates if the frame needed to get ack'ed (1) or not (0)
 * WCID: The wireless client ID.
 * MCS: The tx rate used during the last transmission of this frame, be it
 *      successful or not.
 * PHYMODE: The phymode used for the transmission.
 */
#define TX_STA_FIFO			0x1718
#define TX_STA_FIFO_VALID		FIELD32(0x00000001)
#define TX_STA_FIFO_PID_TYPE		FIELD32(0x0000001e)
#define TX_STA_FIFO_PID_QUEUE		FIELD32(0x00000006)
#define TX_STA_FIFO_PID_ENTRY		FIELD32(0x00000018)
#define TX_STA_FIFO_TX_SUCCESS		FIELD32(0x00000020)
#define TX_STA_FIFO_TX_AGGRE		FIELD32(0x00000040)
#define TX_STA_FIFO_TX_ACK_REQUIRED	FIELD32(0x00000080)
#define TX_STA_FIFO_WCID		FIELD32(0x0000ff00)
#define TX_STA_FIFO_SUCCESS_RATE	FIELD32(0xffff0000)
#define TX_STA_FIFO_MCS			FIELD32(0x007f0000)
#define TX_STA_FIFO_PHYMODE		FIELD32(0xc0000000)

/*
 * TX_AGG_CNT: Debug counter
 */
#define TX_AGG_CNT			0x171c
#define TX_AGG_CNT_NON_AGG_TX_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT_AGG_TX_COUNT		FIELD32(0xffff0000)

/*
 * TX_AGG_CNT0:
 */
#define TX_AGG_CNT0			0x1720
#define TX_AGG_CNT0_AGG_SIZE_1_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT0_AGG_SIZE_2_COUNT	FIELD32(0xffff0000)

/*
 * TX_AGG_CNT1:
 */
#define TX_AGG_CNT1			0x1724
#define TX_AGG_CNT1_AGG_SIZE_3_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT1_AGG_SIZE_4_COUNT	FIELD32(0xffff0000)

/*
 * TX_AGG_CNT2:
 */
#define TX_AGG_CNT2			0x1728
#define TX_AGG_CNT2_AGG_SIZE_5_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT2_AGG_SIZE_6_COUNT	FIELD32(0xffff0000)

/*
 * TX_AGG_CNT3:
 */
#define TX_AGG_CNT3			0x172c
#define TX_AGG_CNT3_AGG_SIZE_7_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT3_AGG_SIZE_8_COUNT	FIELD32(0xffff0000)

/*
 * TX_AGG_CNT4:
 */
#define TX_AGG_CNT4			0x1730
#define TX_AGG_CNT4_AGG_SIZE_9_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT4_AGG_SIZE_10_COUNT	FIELD32(0xffff0000)

/*
 * TX_AGG_CNT5:
 */
#define TX_AGG_CNT5			0x1734
#define TX_AGG_CNT5_AGG_SIZE_11_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT5_AGG_SIZE_12_COUNT	FIELD32(0xffff0000)

/*
 * TX_AGG_CNT6:
 */
#define TX_AGG_CNT6			0x1738
#define TX_AGG_CNT6_AGG_SIZE_13_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT6_AGG_SIZE_14_COUNT	FIELD32(0xffff0000)

/*
 * TX_AGG_CNT7:
 */
#define TX_AGG_CNT7			0x173c
#define TX_AGG_CNT7_AGG_SIZE_15_COUNT	FIELD32(0x0000ffff)
#define TX_AGG_CNT7_AGG_SIZE_16_COUNT	FIELD32(0xffff0000)

/*
 * MPDU_DENSITY_CNT:
 * TX_ZERO_DEL: TX zero length delimiter count
 * RX_ZERO_DEL: RX zero length delimiter count
 */
#define MPDU_DENSITY_CNT		0x1740
#define MPDU_DENSITY_CNT_TX_ZERO_DEL	FIELD32(0x0000ffff)
#define MPDU_DENSITY_CNT_RX_ZERO_DEL	FIELD32(0xffff0000)

/*
 * Security key table memory.
 *
 * The pairwise key table shares some memory with the beacon frame
 * buffers 6 and 7. That basically means that when beacon 6 & 7
 * are used we should only use the reduced pairwise key table which
 * has a maximum of 222 entries.
 *
 * ---------------------------------------------
 * |0x4000 | Pairwise Key   | Reduced Pairwise |
 * |       | Table          | Key Table        |
 * |       | Size: 256 * 32 | Size: 222 * 32   |
 * |0x5BC0 |                |-------------------
 * |       |                | Beacon 6         |
 * |0x5DC0 |                |-------------------
 * |       |                | Beacon 7         |
 * |0x5FC0 |                |-------------------
 * |0x5FFF |                |
 * --------------------------
 *
 * MAC_WCID_BASE: 8-bytes (use only 6 bytes) * 256 entry
 * PAIRWISE_KEY_TABLE_BASE: 32-byte * 256 entry
 * MAC_IVEIV_TABLE_BASE: 8-byte * 256-entry
 * MAC_WCID_ATTRIBUTE_BASE: 4-byte * 256-entry
 * SHARED_KEY_TABLE_BASE: 32-byte * 16-entry
 * SHARED_KEY_MODE_BASE: 4-byte * 16-entry
 */
#define MAC_WCID_BASE			0x1800
#define PAIRWISE_KEY_TABLE_BASE		0x4000
#define MAC_IVEIV_TABLE_BASE		0x6000
#define MAC_WCID_ATTRIBUTE_BASE		0x6800
#define SHARED_KEY_TABLE_BASE		0x6c00
#define SHARED_KEY_MODE_BASE		0x7000

#define MAC_WCID_ENTRY(__idx) \
	(MAC_WCID_BASE + ((__idx) * sizeof(struct mac_wcid_entry)))
#define PAIRWISE_KEY_ENTRY(__idx) \
	(PAIRWISE_KEY_TABLE_BASE + ((__idx) * sizeof(struct hw_key_entry)))
#define MAC_IVEIV_ENTRY(__idx) \
	(MAC_IVEIV_TABLE_BASE + ((__idx) * sizeof(struct mac_iveiv_entry)))
#define MAC_WCID_ATTR_ENTRY(__idx) \
	(MAC_WCID_ATTRIBUTE_BASE + ((__idx) * sizeof(u32)))
#define SHARED_KEY_ENTRY(__idx) \
	(SHARED_KEY_TABLE_BASE + ((__idx) * sizeof(struct hw_key_entry)))
#define SHARED_KEY_MODE_ENTRY(__idx) \
	(SHARED_KEY_MODE_BASE + ((__idx) * sizeof(u32)))

struct mac_wcid_entry {
	u8 mac[6];
	u8 reserved[2];
} __packed;

struct hw_key_entry {
	u8 key[16];
	u8 tx_mic[8];
	u8 rx_mic[8];
} __packed;

struct mac_iveiv_entry {
	u8 iv[8];
} __packed;

/*
 * MAC_WCID_ATTRIBUTE:
 */
#define MAC_WCID_ATTRIBUTE_KEYTAB	FIELD32(0x00000001)
#define MAC_WCID_ATTRIBUTE_CIPHER	FIELD32(0x0000000e)
#define MAC_WCID_ATTRIBUTE_BSS_IDX	FIELD32(0x00000070)
#define MAC_WCID_ATTRIBUTE_RX_WIUDF	FIELD32(0x00000380)
#define MAC_WCID_ATTRIBUTE_CIPHER_EXT	FIELD32(0x00000400)
#define MAC_WCID_ATTRIBUTE_BSS_IDX_EXT	FIELD32(0x00000800)
#define MAC_WCID_ATTRIBUTE_WAPI_MCBC	FIELD32(0x00008000)
#define MAC_WCID_ATTRIBUTE_WAPI_KEY_IDX	FIELD32(0xff000000)

/*
 * SHARED_KEY_MODE:
 */
#define SHARED_KEY_MODE_BSS0_KEY0	FIELD32(0x00000007)
#define SHARED_KEY_MODE_BSS0_KEY1	FIELD32(0x00000070)
#define SHARED_KEY_MODE_BSS0_KEY2	FIELD32(0x00000700)
#define SHARED_KEY_MODE_BSS0_KEY3	FIELD32(0x00007000)
#define SHARED_KEY_MODE_BSS1_KEY0	FIELD32(0x00070000)
#define SHARED_KEY_MODE_BSS1_KEY1	FIELD32(0x00700000)
#define SHARED_KEY_MODE_BSS1_KEY2	FIELD32(0x07000000)
#define SHARED_KEY_MODE_BSS1_KEY3	FIELD32(0x70000000)

/*
 * HOST-MCU communication
 */

/*
 * H2M_MAILBOX_CSR: Host-to-MCU Mailbox.
 * CMD_TOKEN: Command id, 0xff disable status reporting.
 */
#define H2M_MAILBOX_CSR			0x7010
#define H2M_MAILBOX_CSR_ARG0		FIELD32(0x000000ff)
#define H2M_MAILBOX_CSR_ARG1		FIELD32(0x0000ff00)
#define H2M_MAILBOX_CSR_CMD_TOKEN	FIELD32(0x00ff0000)
#define H2M_MAILBOX_CSR_OWNER		FIELD32(0xff000000)

/*
 * H2M_MAILBOX_CID:
 * Free slots contain 0xff. MCU will store command's token to lowest free slot.
 * If all slots are occupied status will be dropped.
 */
#define H2M_MAILBOX_CID			0x7014
#define H2M_MAILBOX_CID_CMD0		FIELD32(0x000000ff)
#define H2M_MAILBOX_CID_CMD1		FIELD32(0x0000ff00)
#define H2M_MAILBOX_CID_CMD2		FIELD32(0x00ff0000)
#define H2M_MAILBOX_CID_CMD3		FIELD32(0xff000000)

/*
 * H2M_MAILBOX_STATUS:
 * Command status will be saved to same slot as command id.
 */
#define H2M_MAILBOX_STATUS		0x701c

/*
 * H2M_INT_SRC:
 */
#define H2M_INT_SRC			0x7024

/*
 * H2M_BBP_AGENT:
 */
#define H2M_BBP_AGENT			0x7028

/*
 * MCU_LEDCS: LED control for MCU Mailbox.
 */
#define MCU_LEDCS_LED_MODE		FIELD8(0x1f)
#define MCU_LEDCS_POLARITY		FIELD8(0x01)

/*
 * HW_CS_CTS_BASE:
 * Carrier-sense CTS frame base address.
 * It's where mac stores carrier-sense frame for carrier-sense function.
 */
#define HW_CS_CTS_BASE			0x7700

/*
 * HW_DFS_CTS_BASE:
 * DFS CTS frame base address. It's where mac stores CTS frame for DFS.
 */
#define HW_DFS_CTS_BASE			0x7780

/*
 * TXRX control registers - base address 0x3000
 */

/*
 * TXRX_CSR1:
 * rt2860b  UNKNOWN reg use R/O Reg Addr 0x77d0 first..
 */
#define TXRX_CSR1			0x77d0

/*
 * HW_DEBUG_SETTING_BASE:
 * since NULL frame won't be that long (256 byte)
 * We steal 16 tail bytes to save debugging settings
 */
#define HW_DEBUG_SETTING_BASE		0x77f0
#define HW_DEBUG_SETTING_BASE2		0x7770

/*
 * HW_BEACON_BASE
 * In order to support maximum 8 MBSS and its maximum length
 * is 512 bytes for each beacon
 * Three section discontinue memory segments will be used.
 * 1. The original region for BCN 0~3
 * 2. Extract memory from FCE table for BCN 4~5
 * 3. Extract memory from Pair-wise key table for BCN 6~7
 *    It occupied those memory of wcid 238~253 for BCN 6
 *    and wcid 222~237 for BCN 7 (see Security key table memory
 *    for more info).
 *
 * IMPORTANT NOTE: Not sure why legacy driver does this,
 * but HW_BEACON_BASE7 is 0x0200 bytes below HW_BEACON_BASE6.
 */
#define HW_BEACON_BASE0			0x7800
#define HW_BEACON_BASE1			0x7a00
#define HW_BEACON_BASE2			0x7c00
#define HW_BEACON_BASE3			0x7e00
#define HW_BEACON_BASE4			0x7200
#define HW_BEACON_BASE5			0x7400
#define HW_BEACON_BASE6			0x5dc0
#define HW_BEACON_BASE7			0x5bc0

#define HW_BEACON_OFFSET(__index) \
	(((__index) < 4) ? (HW_BEACON_BASE0 + (__index * 0x0200)) : \
	  (((__index) < 6) ? (HW_BEACON_BASE4 + ((__index - 4) * 0x0200)) : \
	  (HW_BEACON_BASE6 - ((__index - 6) * 0x0200))))

/*
 * BBP registers.
 * The wordsize of the BBP is 8 bits.
 */

/*
 * BBP 1: TX Antenna & Power Control
 * POWER_CTRL:
 * 0 - normal,
 * 1 - drop tx power by 6dBm,
 * 2 - drop tx power by 12dBm,
 * 3 - increase tx power by 6dBm
 */
#define BBP1_TX_POWER_CTRL		FIELD8(0x07)
#define BBP1_TX_ANTENNA			FIELD8(0x18)

/*
 * BBP 3: RX Antenna
 */
#define BBP3_RX_ADC				FIELD8(0x03)
#define BBP3_RX_ANTENNA			FIELD8(0x18)
#define BBP3_HT40_MINUS			FIELD8(0x20)

/*
 * BBP 4: Bandwidth
 */
#define BBP4_TX_BF			FIELD8(0x01)
#define BBP4_BANDWIDTH			FIELD8(0x18)
#define BBP4_MAC_IF_CTRL		FIELD8(0x40)

/*
 * BBP 109
 */
#define BBP109_TX0_POWER		FIELD8(0x0f)
#define BBP109_TX1_POWER		FIELD8(0xf0)

/*
 * BBP 138: Unknown
 */
#define BBP138_RX_ADC1			FIELD8(0x02)
#define BBP138_RX_ADC2			FIELD8(0x04)
#define BBP138_TX_DAC1			FIELD8(0x20)
#define BBP138_TX_DAC2			FIELD8(0x40)

/*
 * BBP 152: Rx Ant
 */
#define BBP152_RX_DEFAULT_ANT		FIELD8(0x80)

/*
 * RFCSR registers
 * The wordsize of the RFCSR is 8 bits.
 */

/*
 * RFCSR 1:
 */
#define RFCSR1_RF_BLOCK_EN		FIELD8(0x01)
#define RFCSR1_PLL_PD			FIELD8(0x02)
#define RFCSR1_RX0_PD			FIELD8(0x04)
#define RFCSR1_TX0_PD			FIELD8(0x08)
#define RFCSR1_RX1_PD			FIELD8(0x10)
#define RFCSR1_TX1_PD			FIELD8(0x20)
#define RFCSR1_RX2_PD			FIELD8(0x40)
#define RFCSR1_TX2_PD			FIELD8(0x80)

/*
 * RFCSR 2:
 */
#define RFCSR2_RESCAL_EN		FIELD8(0x80)

/*
 * RFCSR 3:
 */
#define RFCSR3_K			FIELD8(0x0f)
/* Bits [7-4] for RF3320 (RT3370/RT3390), on other chipsets reserved */
#define RFCSR3_PA1_BIAS_CCK		FIELD8(0x70);
#define RFCSR3_PA2_CASCODE_BIAS_CCKK	FIELD8(0x80);

/*
 * FRCSR 5:
 */
#define RFCSR5_R1			FIELD8(0x0c)

/*
 * RFCSR 6:
 */
#define RFCSR6_R1			FIELD8(0x03)
#define RFCSR6_R2			FIELD8(0x40)
#define RFCSR6_TXDIV		FIELD8(0x0c)

/*
 * RFCSR 7:
 */
#define RFCSR7_RF_TUNING		FIELD8(0x01)
#define RFCSR7_BIT1			FIELD8(0x02)
#define RFCSR7_BIT2			FIELD8(0x04)
#define RFCSR7_BIT3			FIELD8(0x08)
#define RFCSR7_BIT4			FIELD8(0x10)
#define RFCSR7_BIT5			FIELD8(0x20)
#define RFCSR7_BITS67			FIELD8(0xc0)

/*
 * RFCSR 11:
 */
#define RFCSR11_R			FIELD8(0x03)

/*
 * RFCSR 12:
 */
#define RFCSR12_TX_POWER		FIELD8(0x1f)
#define RFCSR12_DR0				FIELD8(0xe0)

/*
 * RFCSR 13:
 */
#define RFCSR13_TX_POWER		FIELD8(0x1f)
#define RFCSR13_DR0				FIELD8(0xe0)

/*
 * RFCSR 15:
 */
#define RFCSR15_TX_LO2_EN		FIELD8(0x08)

/*
 * RFCSR 16:
 */
#define RFCSR16_TXMIXER_GAIN		FIELD8(0x07)

/*
 * RFCSR 17:
 */
#define RFCSR17_TXMIXER_GAIN		FIELD8(0x07)
#define RFCSR17_TX_LO1_EN		FIELD8(0x08)
#define RFCSR17_R			FIELD8(0x20)
#define RFCSR17_CODE                   FIELD8(0x7f)

/*
 * RFCSR 20:
 */
#define RFCSR20_RX_LO1_EN		FIELD8(0x08)

/*
 * RFCSR 21:
 */
#define RFCSR21_RX_LO2_EN		FIELD8(0x08)

/*
 * RFCSR 22:
 */
#define RFCSR22_BASEBAND_LOOPBACK	FIELD8(0x01)

/*
 * RFCSR 23:
 */
#define RFCSR23_FREQ_OFFSET		FIELD8(0x7f)

/*
 * RFCSR 24:
 */
#define RFCSR24_TX_AGC_FC		FIELD8(0x1f)
#define RFCSR24_TX_H20M			FIELD8(0x20)
#define RFCSR24_TX_CALIB		FIELD8(0x7f)

/*
 * RFCSR 27:
 */
#define RFCSR27_R1			FIELD8(0x03)
#define RFCSR27_R2			FIELD8(0x04)
#define RFCSR27_R3			FIELD8(0x30)
#define RFCSR27_R4			FIELD8(0x40)

/*
 * RFCSR 30:
 */
#define RFCSR30_TX_H20M			FIELD8(0x02)
#define RFCSR30_RX_H20M			FIELD8(0x04)
#define RFCSR30_RX_VCM			FIELD8(0x18)
#define RFCSR30_RF_CALIBRATION		FIELD8(0x80)

/*
 * RFCSR 31:
 */
#define RFCSR31_RX_AGC_FC		FIELD8(0x1f)
#define RFCSR31_RX_H20M			FIELD8(0x20)
#define RFCSR31_RX_CALIB		FIELD8(0x7f)

/*
 * RFCSR 38:
 */
#define RFCSR38_RX_LO1_EN		FIELD8(0x20)

/*
 * RFCSR 39:
 */
#define RFCSR39_RX_LO2_EN		FIELD8(0x80)

/*
 * RFCSR 49:
 */
#define RFCSR49_TX			FIELD8(0x3f)

/*
 * RF registers
 */

/*
 * RF 2
 */
#define RF2_ANTENNA_RX2			FIELD32(0x00000040)
#define RF2_ANTENNA_TX1			FIELD32(0x00004000)
#define RF2_ANTENNA_RX1			FIELD32(0x00020000)

/*
 * RF 3
 */
#define RF3_TXPOWER_G			FIELD32(0x00003e00)
#define RF3_TXPOWER_A_7DBM_BOOST	FIELD32(0x00000200)
#define RF3_TXPOWER_A			FIELD32(0x00003c00)

/*
 * RF 4
 */
#define RF4_TXPOWER_G			FIELD32(0x000007c0)
#define RF4_TXPOWER_A_7DBM_BOOST	FIELD32(0x00000040)
#define RF4_TXPOWER_A			FIELD32(0x00000780)
#define RF4_FREQ_OFFSET			FIELD32(0x001f8000)
#define RF4_HT40			FIELD32(0x00200000)

/*
 * EEPROM content.
 * The wordsize of the EEPROM is 16 bits.
 */

/*
 * Chip ID
 */
#define EEPROM_CHIP_ID			0x0000

/*
 * EEPROM Version
 */
#define EEPROM_VERSION			0x0001
#define EEPROM_VERSION_FAE		FIELD16(0x00ff)
#define EEPROM_VERSION_VERSION		FIELD16(0xff00)

/*
 * HW MAC address.
 */
#define EEPROM_MAC_ADDR_0		0x0002
#define EEPROM_MAC_ADDR_BYTE0		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE1		FIELD16(0xff00)
#define EEPROM_MAC_ADDR_1		0x0003
#define EEPROM_MAC_ADDR_BYTE2		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE3		FIELD16(0xff00)
#define EEPROM_MAC_ADDR_2		0x0004
#define EEPROM_MAC_ADDR_BYTE4		FIELD16(0x00ff)
#define EEPROM_MAC_ADDR_BYTE5		FIELD16(0xff00)

/*
 * EEPROM NIC Configuration 0
 * RXPATH: 1: 1R, 2: 2R, 3: 3R
 * TXPATH: 1: 1T, 2: 2T, 3: 3T
 * RF_TYPE: RFIC type
 */
#define	EEPROM_NIC_CONF0		0x001a
#define EEPROM_NIC_CONF0_RXPATH		FIELD16(0x000f)
#define EEPROM_NIC_CONF0_TXPATH		FIELD16(0x00f0)
#define EEPROM_NIC_CONF0_RF_TYPE		FIELD16(0x0f00)

/*
 * EEPROM NIC Configuration 1
 * HW_RADIO: 0: disable, 1: enable
 * EXTERNAL_TX_ALC: 0: disable, 1: enable
 * EXTERNAL_LNA_2G: 0: disable, 1: enable
 * EXTERNAL_LNA_5G: 0: disable, 1: enable
 * CARDBUS_ACCEL: 0: enable, 1: disable
 * BW40M_SB_2G: 0: disable, 1: enable
 * BW40M_SB_5G: 0: disable, 1: enable
 * WPS_PBC: 0: disable, 1: enable
 * BW40M_2G: 0: enable, 1: disable
 * BW40M_5G: 0: enable, 1: disable
 * BROADBAND_EXT_LNA: 0: disable, 1: enable
 * ANT_DIVERSITY: 00: Disable, 01: Diversity,
 * 				  10: Main antenna, 11: Aux antenna
 * INTERNAL_TX_ALC: 0: disable, 1: enable
 * BT_COEXIST: 0: disable, 1: enable
 * DAC_TEST: 0: disable, 1: enable
 */
#define	EEPROM_NIC_CONF1		0x001b
#define EEPROM_NIC_CONF1_HW_RADIO		FIELD16(0x0001)
#define EEPROM_NIC_CONF1_EXTERNAL_TX_ALC		FIELD16(0x0002)
#define EEPROM_NIC_CONF1_EXTERNAL_LNA_2G		FIELD16(0x0004)
#define EEPROM_NIC_CONF1_EXTERNAL_LNA_5G		FIELD16(0x0008)
#define EEPROM_NIC_CONF1_CARDBUS_ACCEL		FIELD16(0x0010)
#define EEPROM_NIC_CONF1_BW40M_SB_2G		FIELD16(0x0020)
#define EEPROM_NIC_CONF1_BW40M_SB_5G		FIELD16(0x0040)
#define EEPROM_NIC_CONF1_WPS_PBC		FIELD16(0x0080)
#define EEPROM_NIC_CONF1_BW40M_2G		FIELD16(0x0100)
#define EEPROM_NIC_CONF1_BW40M_5G		FIELD16(0x0200)
#define EEPROM_NIC_CONF1_BROADBAND_EXT_LNA		FIELD16(0x400)
#define EEPROM_NIC_CONF1_ANT_DIVERSITY		FIELD16(0x1800)
#define EEPROM_NIC_CONF1_INTERNAL_TX_ALC		FIELD16(0x2000)
#define EEPROM_NIC_CONF1_BT_COEXIST		FIELD16(0x4000)
#define EEPROM_NIC_CONF1_DAC_TEST		FIELD16(0x8000)

/*
 * EEPROM frequency
 */
#define	EEPROM_FREQ			0x001d
#define EEPROM_FREQ_OFFSET		FIELD16(0x00ff)
#define EEPROM_FREQ_LED_MODE		FIELD16(0x7f00)
#define EEPROM_FREQ_LED_POLARITY	FIELD16(0x1000)

/*
 * EEPROM LED
 * POLARITY_RDY_G: Polarity RDY_G setting.
 * POLARITY_RDY_A: Polarity RDY_A setting.
 * POLARITY_ACT: Polarity ACT setting.
 * POLARITY_GPIO_0: Polarity GPIO0 setting.
 * POLARITY_GPIO_1: Polarity GPIO1 setting.
 * POLARITY_GPIO_2: Polarity GPIO2 setting.
 * POLARITY_GPIO_3: Polarity GPIO3 setting.
 * POLARITY_GPIO_4: Polarity GPIO4 setting.
 * LED_MODE: Led mode.
 */
#define EEPROM_LED_AG_CONF		0x001e
#define EEPROM_LED_ACT_CONF		0x001f
#define EEPROM_LED_POLARITY		0x0020
#define EEPROM_LED_POLARITY_RDY_BG	FIELD16(0x0001)
#define EEPROM_LED_POLARITY_RDY_A	FIELD16(0x0002)
#define EEPROM_LED_POLARITY_ACT		FIELD16(0x0004)
#define EEPROM_LED_POLARITY_GPIO_0	FIELD16(0x0008)
#define EEPROM_LED_POLARITY_GPIO_1	FIELD16(0x0010)
#define EEPROM_LED_POLARITY_GPIO_2	FIELD16(0x0020)
#define EEPROM_LED_POLARITY_GPIO_3	FIELD16(0x0040)
#define EEPROM_LED_POLARITY_GPIO_4	FIELD16(0x0080)
#define EEPROM_LED_LED_MODE		FIELD16(0x1f00)

/*
 * EEPROM NIC Configuration 2
 * RX_STREAM: 0: Reserved, 1: 1 Stream, 2: 2 Stream
 * TX_STREAM: 0: Reserved, 1: 1 Stream, 2: 2 Stream
 * CRYSTAL: 00: Reserved, 01: One crystal, 10: Two crystal, 11: Reserved
 */
#define EEPROM_NIC_CONF2		0x0021
#define EEPROM_NIC_CONF2_RX_STREAM		FIELD16(0x000f)
#define EEPROM_NIC_CONF2_TX_STREAM		FIELD16(0x00f0)
#define EEPROM_NIC_CONF2_CRYSTAL		FIELD16(0x0600)

/*
 * EEPROM LNA
 */
#define EEPROM_LNA			0x0022
#define EEPROM_LNA_BG			FIELD16(0x00ff)
#define EEPROM_LNA_A0			FIELD16(0xff00)

/*
 * EEPROM RSSI BG offset
 */
#define EEPROM_RSSI_BG			0x0023
#define EEPROM_RSSI_BG_OFFSET0		FIELD16(0x00ff)
#define EEPROM_RSSI_BG_OFFSET1		FIELD16(0xff00)

/*
 * EEPROM RSSI BG2 offset
 */
#define EEPROM_RSSI_BG2			0x0024
#define EEPROM_RSSI_BG2_OFFSET2		FIELD16(0x00ff)
#define EEPROM_RSSI_BG2_LNA_A1		FIELD16(0xff00)

/*
 * EEPROM TXMIXER GAIN BG offset (note overlaps with EEPROM RSSI BG2).
 */
#define EEPROM_TXMIXER_GAIN_BG		0x0024
#define EEPROM_TXMIXER_GAIN_BG_VAL	FIELD16(0x0007)

/*
 * EEPROM RSSI A offset
 */
#define EEPROM_RSSI_A			0x0025
#define EEPROM_RSSI_A_OFFSET0		FIELD16(0x00ff)
#define EEPROM_RSSI_A_OFFSET1		FIELD16(0xff00)

/*
 * EEPROM RSSI A2 offset
 */
#define EEPROM_RSSI_A2			0x0026
#define EEPROM_RSSI_A2_OFFSET2		FIELD16(0x00ff)
#define EEPROM_RSSI_A2_LNA_A2		FIELD16(0xff00)

/*
 * EEPROM TXMIXER GAIN A offset (note overlaps with EEPROM RSSI A2).
 */
#define EEPROM_TXMIXER_GAIN_A		0x0026
#define EEPROM_TXMIXER_GAIN_A_VAL	FIELD16(0x0007)

/*
 * EEPROM EIRP Maximum TX power values(unit: dbm)
 */
#define EEPROM_EIRP_MAX_TX_POWER	0x0027
#define EEPROM_EIRP_MAX_TX_POWER_2GHZ	FIELD16(0x00ff)
#define EEPROM_EIRP_MAX_TX_POWER_5GHZ	FIELD16(0xff00)

/*
 * EEPROM TXpower delta: 20MHZ AND 40 MHZ use different power.
 * This is delta in 40MHZ.
 * VALUE: Tx Power dalta value, MAX=4(unit: dbm)
 * TYPE: 1: Plus the delta value, 0: minus the delta value
 * ENABLE: enable tx power compensation for 40BW
 */
#define EEPROM_TXPOWER_DELTA		0x0028
#define EEPROM_TXPOWER_DELTA_VALUE_2G	FIELD16(0x003f)
#define EEPROM_TXPOWER_DELTA_TYPE_2G	FIELD16(0x0040)
#define EEPROM_TXPOWER_DELTA_ENABLE_2G	FIELD16(0x0080)
#define EEPROM_TXPOWER_DELTA_VALUE_5G	FIELD16(0x3f00)
#define EEPROM_TXPOWER_DELTA_TYPE_5G	FIELD16(0x4000)
#define EEPROM_TXPOWER_DELTA_ENABLE_5G	FIELD16(0x8000)

/*
 * EEPROM TXPOWER 802.11BG
 */
#define	EEPROM_TXPOWER_BG1		0x0029
#define	EEPROM_TXPOWER_BG2		0x0030
#define EEPROM_TXPOWER_BG_SIZE		7
#define EEPROM_TXPOWER_BG_1		FIELD16(0x00ff)
#define EEPROM_TXPOWER_BG_2		FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11BG
 * MINUS4: If the actual TSSI is below this boundary, tx power needs to be
 *         reduced by (agc_step * -4)
 * MINUS3: If the actual TSSI is below this boundary, tx power needs to be
 *         reduced by (agc_step * -3)
 */
#define EEPROM_TSSI_BOUND_BG1		0x0037
#define EEPROM_TSSI_BOUND_BG1_MINUS4	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_BG1_MINUS3	FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11BG
 * MINUS2: If the actual TSSI is below this boundary, tx power needs to be
 *         reduced by (agc_step * -2)
 * MINUS1: If the actual TSSI is below this boundary, tx power needs to be
 *         reduced by (agc_step * -1)
 */
#define EEPROM_TSSI_BOUND_BG2		0x0038
#define EEPROM_TSSI_BOUND_BG2_MINUS2	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_BG2_MINUS1	FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11BG
 * REF: Reference TSSI value, no tx power changes needed
 * PLUS1: If the actual TSSI is above this boundary, tx power needs to be
 *        increased by (agc_step * 1)
 */
#define EEPROM_TSSI_BOUND_BG3		0x0039
#define EEPROM_TSSI_BOUND_BG3_REF	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_BG3_PLUS1	FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11BG
 * PLUS2: If the actual TSSI is above this boundary, tx power needs to be
 *        increased by (agc_step * 2)
 * PLUS3: If the actual TSSI is above this boundary, tx power needs to be
 *        increased by (agc_step * 3)
 */
#define EEPROM_TSSI_BOUND_BG4		0x003a
#define EEPROM_TSSI_BOUND_BG4_PLUS2	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_BG4_PLUS3	FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11BG
 * PLUS4: If the actual TSSI is above this boundary, tx power needs to be
 *        increased by (agc_step * 4)
 * AGC_STEP: Temperature compensation step.
 */
#define EEPROM_TSSI_BOUND_BG5		0x003b
#define EEPROM_TSSI_BOUND_BG5_PLUS4	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_BG5_AGC_STEP	FIELD16(0xff00)

/*
 * EEPROM TXPOWER 802.11A
 */
#define EEPROM_TXPOWER_A1		0x003c
#define EEPROM_TXPOWER_A2		0x0053
#define EEPROM_TXPOWER_A_SIZE		6
#define EEPROM_TXPOWER_A_1		FIELD16(0x00ff)
#define EEPROM_TXPOWER_A_2		FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11A
 * MINUS4: If the actual TSSI is below this boundary, tx power needs to be
 *         reduced by (agc_step * -4)
 * MINUS3: If the actual TSSI is below this boundary, tx power needs to be
 *         reduced by (agc_step * -3)
 */
#define EEPROM_TSSI_BOUND_A1		0x006a
#define EEPROM_TSSI_BOUND_A1_MINUS4	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_A1_MINUS3	FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11A
 * MINUS2: If the actual TSSI is below this boundary, tx power needs to be
 *         reduced by (agc_step * -2)
 * MINUS1: If the actual TSSI is below this boundary, tx power needs to be
 *         reduced by (agc_step * -1)
 */
#define EEPROM_TSSI_BOUND_A2		0x006b
#define EEPROM_TSSI_BOUND_A2_MINUS2	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_A2_MINUS1	FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11A
 * REF: Reference TSSI value, no tx power changes needed
 * PLUS1: If the actual TSSI is above this boundary, tx power needs to be
 *        increased by (agc_step * 1)
 */
#define EEPROM_TSSI_BOUND_A3		0x006c
#define EEPROM_TSSI_BOUND_A3_REF	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_A3_PLUS1	FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11A
 * PLUS2: If the actual TSSI is above this boundary, tx power needs to be
 *        increased by (agc_step * 2)
 * PLUS3: If the actual TSSI is above this boundary, tx power needs to be
 *        increased by (agc_step * 3)
 */
#define EEPROM_TSSI_BOUND_A4		0x006d
#define EEPROM_TSSI_BOUND_A4_PLUS2	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_A4_PLUS3	FIELD16(0xff00)

/*
 * EEPROM temperature compensation boundaries 802.11A
 * PLUS4: If the actual TSSI is above this boundary, tx power needs to be
 *        increased by (agc_step * 4)
 * AGC_STEP: Temperature compensation step.
 */
#define EEPROM_TSSI_BOUND_A5		0x006e
#define EEPROM_TSSI_BOUND_A5_PLUS4	FIELD16(0x00ff)
#define EEPROM_TSSI_BOUND_A5_AGC_STEP	FIELD16(0xff00)

/*
 * EEPROM TXPOWER by rate: tx power per tx rate for HT20 mode
 */
#define EEPROM_TXPOWER_BYRATE		0x006f
#define EEPROM_TXPOWER_BYRATE_SIZE	9

#define EEPROM_TXPOWER_BYRATE_RATE0	FIELD16(0x000f)
#define EEPROM_TXPOWER_BYRATE_RATE1	FIELD16(0x00f0)
#define EEPROM_TXPOWER_BYRATE_RATE2	FIELD16(0x0f00)
#define EEPROM_TXPOWER_BYRATE_RATE3	FIELD16(0xf000)

/*
 * EEPROM BBP.
 */
#define	EEPROM_BBP_START		0x0078
#define EEPROM_BBP_SIZE			16
#define EEPROM_BBP_VALUE		FIELD16(0x00ff)
#define EEPROM_BBP_REG_ID		FIELD16(0xff00)

/*
 * MCU mailbox commands.
 * MCU_SLEEP - go to power-save mode.
 *             arg1: 1: save as much power as possible, 0: save less power.
 *             status: 1: success, 2: already asleep,
 *                     3: maybe MAC is busy so can't finish this task.
 * MCU_RADIO_OFF
 *             arg0: 0: do power-saving, NOT turn off radio.
 */
#define MCU_SLEEP			0x30
#define MCU_WAKEUP			0x31
#define MCU_RADIO_OFF			0x35
#define MCU_CURRENT			0x36
#define MCU_LED				0x50
#define MCU_LED_STRENGTH		0x51
#define MCU_LED_AG_CONF		0x52
#define MCU_LED_ACT_CONF		0x53
#define MCU_LED_LED_POLARITY		0x54
#define MCU_RADAR			0x60
#define MCU_BOOT_SIGNAL			0x72
#define MCU_ANT_SELECT			0X73
#define MCU_BBP_SIGNAL			0x80
#define MCU_POWER_SAVE			0x83
#define MCU_BAND_SELECT		0x91

/*
 * MCU mailbox tokens
 */
#define TOKEN_SLEEP			1
#define TOKEN_RADIO_OFF			2
#define TOKEN_WAKEUP			3


/*
 * DMA descriptor defines.
 */
#define TXWI_DESC_SIZE			(4 * sizeof(__le32))
#define RXWI_DESC_SIZE			(4 * sizeof(__le32))

/*
 * TX WI structure
 */

/*
 * Word0
 * FRAG: 1 To inform TKIP engine this is a fragment.
 * MIMO_PS: The remote peer is in dynamic MIMO-PS mode
 * TX_OP: 0:HT TXOP rule , 1:PIFS TX ,2:Backoff, 3:sifs
 * BW: Channel bandwidth 0:20MHz, 1:40 MHz (for legacy rates this will
 *     duplicate the frame to both channels).
 * STBC: 1: STBC support MCS =0-7, 2,3 : RESERVED
 * AMPDU: 1: this frame is eligible for AMPDU aggregation, the hw will
 *        aggregate consecutive frames with the same RA and QoS TID. If
 *        a frame A with the same RA and QoS TID but AMPDU=0 is queued
 *        directly after a frame B with AMPDU=1, frame A might still
 *        get aggregated into the AMPDU started by frame B. So, setting
 *        AMPDU to 0 does _not_ necessarily mean the frame is sent as
 *        MPDU, it can still end up in an AMPDU if the previous frame
 *        was tagged as AMPDU.
 */
#define TXWI_W0_FRAG			FIELD32(0x00000001)
#define TXWI_W0_MIMO_PS			FIELD32(0x00000002)
#define TXWI_W0_CF_ACK			FIELD32(0x00000004)
#define TXWI_W0_TS			FIELD32(0x00000008)
#define TXWI_W0_AMPDU			FIELD32(0x00000010)
#define TXWI_W0_MPDU_DENSITY		FIELD32(0x000000e0)
#define TXWI_W0_TX_OP			FIELD32(0x00000300)
#define TXWI_W0_MCS			FIELD32(0x007f0000)
#define TXWI_W0_BW			FIELD32(0x00800000)
#define TXWI_W0_SHORT_GI		FIELD32(0x01000000)
#define TXWI_W0_STBC			FIELD32(0x06000000)
#define TXWI_W0_IFS			FIELD32(0x08000000)
#define TXWI_W0_PHYMODE			FIELD32(0xc0000000)

/*
 * Word1
 * ACK: 0: No Ack needed, 1: Ack needed
 * NSEQ: 0: Don't assign hw sequence number, 1: Assign hw sequence number
 * BW_WIN_SIZE: BA windows size of the recipient
 * WIRELESS_CLI_ID: Client ID for WCID table access
 * MPDU_TOTAL_BYTE_COUNT: Length of 802.11 frame
 * PACKETID: Will be latched into the TX_STA_FIFO register once the according
 *           frame was processed. If multiple frames are aggregated together
 *           (AMPDU==1) the reported tx status will always contain the packet
 *           id of the first frame. 0: Don't report tx status for this frame.
 * PACKETID_QUEUE: Part of PACKETID, This is the queue index (0-3)
 * PACKETID_ENTRY: Part of PACKETID, THis is the queue entry index (1-3)
 *                 This identification number is calculated by ((idx % 3) + 1).
 *		   The (+1) is required to prevent PACKETID to become 0.
 */
#define TXWI_W1_ACK			FIELD32(0x00000001)
#define TXWI_W1_NSEQ			FIELD32(0x00000002)
#define TXWI_W1_BW_WIN_SIZE		FIELD32(0x000000fc)
#define TXWI_W1_WIRELESS_CLI_ID		FIELD32(0x0000ff00)
#define TXWI_W1_MPDU_TOTAL_BYTE_COUNT	FIELD32(0x0fff0000)
#define TXWI_W1_PACKETID		FIELD32(0xf0000000)
#define TXWI_W1_PACKETID_QUEUE		FIELD32(0x30000000)
#define TXWI_W1_PACKETID_ENTRY		FIELD32(0xc0000000)

/*
 * Word2
 */
#define TXWI_W2_IV			FIELD32(0xffffffff)

/*
 * Word3
 */
#define TXWI_W3_EIV			FIELD32(0xffffffff)

/*
 * RX WI structure
 */

/*
 * Word0
 */
#define RXWI_W0_WIRELESS_CLI_ID		FIELD32(0x000000ff)
#define RXWI_W0_KEY_INDEX		FIELD32(0x00000300)
#define RXWI_W0_BSSID			FIELD32(0x00001c00)
#define RXWI_W0_UDF			FIELD32(0x0000e000)
#define RXWI_W0_MPDU_TOTAL_BYTE_COUNT	FIELD32(0x0fff0000)
#define RXWI_W0_TID			FIELD32(0xf0000000)

/*
 * Word1
 */
#define RXWI_W1_FRAG			FIELD32(0x0000000f)
#define RXWI_W1_SEQUENCE		FIELD32(0x0000fff0)
#define RXWI_W1_MCS			FIELD32(0x007f0000)
#define RXWI_W1_BW			FIELD32(0x00800000)
#define RXWI_W1_SHORT_GI		FIELD32(0x01000000)
#define RXWI_W1_STBC			FIELD32(0x06000000)
#define RXWI_W1_PHYMODE			FIELD32(0xc0000000)

/*
 * Word2
 */
#define RXWI_W2_RSSI0			FIELD32(0x000000ff)
#define RXWI_W2_RSSI1			FIELD32(0x0000ff00)
#define RXWI_W2_RSSI2			FIELD32(0x00ff0000)

/*
 * Word3
 */
#define RXWI_W3_SNR0			FIELD32(0x000000ff)
#define RXWI_W3_SNR1			FIELD32(0x0000ff00)

/*
 * Macros for converting txpower from EEPROM to mac80211 value
 * and from mac80211 value to register value.
 */
#define MIN_G_TXPOWER	0
#define MIN_A_TXPOWER	-7
#define MAX_G_TXPOWER	31
#define MAX_A_TXPOWER	15
#define DEFAULT_TXPOWER	5

#define TXPOWER_G_FROM_DEV(__txpower) \
	((__txpower) > MAX_G_TXPOWER) ? DEFAULT_TXPOWER : (__txpower)

#define TXPOWER_G_TO_DEV(__txpower) \
	clamp_t(char, __txpower, MIN_G_TXPOWER, MAX_G_TXPOWER)

#define TXPOWER_A_FROM_DEV(__txpower) \
	((__txpower) > MAX_A_TXPOWER) ? DEFAULT_TXPOWER : (__txpower)

#define TXPOWER_A_TO_DEV(__txpower) \
	clamp_t(char, __txpower, MIN_A_TXPOWER, MAX_A_TXPOWER)

/*
 *  Board's maximun TX power limitation
 */
#define EIRP_MAX_TX_POWER_LIMIT	0x50

/*
 * Number of TBTT intervals after which we have to adjust
 * the hw beacon timer.
 */
#define BCN_TBTT_OFFSET 64

/*
 * RT2800 driver data structure
 */
struct rt2800_drv_data {
	u8 calibration_bw20;
	u8 calibration_bw40;
	u8 bbp25;
	u8 bbp26;
	u8 txmixer_gain_24g;
	u8 txmixer_gain_5g;
	unsigned int tbtt_tick;
};

#endif /* RT2800_H */
