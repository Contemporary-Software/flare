/*
 * Copyright 2024 Contemporary Software
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#if !defined(_BOOTLOADER_FLASH_VERSAL_H_)
#define _BOOTLOADER_FLASH_VERSAL_H_

#include <io/flare-io.h>

#include "flash.h"

#define QSPI_CONFIG_INIT_VAL (GQSPI_CFG_GEN_FIFO_START_MODE_MASK|GQSPI_CFG_WP_HOLD_MASK)

#define FLASH_COMM_METHOD_SINGLE FLASH_COMM_METHOD_SINGLE_BOTTOM
#define FLASH_COMM_METHOD_ALL FLASH_COMM_METHOD_SINGLE_BOTTOM


/*
 * Flash Status bits.
 */
#define FLASH_SR_WIP   (1 << 0)
#define FLASH_SR_WEL   (1 << 1)
#define FLASH_SR_BP0   (1 << 2)
#define FLASH_SR_BP1   (1 << 3)
#define FLASH_SR_BP2   (1 << 4)
#define FLASH_SR_TB    (1 << 5)
#define FLASH_SR_BP3   (1 << 6)
#define FLASH_SR_SRWD  (1 << 7)

#define FLASH_SR_P_ERR (1 << 4)
#define FLASH_SR_E_ERR (1 << 5)

/* Command Word Offsets */
#define CMD_OFFSET_IMM_DATA   0
#define CMD_OFFSET_DATA_XFER  8
#define CMD_OFFSET_EXP        9
#define CMD_OFFSET_MODE       10
#define CMD_OFFSET_CS_LOWER   12
#define CMD_OFFSET_CS_UPPER   13
#define CMD_OFFSET_BUS_SEL    14
#define CMD_OFFSET_TX         16
#define CMD_OFFSET_RX         17
#define CMD_OFFSET_STRIPE     18
#define CMD_OFFSET_POLL       19

/* Generic QSPI register offsets */
#define GQSPI_CONFIG_OFST                 0x00000100
#define GQSPI_ISR_OFST                    0x00000104
#define GQSPI_IDR_OFST                    0x0000010C
#define GQSPI_IER_OFST                    0x00000108
#define GQSPI_IMASK_OFST                  0x00000110
#define GQSPI_EN_OFST                     0x00000114
#define GQSPI_TXD_OFST                    0x0000011C
#define GQSPI_RXD_OFST                    0x00000120
#define GQSPI_TX_THRESHOLD_OFST           0x00000128
#define GQSPI_RX_THRESHOLD_OFST           0x0000012C
#define GQSPI_LPBK_DLY_ADJ_OFST           0x00000138
#define GQSPI_GEN_FIFO_OFST               0x00000140
#define GQSPI_SEL_OFST                    0x00000144
#define GQSPI_GF_THRESHOLD_OFST           0x00000150
#define GQSPI_FIFO_CTRL_OFST              0x0000014C
#define GQSPI_QSPIDMA_DST_CTRL_OFST       0x0000080C
#define GQSPI_QSPIDMA_DST_SIZE_OFST       0x00000804
#define GQSPI_QSPIDMA_DST_STS_OFST        0x00000808
#define GQSPI_QSPIDMA_DST_I_STS_OFST      0x00000814
#define GQSPI_QSPIDMA_DST_I_EN_OFST       0x00000818
#define GQSPI_QSPIDMA_DST_I_DIS_OFST      0x0000081C
#define GQSPI_QSPIDMA_DST_I_MASK_OFST     0x00000820
#define GQSPI_QSPIDMA_DST_ADDR_OFST       0x00000800
#define GQSPI_QSPIDMA_DST_ADDR_MSB_OFST   0x00000828

