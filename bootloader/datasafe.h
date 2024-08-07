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
 * reset. This is the only memory no touched by the secure boot of the Zynq.
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

#define FLARE_DATASAFE_HASH_LEN         (16)
#define FLARE_DATASAFE_BOOT_PATH_LEN    (64)
#define FLARE_DATASAFE_FACTORY_DATA_LEN (32)
#define FLARE_DATASAFE_ERROR_TRACE_LEN  (256)

/*
 * Place the datasafe at the bottom of the second piece of RAM. This is the
 * ps7_ram_1_S_AXI_BASEADDR as defined in the boot loader lscript.ld.
 *
 * Length is for the mmu on the Versal
 */
#define DDR_RAM_BASE             (0x00100000UL)
#define FLARE_DATASAFE_LENGTH    (0x7000000)
#define FLARE_DATASAFE_HASH_BASE (DDR_RAM_BASE)
#define FLARE_DATASAFE_BASE      (FLARE_DATASAFE_HASH_BASE + FLARE_DATASAFE_HASH_LEN)
#define FLARE_DATASAFE_TOP       (FLARE_DATASAFE_HASH_BASE + FLARE_DATASAFE_LENGTH)

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
 *   printf("Version: %" FLARE_VERSION_PRI "lx\n", bc->fsbl_Version);
 */
#define FLARE_VERSION_PRI "07"

/*
 * Boot version and configuration.
 */
typedef struct
{
    uint32_t fsbl_Version;    /* First stage boot loader */
    uint32_t hbl_Version;     /* Flare boot loader. */
    uint32_t bootMode;
    char     bootPath[FLARE_DATASAFE_BOOT_PATH_LEN];
    char     bootLoader[FLARE_DATASAFE_BOOT_PATH_LEN];
    char     bootFirmware[FLARE_DATASAFE_BOOT_PATH_LEN];
    char     bootExe[FLARE_DATASAFE_BOOT_PATH_LEN];
} flare_DSBootConfig;

/*
 * Factory Settings.
 */
typedef struct
{
    uint8_t macAddress[6];
    char    serialNumber[FLARE_DATASAFE_FACTORY_DATA_LEN];
    char    partNumber[FLARE_DATASAFE_FACTORY_DATA_LEN];
    char    revision[FLARE_DATASAFE_FACTORY_DATA_LEN];
    char    modStrike[FLARE_DATASAFE_FACTORY_DATA_LEN];
    char    bootOptions[FLARE_DATASAFE_FACTORY_DATA_LEN];
} flare_DSFactoryConfig;

/*
 * The data safe.
 *
 * The count is the number of times a reset has happened with a valid datasafe.
 * The error is one of the values list above and relates to the error trace data.
 * The reset is the reason for the reset as reported by the processor.
 * The boot configuration as it boots.
 * The factory configuration as loaded from the board.def file.
 * The error trace is the data used to report a fatal error condition.
 */
typedef struct
{
    uint16_t              count;
    uint16_t              code;
    uint32_t              reset;
    flare_DSBootConfig    boot;
    flare_DSFactoryConfig factory;
    uint32_t              errorTrace[FLARE_DATASAFE_ERROR_TRACE_LEN];
} flare_DataSafe;

/*
 * There is one header file and a few source files because the FSBL and HBL
 * have memory limits and the code is specific to them.
 */

/*
 * Initialise the datasafe clearing the memory if the hash is not valid and
 * if valid update the count. Always set the reset value.
 */
void flare_DataSafe_FsblInit(uint32_t resetReason);

/*
 * Reset if the datasafe is not valid.
 */
void flare_DataSafe_FlareInit(void);

/*
 * Set the boot path and loader.
 */
void flare_DataSafe_FsblSet(const char* path, const char* loader);

/*
 * Set the factory settings.
 */
void flare_DataSafe_FactorySet(const uint8_t* mac,
                               const char*    serial,
                               const char*    part,
                               const char*    revision,
                               const char*    mod,
                               const char*    bootOptions);

/*
 * Set the boot mode.
 */
void flare_DataSafe_FlareSet(uint32_t    bootMode,
                             const char* firmware,
                             const char* exe,
                             bool        bitfile_loaded);

/*
 * Set the factory boot flag.
 */
void flare_DataSafe_SetFactoryBoot(void);

/*
 * Is the datasafe data valid ? Only available in the app.
 */
bool flare_DataSafe_Valid(void);

/*
 * Update the checksum.
 */
void flare_DataSafe_UpdateChecksum(void);

/*
 * Print the current status of the datasafe.
 */
void flare_DataSafe_Show(void);

/*
 * Return the boot configuration. NULL if the data safe is not valid.
 */
const flare_DSBootConfig* flare_DataSafe_BootConfig(void);

/*
 * Return the factory configuration. NULL if the data safe is not valid.
 */
const flare_DSFactoryConfig* flare_DataSafe_FactoryConfig(void);

/*
 * Return the boot path.
 */
const char* flare_DataSafe_BootPath(void);

/*
 * Return the boot firmware filename.
 */
const char* flare_DataSafe_BootFirmware(void);

/*
 * Return true if the firmware has been loaded.
 */
bool flare_DataSafe_FirmwareLoaded(void);

/*
 * Return true if the boot script format supports a checksum.
 */
bool flare_DataSafe_BootScriptChecksum(void);

/*
 * Set the firmware state.
 */
void flare_DataSafe_SetFirmwareLoaded(void);

/*
 * Return the boot mode.
 */
uint32_t flare_DataSafe_BootMode(void);

/*
 * Return the run mode.
 */
uint32_t flare_DataSafe_RunMode(void);

/*
 * Return true if this is a factory boot.
 */
bool flare_DataSafe_BootFactory(void);

/*
 * Set the run mode.
 */
void flare_DataSafe_SetRunMode(uint32_t mode);

/*
 * Request a factory boot.
 */
void flare_DataSafe_RequestFactoryBoot(void);

/*
 * Clear a factory boot request.
 */
void flare_DataSafe_ClearFactoryBootRequest(void);

/*
 * Has a factory boot been requested ?
 */
bool flare_DataSafe_FactoryBootRequested(void);

/*
 * Save the data safe to FLASH on boot if there was a problem.
 */
void flare_DataSafe_Dump(void);

/*
 * Copy the crash dump file to the web download directory on shell start.
 */
void flare_DataSafe_Copy_Dump(void);

/*
 * Remove the crash dump.
 */
void flare_DataSafe_Remove_Dump(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
