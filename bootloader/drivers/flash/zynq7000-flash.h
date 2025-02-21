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

#if !defined(_BOOTLOADER_FLASH_ZYNQ_H_)
#define _BOOTLOADER_FLASH_ZYNQ_H_

#include "flash.h"
#include "flare-io.h"

#define FLASH_COMM_METHOD_SINGLE 0 /* Unused */
#define FLASH_COMM_METHOD_ALL 0 /* Unused */


/*
 * QSPI registers.
 */
#define QSPI_REG_CONFIG           0x00000000
#define QSPI_REG_INTR_STATUS      0x00000004
#define QSPI_REG_INTR_ENABLE      0x00000005
#define QSPI_REG_INTR_DISABLE     0x0000000c
#define QSPI_REG_INTR_MASK        0x00000010
#define QSPI_REG_EN               0x00000014
#define QSPI_REG_DELAY            0x00000018
#define QSPI_REG_TXD0             0x0000001c
#define QSPI_REG_RX_DATA          0x00000020
#define QSPI_REG_SLAVE_IDLE_COUNT 0x00000024
#define QSPI_REG_TX_THRES         0x00000028
#define QSPI_REG_RX_THRES         0x0000002c
#define QSPI_REG_GPIO             0x00000030
#define QSPI_REG_LPBK_DLY_ADJ     0x00000038
#define QSPI_REG_TXD1             0x00000080
#define QSPI_REG_TXD2             0x00000084
#define QSPI_REG_TXD3             0x00000088
#define QSPI_REG_LSPI_CFG         0x000000a0
#define QSPI_REG_LSPI_STS         0x000000a4
#define QSPI_REG_MOD_ID           0x000000fc

/*
 * TX FIFO depth in words.
 */
#define QSPI_FIFO_DEPTH   (63)

/*
 * Control register.
 */
#define QSPI_CR_HOLDB_DR        (1 << 19)
#define QSPI_CR_MANSTRT         (1 << 16)
#define QSPI_CR_MANSTRTEN       (1 << 15)
#define QSPI_CR_SSFORCE         (1 << 14)
#define QSPI_CR_PCS             (1 << 10)
#define QSPI_CR_BAUD_RATE_DIV_2 (0 << 3)
#define QSPI_CR_BAUD_RATE_DIV_4 (1 << 3)
#define QSPI_CR_BAUD_RATE_DIV_8 (2 << 3)
#define QSPI_CR_MODE_SEL        (1 << 0)

/*
 * Fast clock rate of 100MHz for fast reads.
 */
#define QSPI_CR_BAUD_RATE_FAST  QSPI_CR_BAUD_RATE_DIV_2

/*
 * Status register.
 */
#define QSPI_IXR_RXNEMPTY  (1 << 4)
#define QSPI_IXR_TXFULL    (1 << 3)
#define QSPI_IXR_TXOW      (1 << 2)

/*
 * Enable register.
 */
#define QSPI_EN_SPI_ENABLE  (1 << 0)

/*
 * Clock rate is 200MHz and 50MHz is the normal rate and 100MHz the fast rate.
 */
#if FLASH_FAST_READ
 #define QSPI_CR_BAUD_RATE QSPI_CR_BAUD_RATE_DIV_2
#else
 #define QSPI_CR_BAUD_RATE QSPI_CR_BAUD_RATE_DIV_4
#endif



/*
 * Flash Status bits.
 */
#define FLASH_SR_WIP   (1 << 0)
#define FLASH_SR_WEL   (1 << 1)
#define FLASH_SR_BP0   (1 << 2)
#define FLASH_SR_BP1   (1 << 3)
#define FLASH_SR_BP2   (1 << 4)
#define FLASH_SR_E_ERR (1 << 5)
#define FLASH_SR_P_ERR (1 << 6)
#define FLASH_SR_SRWD  (1 << 7)


void flash_writeUnlock(void);

void flash_writeLock(void);

flash_error flash_Transfer(flash_transfer_buffer* transfer, bool initialised);

#endif /* _BOOTLOADER_FLASH_ZYNQ_H_ */
