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

#include <datasafe.h>
#include <crc/crc.h>

void
flare_datasafe_init(uint32_t resetReason)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    CRC32 crc = 0;
    crc32_update(&crc, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
    if (crc != datasafe->crc32) {
        memset(datasafe, 0, sizeof(flare_datasafe));
    }
    datasafe->length = sizeof(flare_datasafe) - 2 * sizeof(uint32_t);
    datasafe->format = FLARE_DATASAFE_FORMAT;
    ++datasafe->count;
    datasafe->reset = resetReason;
    datasafe->bootmode = datasafe->bootmode & FLARE_DS_BOOTMODE_REQ_MASK;
    datasafe->bootmode = datasafe->bootmode | FLARE_DS_BOOTMODE_BS_CHECKSUM;
    datasafe->flare_version = 0;
    memset(&datasafe->boot_firmware[0], 0, sizeof(datasafe->boot_firmware));
    memset(&datasafe->boot_exe[0], 0, sizeof(datasafe->boot_exe));
    datasafe->crc32 = 0;
    crc32_update(&datasafe->crc32, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
    printf(" Reset Count: %d\n  Reset Code: 0x%08x\n   Boot Mode: 0x%08x\n",
           datasafe->count, resetReason, datasafe->bootmode);
}

bool
flare_datasafe_valid(void)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    CRC32 crc = 0;
    crc32_update(&crc, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
    return crc == datasafe->crc32;
}

void
flare_datasafe_set_boot(const char* path, const char* loader)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    memcpy(&datasafe->boot_path[0], path, FLARE_DS_BOOT_PATH_LEN - 1);
    memcpy(&datasafe->boot_loader[0], loader, FLARE_DS_BOOT_PATH_LEN - 1);
    datasafe->crc32 = 0;
    crc32_update(&datasafe->crc32, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
}

void
flare_datasafe_set_bootmode(uint32_t    bootmode,
                        const char* firmware,
                        const char* exe,
                        bool        bitfile_loaded)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    datasafe->bootmode |= bootmode & FLARE_DS_BOOTMODE_MASK;
    if (bitfile_loaded) {
        datasafe->bootmode |= FLARE_DS_BOOTMODE_BITFILE_LD;
    }
    memcpy(&datasafe->boot_firmware[0], firmware, FLARE_DS_BOOT_PATH_LEN - 1);
    memcpy(&datasafe->boot_exe[0], exe, FLARE_DS_BOOT_PATH_LEN - 1);
    datasafe->crc32 = 0;
    crc32_update(&datasafe->crc32, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
}

void
flare_datasafe_factory_set(const uint8_t* mac,
                           const char*    serial,
                           const char*    part,
                           const char*    revision,
                           const char*    mod,
                           const char*    bootOptions)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    size_t o;
    for (o = 0; o < sizeof(datasafe->mac_address); ++o)
        datasafe->mac_address[o] = mac[o];
    memcpy(&datasafe->serial_number[0], serial, FLARE_DS_FACTORY_DATA_LEN - 1);
    memcpy(&datasafe->part_number[0], part, FLARE_DS_FACTORY_DATA_LEN - 1);
    memcpy(&datasafe->revision[0], revision, FLARE_DS_FACTORY_DATA_LEN - 1);
    memcpy(&datasafe->mod_strike[0], mod, FLARE_DS_FACTORY_DATA_LEN - 1);
    memcpy(&datasafe->boot_options[0], bootOptions, FLARE_DS_FACTORY_DATA_LEN - 1);
    datasafe->crc32 = 0;
    crc32_update(&datasafe->crc32, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
}

void
flare_datasafe_set_factory_boot(void)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    datasafe->bootmode |= FLARE_DS_BOOTMODE_FACTORY;
    datasafe->crc32 = 0;
    crc32_update(&datasafe->crc32, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
}

void
flare_datasafe_request_factory_boot(void)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    datasafe->bootmode |= FLARE_DS_BOOTMODE_FACTORY_REQ;
    datasafe->crc32 = 0;
    crc32_update(&datasafe->crc32, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
}

bool
flare_datasafe_factory_boot_requested(void)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    if (flare_datasafe_valid()) {
        return (datasafe->bootmode & FLARE_DS_BOOTMODE_FACTORY_REQ) != 0;
    }
    return false;
}

void
flare_datasafe_clear_factory_boot_request(void)
{
    flare_datasafe *datasafe = (flare_datasafe*) FLARE_DS_BASE;
    datasafe->bootmode &= ~FLARE_DS_BOOTMODE_FACTORY_REQ;
    datasafe->crc32 = 0;
    crc32_update(&datasafe->crc32, FLARE_DS_CRC_BASE, FLARE_DS_CRC_LEN);
}
