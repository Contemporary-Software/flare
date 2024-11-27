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

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "datasafe.h"
#include "md5.h"

uint8_t * const flare_datasafe_hash = (uint8_t*) FLARE_DATASAFE_HASH_BASE;
flare_DataSafe * const flare_datasafe = (flare_DataSafe*) FLARE_DATASAFE_BASE;

void
flare_DataSafe_FsblInit(uint32_t resetReason)
{
    uint8_t checksum[FLARE_DATASAFE_HASH_LEN];
    md5(flare_datasafe, sizeof(*flare_datasafe), checksum);
    if (memcmp(flare_datasafe_hash, checksum, FLARE_DATASAFE_HASH_LEN) != 0)
        memset(flare_datasafe, 0, sizeof(*flare_datasafe));
    ++flare_datasafe->count;
    flare_datasafe->reset = resetReason;
    flare_datasafe->boot.bootMode =
      flare_datasafe->boot.bootMode & FLARE_DS_BOOTMODE_REQ_MASK;
    flare_datasafe->boot.bootMode |= FLARE_DS_BOOTMODE_BS_CHECKSUM;
    flare_datasafe->boot.fsbl_Version = 0;
    memset(&flare_datasafe->boot.bootFirmware[0], 0, sizeof(flare_datasafe->boot.bootFirmware));
    memset(&flare_datasafe->boot.bootExe[0], 0, sizeof(flare_datasafe->boot.bootExe));
    md5(flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash);
    printf(" Reset Count: %d\n  Reset Code: 0x%08" PRIx32 "\n   Boot Mode: 0x%08" PRIx32 "\n",
           flare_datasafe->count, resetReason, flare_datasafe->boot.bootMode);
}

bool
flare_DataSafe_Valid(void)
{
    uint8_t checksum[FLARE_DATASAFE_HASH_LEN];
    md5(flare_datasafe, sizeof(*flare_datasafe), checksum);
    return memcmp(flare_datasafe_hash, checksum, FLARE_DATASAFE_HASH_LEN) == 0;
}

void
flare_DataSafe_FsblSet(const char* path, const char* loader)
{
    memcpy(&flare_datasafe->boot.bootPath[0], path, FLARE_DATASAFE_BOOT_PATH_LEN - 1);
    memcpy(&flare_datasafe->boot.bootLoader[0], loader, FLARE_DATASAFE_BOOT_PATH_LEN - 1);
    md5(flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash);
}

void
flare_DataSafe_FlareSet(uint32_t    bootMode,
                        const char* firmware,
                        const char* exe,
                        bool        bitfile_loaded)
{
    flare_datasafe->boot.bootMode |= bootMode & FLARE_DS_BOOTMODE_MASK;
    if (bitfile_loaded)
      flare_datasafe->boot.bootMode |= FLARE_DS_BOOTMODE_BITFILE_LD;
    memcpy(&flare_datasafe->boot.bootFirmware[0], firmware, FLARE_DATASAFE_BOOT_PATH_LEN - 1);
    memcpy(&flare_datasafe->boot.bootExe[0], exe, FLARE_DATASAFE_BOOT_PATH_LEN - 1);
    md5(flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash);
}

void
flare_DataSafe_FactorySet(const uint8_t* mac,
                          const char*    serial,
                          const char*    part,
                          const char*    revision,
                          const char*    mod,
                          const char*    bootOptions)
{
    flare_DSFactoryConfig* fc = &flare_datasafe->factory;
    size_t                 o;
    for (o = 0; o < sizeof(fc->macAddress); ++o)
        fc->macAddress[o] = mac[o];
    memcpy(&fc->serialNumber[0], serial, FLARE_DATASAFE_FACTORY_DATA_LEN - 1);
    memcpy(&fc->partNumber[0], part, FLARE_DATASAFE_FACTORY_DATA_LEN - 1);
    memcpy(&fc->revision[0], revision, FLARE_DATASAFE_FACTORY_DATA_LEN - 1);
    memcpy(&fc->modStrike[0], mod, FLARE_DATASAFE_FACTORY_DATA_LEN - 1);
    memcpy(&fc->bootOptions[0], bootOptions, FLARE_DATASAFE_FACTORY_DATA_LEN - 1);
    md5(flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash);
}

const flare_DSFactoryConfig*
flare_DataSafe_FactoryConfig(void)
{
    return flare_DataSafe_Valid() ? &flare_datasafe->factory : NULL;
}

void
flare_DataSafe_SetFactoryBoot(void)
{
    flare_datasafe->boot.bootMode |= FLARE_DS_BOOTMODE_FACTORY;
    md5(flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash);
}

void
flare_DataSafe_RequestFactoryBoot(void)
{
    flare_datasafe->boot.bootMode |= FLARE_DS_BOOTMODE_FACTORY_REQ;
    md5(flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash);
}

bool
flare_DataSafe_FactoryBootRequested(void)
{
    if (flare_DataSafe_Valid())
        return (flare_datasafe->boot.bootMode & FLARE_DS_BOOTMODE_FACTORY_REQ) != 0;
    return false;
}

void
flare_DataSafe_ClearFactoryBootRequest(void)
{
    flare_datasafe->boot.bootMode &= ~FLARE_DS_BOOTMODE_FACTORY_REQ;
    md5(flare_datasafe, sizeof(*flare_datasafe), flare_datasafe_hash);
}
