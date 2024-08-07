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
 ==================================================================
 * Copyright (C) 2016 Southern Innovation International Pty Ltd.  *
 * All rights reserved.                                           *
 *                                                                *
 * Unauthorised copying, distribution, use, modification, reverse *
 * engineering or disclosure is strictly prohibited.              *
 *                                                                *
 * This file, its contents and the  fact of its disclosure to you *
 * is  Confidential  Information  and  Proprietary Information of *
 * Southern   Innovation   International  Pty  Ltd  and  Southern *
 * Innovation Trading Pty Ltd.   Unauthorised  disclosure of such *
 * information is strictly prohibited.                            *
 *                                                                *
 * THIS SOURCE CODE AND  SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY *
 * REPRESENTATIONS  OR WARRANTIES OTHER  THAN THOSE WHICH MAY NOT *
 * BE  EXCLUDED BY LAW.   IN NO EVENT  SHALL  SOUTHERN INNOVATION *
 * INTERNATIONAL PTY LTD  OR  SOUTHERN INNOVATION TRADING PTY LTD *
 * BE LIABLE  FOR ANY SPECIAL, DIRECT, INDIRECT  OR CONSEQUENTIAL *
 * DAMAGES   HOWSOEVER  INCURRED  (INCLUDING  BY  NEGLIGENCE)  IN *
 * CONNECTION WITH THE USE OF THIS SOURCE CODE AND SOFTWARE.      *
 * WHERE SUCH LIABILITY  MAY ONLY BE LIMITED AT LAW, LIABILITY IS *
 * LIMITED TO THE MAXIMUM EXTENT PERMITTED BY LAW.                *
 ==================================================================
*/

/**
 * Flash Layout
 */

#if !defined(_FLARE_VERSAL_FLASH_H_)
#define _FLARE_VERSAL_FLASH_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Flash memory map.
 */
#define FLARE_FLASH_SIZE             flash_DeviceSize()
#define FLARE_FLASH_BLOCK_SIZE       flash_DeviceSectorEraseSize()

#define FLARE_FLASH_BOOT_BASE        (0UL)
#define FLARE_FLASH_BOOT_SIZE        (16UL * 1024 * 1024)
#define FLARE_FLASH_FACTORY_BASE     (FLARE_FLASH_SIZE - FLARE_FLASH_FACTORY_SIZE)
#define FLARE_FLASH_FACTORY_SIZE     (FLARE_FLASH_BLOCK_SIZE)
#define FLARE_FLASH_FILESYSTEM_BASE  (FLARE_FLASH_BOOT_BASE + FLARE_FLASH_BOOT_SIZE)
#define FLARE_FLASH_FILESYSTEM_SIZE  (FLARE_FLASH_FACTORY_BASE - \
                                        (FLARE_FLASH_BOOT_BASE + FLARE_FLASH_BOOT_SIZE))

/*
 * Factory flash partition.
 */
#define FLARE_FLASH_FSBL_BASE        FLARE_FLASH_BOOT_BASE
#define FLARE_FLASH_FSBL_SIZE        (3 * 1024 * 1024)
#define FLARE_FLASH_SSBL_BASE        (FLARE_FLASH_FSBL_BASE + FLARE_FLASH_FSBL_SIZE)
#define FLARE_FLASH_SSBL_SIZE        (128 * 1024)
#define FLARE_FLASH_EXE_BASE         (FLARE_FLASH_SSBL_BASE + FLARE_FLASH_SSBL_SIZE)
#define FLARE_FLASH_EXE_SIZE         (FLARE_FLASH_BOOT_SIZE - FLARE_FLASH_EXE_BASE)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
