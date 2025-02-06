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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define FLARE_DS_BASE       (0x00080000UL)
#define FLARE_DS_CRC_BASE   ((const unsigned char*)(FLARE_DS_BASE + sizeof(uint32_t)))
#define FLARE_DS_CRC_LEN    (sizeof(flare_datasafe) - sizeof(uint32_t))

/*
 * Indicate if this is a factory boot.
 *
 * @note the factory boot request mode is a work around related to the REBOOT_STATUS
 *       register not clearing after reset.
 */
#define FLARE_DS_BOOTMODE_MASK        (0xff)
#define FLARE_DS_RUNMODE_BIT_OFF      (16)
#define FLARE_DS_RUNMODE_MASK         (0xff)
#define FLARE_DS_BOOTMODE_FACTORY     (1 << 31) /* Set by the FSBL to indicate a
                                                 * factory boot */
#define FLARE_DS_BOOTMODE_FACTORY_REQ (1 << 30) /* Set in the application to request
                                                 * booting into factory boot mode. */
#define FLARE_DS_BOOTMODE_BITFILE_LD  (1 << 29) /* The bitfile has been loaded into
                                                 * the PL. */
#define FLARE_DS_BOOTMODE_BS_CHECKSUM (1 << 28) /* Boot script supports a checksum */

#define FLARE_DS_GET_RUNMODE(_m) (((_m) >> FLARE_DS_RUNMODE_BIT_OFF) & FLARE_DS_RUNMODE_MASK)

/*
 * Mask of request bits in the bootmode field that are not cleared on reset.
 */
#define FLARE_DS_BOOTMODE_REQ_MASK                      \
  (FLARE_DS_BOOTMODE_FACTORY_REQ |                      \
   (FLARE_DS_RUNMODE_MASK << FLARE_DS_RUNMODE_BIT_OFF))

/*
 * Version number is always a 7 digit string. The following format string must
 * be used as:
 *
 *   printf("Version: %" FLARE_VERSION_PRI "lx\n", bc->flare_version);
 */
#define FLARE_VERSION_PRI "07"

/*
 * The data safe.
 *
 * Definitions of the data safe can be found in datasafe.txt.
 * This is datasafe version 1
 */
typedef struct
{
    uint32_t              crc32;
    uint32_t              format;
#define FLARE_DS_BOOT_PATH_LEN    (64)
#define FLARE_DS_FACTORY_DATA_LEN (32)
#define FLARE_DS_ERROR_TRACE_LEN  (256)
    uint32_t              count;
    uint32_t              reset;
    uint32_t              flare_version;
    uint32_t              bootmode;
    char                  boot_path[FLARE_DS_BOOT_PATH_LEN];
    char                  boot_loader[FLARE_DS_BOOT_PATH_LEN];
    char                  boot_firmware[FLARE_DS_BOOT_PATH_LEN];
    char                  boot_exe[FLARE_DS_BOOT_PATH_LEN];
    uint8_t               mac_address[6];
    char                  serial_number[FLARE_DS_FACTORY_DATA_LEN];
    char                  part_number[FLARE_DS_FACTORY_DATA_LEN];
    char                  revision[FLARE_DS_FACTORY_DATA_LEN];
    char                  mod_strike[FLARE_DS_FACTORY_DATA_LEN];
    char                  boot_options[FLARE_DS_FACTORY_DATA_LEN];
    uint32_t              error_trace[FLARE_DS_ERROR_TRACE_LEN];
} flare_datasafe;

/*
 * There is one header file and a few source files because the FSBL and HBL
 * have memory limits and the code is specific to them.
 */

/*
 * Initialise the datasafe clearing the memory if the hash is not valid and
 * if valid update the count. Always set the reset value.
 */
void flare_datasafe_FsblInit(uint32_t resetReason);

/*
 * Reset if the datasafe is not valid.
 */
void flare_datasafe_FlareInit(void);

/*
 * Set the boot path and loader.
 */
void flare_datasafe_FsblSet(const char* path, const char* loader);

/*
 * Set the factory settings.
 */
void flare_datasafe_FactorySet(const uint8_t* mac,
                               const char*    serial,
                               const char*    part,
                               const char*    revision,
                               const char*    mod,
                               const char*    bootOptions);

/*
 * Set the boot mode.
 */
void flare_datasafe_FlareSet(uint32_t    bootMode,
                             const char* firmware,
                             const char* exe,
                             bool        bitfile_loaded);

/*
 * Set the factory boot flag.
 */
void flare_datasafe_SetFactoryBoot(void);

/*
 * Is the datasafe data valid ? Only available in the app.
 */
bool flare_datasafe_Valid(void);

/*
 * Update the checksum.
 */
void flare_datasafe_UpdateChecksum(void);

/*
 * Print the current status of the datasafe.
 */
void flare_datasafe_Show(void);

/*
 * Return the boot path.
 */
const char* flare_datasafe_BootPath(void);

/*
 * Return the boot firmware filename.
 */
const char* flare_datasafe_BootFirmware(void);

/*
 * Return true if the firmware has been loaded.
 */
bool flare_datasafe_FirmwareLoaded(void);

/*
 * Return true if the boot script format supports a checksum.
 */
bool flare_datasafe_BootScriptChecksum(void);

/*
 * Set the firmware state.
 */
void flare_datasafe_SetFirmwareLoaded(void);

/*
 * Return the boot mode.
 */
uint32_t flare_datasafe_BootMode(void);

/*
 * Return the run mode.
 */
uint32_t flare_datasafe_RunMode(void);

/*
 * Return true if this is a factory boot.
 */
bool flare_datasafe_BootFactory(void);

/*
 * Set the run mode.
 */
void flare_datasafe_SetRunMode(uint32_t mode);

/*
 * Request a factory boot.
 */
void flare_datasafe_RequestFactoryBoot(void);

/*
 * Clear a factory boot request.
 */
void flare_datasafe_ClearFactoryBootRequest(void);

/*
 * Has a factory boot been requested ?
 */
bool flare_datasafe_FactoryBootRequested(void);

/*
 * Save the data safe to FLASH on boot if there was a problem.
 */
void flare_datasafe_Dump(void);

/*
 * Copy the crash dump file to the web download directory on shell start.
 */
void flare_datasafe_Copy_Dump(void);

/*
 * Remove the crash dump.
 */
void flare_datasafe_Remove_Dump(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
