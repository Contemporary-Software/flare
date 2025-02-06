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
 * FSBL factory configruation.
 */

#include <stdio.h>

#include "boot-buffer.h"
#include "boot-filesystem.h"
#include "datasafe.h"
#include "factory.h"
#include "flash.h"
#include "md5.h"

void
FactoryConfigLoad(void)
{
    char*       buffer;
    uint8_t     checksum[FACTORY_CHECKSUM_SIZE];
    flash_error fe;
    int         i;

    if (flare_GetReadBufferSize() < FACTORY_TABLE_SIZE)
    {
        printf("error: factory table size too large\n");
        return;
    }

    buffer = flare_GetReadBuffer();

    /*
     * Assumes the file has been mounted and so the flash driver initialised.
     */
    fe = flash_Read(flash_DeviceSize() - FACTORY_TABLE_FLASH_SIZE,
                   buffer, FACTORY_TABLE_SIZE);
    if (fe != FLASH_NO_ERROR)
    {
        printf("error: factory table read: %d\n", fe);
        return;
    }

    md5((uint8_t*) (buffer + FACTORY_CHECKSUM_SIZE),
        FACTORY_TABLE_SIZE - FACTORY_CHECKSUM_SIZE,
        checksum);

    for (i = 0; i < FACTORY_CHECKSUM_SIZE; ++i)
    {
        if (buffer[i] != checksum[i])
        {
            printf("error: invalid factory settings checksum\n");
            return;
        }
    }

    flare_datasafe_FactorySet((uint8_t*) &buffer[FACTORY_MAC_ADDR_OFF],
                              &buffer[FACTORY_SERIAL_NUM_OFF],
                              &buffer[FACTORY_PART_NUM_OFF],
                              &buffer[FACTORY_REVISION_OFF],
                              &buffer[FACTORY_MODSTRIKE_OFF],
                              &buffer[FACTORY_BOOT_OPTIONS_OFF]);
}
