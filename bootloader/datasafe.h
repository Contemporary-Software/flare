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

/**
 * Data safe is a static block of memory not directly accessable from the boot
 * loader or application. Directly accessable means the memory is not part of
 * the statically linked or heap memory.
 *
 * The datasafe has been moved to DDR ram and seems work stay around after a
 * reset. This is the only memory not touched by the secure boot of the Zynq.
 *
 * In the application most of the access is read only so it assumes the contents
 * are not changing. Updates are special cases.
 */

#if !defined(_DATASAFE_H_)
#define _DATASAFE_H_

#include <stdbool.h>
#include <stdint.h>

#define FLARE_DS_BASE     (0x00080000UL)
#define FLARE_DS_CRC_BASE ((const unsigned char*)(FLARE_DS_BASE + 2*sizeof(uint32_t)))
#define FLARE_DS_CRC_LEN  (sizeof(flare_datasafe) - 2*sizeof(uint32_t))

/*
 * status bit masks
 */
#define FLARE_DS_STATUS_FACTORY_DATA_VALID   (1 << 0)

/*
 * bootmode bit masks
 */
#define FLARE_DS_BOOTMODE_FACTORY     (1 << 31) /* Set by the FSBL to indicate a
                                                 * factory boot */
#define FLARE_DS_BOOTMODE_FACTORY_REQ (1 << 30) /* Set in the application to request
                                                 * booting into factory boot mode. */

#define FLARE_DS_BOOTMODE_HW_MASK     (0xF)
#define FLARE_DS_BOOTMODE_ERROR       (1 << 0) /* Boot mode unknown or unsupported */
#define FLARE_DS_BOOTMODE_QSPI        (1 << 1) /* Boot from QSPI */
#define FLARE_DS_BOOTMODE_SD_CARD     (1 << 2) /* Boot from SD card */
#define FLARE_DS_BOOTMODE_JTAG        (1 << 3) /* Boot from JTAG */

/*
 * reset bit masks
 */
#define FLARE_DS_RESET_MASK (0x3F << 26)
#define FLARE_DS_RESET_POR (1 << 31) /* Power on reset */
#define FLARE_DS_RESET_WDT (1 << 30) /* Watch dog timer */
#define FLARE_DS_RESET_SWR (1 << 29) /* Software reset */
#define FLARE_DS_RESET_DBG (1 << 28) /* Debug reset */
#define FLARE_DS_RESET_EXT (1 << 27) /* External reset */
#define FLARE_DS_RESET_ERR (1 << 26) /* Unknown or unsupported reset */

/*
 * The data safe.
 *
 * Definitions of the data safe can be found in datasafe.txt.
 * This is datasafe version 1
 */
typedef struct
{
    uint32_t              crc32;
    uint32_t              length;
    uint32_t              format;
#define FLARE_DS_BOOT_PATH_LEN            (64)
#define FLARE_DS_FACTORY_DATA_LEN         (32)
#define FLARE_DS_ERROR_TRACE_LEN          (256)
#define FLARE_DS_FACTORY_APP_DETAILS_SIZE (128)
    uint32_t              count;
    uint32_t              reset;
    uint32_t              flare_version;
    uint32_t              bootmode;
    uint32_t              status;
    char                  boot_path[FLARE_DS_BOOT_PATH_LEN];
    char                  boot_loader[FLARE_DS_BOOT_PATH_LEN];
    char                  boot_firmware[FLARE_DS_BOOT_PATH_LEN];
    char                  boot_exe[FLARE_DS_BOOT_PATH_LEN];
    uint8_t               mac_address_0[6];
    uint8_t               mac_address_1[6];
    uint8_t               mac_address_2[6];
    uint8_t               mac_address_3[6];
    char                  assembly_serial_number[FLARE_DS_FACTORY_DATA_LEN];
    char                  part_number[FLARE_DS_FACTORY_DATA_LEN];
    char                  revision[FLARE_DS_FACTORY_DATA_LEN];
    char                  mod_strike[FLARE_DS_FACTORY_DATA_LEN];
    char                  board_serial_number[FLARE_DS_FACTORY_DATA_LEN];
    char                  app_data[FLARE_DS_FACTORY_APP_DETAILS_SIZE];
    char                  boot_cmd[FLARE_DS_FACTORY_APP_DETAILS_SIZE];
    uint32_t              error_trace[FLARE_DS_ERROR_TRACE_LEN];
} flare_datasafe;

/*
 * Initialise the datasafe clearing the memory if the hash is not valid and
 * if valid update the count. Always set the reset value.
 */
void flare_datasafe_init();

/*
 * Set the boot path and loader.
 */
void flare_datasafe_set_boot(const char* path, const char* exe);

/*
 * Set the bootmode.
 */
void flare_datasafe_set_bootmode(uint32_t bootmode);

/*
 * Set the factory settings.
 */
void flare_datasafe_factory_data_set(const uint8_t* mac,
                                     const char*    serial,
                                     const char*    part,
                                     const char*    revision,
                                     const char*    mod,
                                     const char*    board_serial,
                                     const char*    app_data,
                                     const char*    boot_cmd);

/*
 * Clear the factory data
 */
void flare_datasafe_factory_data_clear();

/*
 * Set the factory boot flag.
 */
void flare_datasafe_set_factory_boot(void);

/*
 * Is the datasafe data valid ? Only available in the app.
 */
bool flare_datasafe_valid(void);

/*
 * Request a factory boot.
 */
void flare_datasafe_request_factory_boot(void);

/*
 * Clear a factory boot request.
 */
void flare_datasafe_clear_factory_boot_request(void);

/*
 * Has a factory boot been requested?
 */
bool flare_datasafe_factory_boot_requested(void);

#endif
