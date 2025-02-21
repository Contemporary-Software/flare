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
 * Data safe.
 */

#include <stdbool.h>
#include <string.h>

#include "datasafe.h"
//#include "fsbl.h"
#include "md5.h"
#include "wdog.h"

uint8_t * const flare_datasafe_hash = (uint8_t*) FLARE_DATASAFE_HASH_BASE;
flare_DataSafe * const flare_datasafe = (flare_DataSafe*) FLARE_DATASAFE_BASE;

bool
flare_DataSafe_Valid(void)
{
    uint8_t checksum[FLARE_DATASAFE_HASH_LEN];
    md5((u8*) flare_datasafe, sizeof(*flare_datasafe), checksum, 0);
    return memcmp(flare_datasafe_hash, checksum, FLARE_DATASAFE_HASH_LEN) == 0;
}

void
flare_DataSafe_FlareInit(void)
{
    if (!flare_DataSafe_Valid())
    {
      flare_DataSafe_RequestFactoryBoot();
      flare_wdog_control(true);
      while (true);
    }
    //flare_datasafe->boot.hbl_Version = ;
    md5((u8*) flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash, 0);
}

void
flare_DataSafe_FlareSet(uint32_t    bootmode,
                        const char* firmware,
                        const char* exe,
                        bool        bitfile_loaded)
{
    flare_datasafe->boot.bootmode |= bootmode & FLARE_DS_BOOTMODE_MASK;
    if (bitfile_loaded)
      flare_datasafe->boot.bootmode |= FLARE_DS_BOOTMODE_BITFILE_LD;
    memcpy(&flare_datasafe->boot.bootFirmware[0], firmware, FLARE_DATASAFE_BOOT_PATH_LEN - 1);
    memcpy(&flare_datasafe->boot.bootExe[0], exe, FLARE_DATASAFE_BOOT_PATH_LEN - 1);
    md5((u8*) flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash, 0);
}

void
flare_DataSafe_FsblSet(const char* path, const char* loader)
{
    memcpy(&flare_datasafe->boot.bootPath[0], path, FLARE_DATASAFE_BOOT_PATH_LEN - 1);
    memcpy(&flare_datasafe->boot.bootLoader[0], loader, FLARE_DATASAFE_BOOT_PATH_LEN - 1);
    md5((u8*) flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash, 0);
}

const char*
flare_DataSafe_BootPath(void)
{
    if (flare_DataSafe_Valid())
        return flare_datasafe->boot.bootPath;
    return "";
}

void
flare_DataSafe_SetFactoryBoot(void)
{
    flare_datasafe->boot.bootmode |= FLARE_DS_BOOTMODE_FACTORY;
    md5((u8*) flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash, 0);
}

bool
flare_DataSafe_BootFactory(void)
{
    if (flare_DataSafe_Valid())
        return (flare_datasafe->boot.bootmode & FLARE_DS_BOOTMODE_FACTORY) != 0;
    return false;
}

void
flare_DataSafe_RequestFactoryBoot(void)
{
    flare_datasafe->boot.bootmode |= FLARE_DS_BOOTMODE_FACTORY_REQ;
    md5((u8*) flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash, 0);
}

void
flare_DataSafe_ClearFactoryBootRequest(void)
{
    flare_datasafe->boot.bootmode &= ~FLARE_DS_BOOTMODE_FACTORY_REQ;
    md5((u8*) flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash, 0);
}