/* GQSPI register bit masks */
#define GQSPI_SEL_MASK                      0x00000001
#define GQSPI_EN_MASK                       0x00000001
#define GQSPI_LPBK_DLY_ADJ_USE_LPBK_MASK    0x00000020
#define GQSPI_ISR_WR_TO_CLR_MASK            0x00000002
#define GQSPI_IDR_ALL_MASK                  0x00000FBE
#define GQSPI_CFG_MODE_EN_MASK              0xC0000000
#define GQSPI_CFG_GEN_FIFO_START_MODE_MASK  0x20000000
#define GQSPI_CFG_ENDIAN_MASK               0x04000000
#define GQSPI_CFG_EN_POLL_TO_MASK           0x00100000
#define GQSPI_CFG_WP_HOLD_MASK              0x00080000
#define GQSPI_CFG_BAUD_RATE_DIV_MASK        0x00000038
#define GQSPI_CFG_CLK_PHA_MASK              0x00000004
#define GQSPI_CFG_CLK_POL_MASK              0x00000002
#define GQSPI_CFG_START_GEN_FIFO_MASK       0x10000000
#define GQSPI_GENFIFO_IMM_DATA_MASK         0x000000FF
#define GQSPI_GENFIFO_DATA_XFER             0x00000100
#define GQSPI_GENFIFO_EXP                   0x00000200
#define GQSPI_GENFIFO_MODE_SPI              0x00000400
#define GQSPI_GENFIFO_MODE_DUALSPI          0x00000800
#define GQSPI_GENFIFO_MODE_QUADSPI          0x00000C00
#define GQSPI_GENFIFO_MODE_MASK             0x00000C00
#define GQSPI_GENFIFO_CS_LOWER              0x00001000
#define GQSPI_GENFIFO_CS_UPPER              0x00002000
#define GQSPI_GENFIFO_BUS_LOWER             0x00004000
#define GQSPI_GENFIFO_BUS_UPPER             0x00008000
#define GQSPI_GENFIFO_BUS_BOTH              0x0000C000
#define GQSPI_GENFIFO_BUS_MASK              0x0000C000
#define GQSPI_GENFIFO_TX                    0x00010000
#define GQSPI_GENFIFO_RX                    0x00020000
#define GQSPI_GENFIFO_STRIPE                0x00040000
#define GQSPI_GENFIFO_POLL                  0x00080000
#define GQSPI_GENFIFO_EXP_START             0x00000100
#define GQSPI_FIFO_CTRL_RST_RX_FIFO_MASK    0x00000004
#define GQSPI_FIFO_CTRL_RST_TX_FIFO_MASK    0x00000002
#define GQSPI_FIFO_CTRL_RST_GEN_FIFO_MASK   0x00000001
#define GQSPI_ISR_RXEMPTY_MASK              0x00000800
#define GQSPI_ISR_GENFIFOFULL_MASK          0x00000400
#define GQSPI_ISR_GENFIFONOT_FULL_MASK      0x00000200
#define GQSPI_ISR_TXEMPTY_MASK              0x00000100
#define GQSPI_ISR_GENFIFOEMPTY_MASK         0x00000080
#define GQSPI_ISR_RXFULL_MASK               0x00000020
#define GQSPI_ISR_RXNEMPTY_MASK             0x00000010
#define GQSPI_ISR_TXFULL_MASK               0x00000008
#define GQSPI_ISR_TXNOT_FULL_MASK           0x00000004
#define GQSPI_ISR_POLL_TIME_EXPIRE_MASK     0x00000002
#define GQSPI_IER_TXNOT_FULL_MASK           0x00000004
#define GQSPI_IER_RXEMPTY_MASK              0x00000800
#define GQSPI_IER_POLL_TIME_EXPIRE_MASK     0x00000002
#define GQSPI_IER_RXNEMPTY_MASK             0x00000010
#define GQSPI_IER_GENFIFOEMPTY_MASK         0x00000080
#define GQSPI_IER_TXEMPTY_MASK              0x00000100
#define GQSPI_QSPIDMA_DST_INTR_ALL_MASK     0x000000FE
#define GQSPI_QSPIDMA_DST_STS_WTC           0x0000E000
#define GQSPI_CFG_MODE_EN_DMA_MASK          0x80000000
#define GQSPI_ISR_IDR_MASK                  0x00000994
#define GQSPI_QSPIDMA_DST_I_EN_DONE_MASK    0x00000002
#define GQSPI_QSPIDMA_DST_I_STS_DONE_MASK   0x00000002
#define GQSPI_IRQ_MASK                      0x00000980
#define GQSPI_FIFO_CTRL_RST_GEN_MASK        0x00000001
#define GQSPI_FIFO_CTRL_RST_TX_MASK         0x00000002
#define GQSPI_FIFO_CTRL_RST_RX_MASK         0x00000003
#define GQSPI_FIFO_CTRL_RST_ALL_MASK        0x00000007

#define GQSPI_CFG_BAUD_RATE_DIV_SHIFT       3
#define GQSPI_GENFIFO_CS_SETUP              0x4
#define GQSPI_GENFIFO_CS_HOLD               0x3
#define GQSPI_TXD_DEPTH                     64
#define GQSPI_RX_FIFO_THRESHOLD             32
#define GQSPI_RX_FIFO_FILL                  (GQSPI_RX_FIFO_THRESHOLD * 4)
#define GQSPI_TX_FIFO_THRESHOLD_RESET_VAL   32
#define GQSPI_TX_FIFO_FILL                  (GQSPI_TXD_DEPTH -\
                                            GQSPI_TX_FIFO_THRESHOLD_RESET_VAL)
#define GQSPI_GEN_FIFO_THRESHOLD_RESET_VAL  0X10
#define GQSPI_QSPIDMA_DST_CTRL_RESET_VAL    0x803FFA00
#define GQSPI_SELECT_FLASH_CS_LOWER         0x1
#define GQSPI_SELECT_FLASH_CS_UPPER         0x2
#define GQSPI_SELECT_FLASH_CS_BOTH          0x3
#define GQSPI_SELECT_FLASH_BUS_LOWER        0x1
#define GQSPI_SELECT_FLASH_BUS_UPPER        0x2
#define GQSPI_SELECT_FLASH_BUS_BOTH         0x3
#define GQSPI_BAUD_DIV_MAX                  7  /* Baud rate divisor maximum */
#define GQSPI_BAUD_DIV_SHIFT                2  /* Baud rate divisor shift */
#define GQSPI_SELECT_MODE_SPI               0x1
#define GQSPI_SELECT_MODE_DUALSPI           0x2
#define GQSPI_SELECT_MODE_QUADSPI           0x4
#define GQSPI_DMA_UNALIGN                   0x3
#define GQSPI_DEFAULT_NUM_CS                1  /* Default number of chip selects */
#define GQSPI_MAX_NUM_CS                    2  /* Maximum number of chip selects */

void flash_writeUnlock(void);

void flash_writeLock(void);

flash_error flash_Transfer(flash_transfer_buffer* transfer, bool initialised);


#endif /* _BOOTLOADER_FLASH_VERSAL_H_ */
