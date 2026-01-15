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

/*
 * Xilinx Versal.
 */

#if !defined(VERSAL_H)
#define VERSAL_H

#include <stdint.h>
#include <board-io.h>

#define ARCHITECTURE "Versal"

/*
 * PS reset control register define
 */
#define PS_RST_MASK         0x00000001 /**< PS software reset */

/*
 * SLCR BOOT Mode Register defines
 */
#define BOOT_MODES_MASK     0x00000007 /**< FLASH types */

/*
 * Boot Modes
 */
#define JTAG_MODE           0x00000000 /**< JTAG Boot Mode */
#define SELECT_MAP_MODE     0x0000000A /**< SELECT MAP Boot Mode */
#define OSPI_MODE           0x00000008 /**< OSPI Boot Mode */
#define QSPI_MODE           0x00000002 /**< QSPI32 Boot Mode */
#define SD_MODE             0x00000005 /**< SD Boot Mode */
#define MMC_MODE            0x00000006 /**< eMMC 4.51 1.8v Boot Device */
#define NO_MODE_MODE        0x00000007 /**< Invalid mode. */

#define RESET_REASON_SRST   0x00000020 /**< Reason for reset is SRST */
#define RESET_REASON_SWDT   0x00000001 /**< Reason for reset is SWDT */


/*
 * Register Addresses
 */

#define BOOT_MODE_REG       (0xf1260200)
#define REBOOT_STATUS_REG   (0xf1260220)

#endif /* VERSAL_H */
