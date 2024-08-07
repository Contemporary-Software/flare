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
 * FSBL Support for the Flare board boot.
 */

#include <inttypes.h>
#include <stdio.h>

#include "boot-filesystem.h"
#include "boot-load.h"
#include "boot-script.h"
#include "cache.h"
#include "console.h"
#include "datasafe.h"
#include "flash-map.h"
#include "flash.h"
#include "flare-boot.h"
#include "leds.h"
#include "md5.h"
#include "reset.h"
#include "wdog.h"
#include "board-handoff.h"
#include "board-timer.h"
#include "board.h"

void DataSafe_Print(void);

static bool
flash_Check(void)
{
    uint32_t    flash_size = 0;
    uint32_t    manufactureCode;
    uint32_t    memIfaceType;
    uint32_t    density;
    const char* label = NULL;
    flash_error fe;

    fe = flash_ReadId(&manufactureCode,
                      &memIfaceType,
                      &density);
    if (fe != FLASH_NO_ERROR)
    {
        printf("flash: device id read failed: %d\n", fe);
        return false;
    }

    if (manufactureCode == 1)
    {
        if ((memIfaceType == 0x20) && (density == 0x18))
        {
            label = "S25FL128P_64K (16M)";
            flash_size = 16 * 1024 * 1024;
        }
        else if ((memIfaceType == 0x02) && (density == 0x20))
        {
            label = "S25FL512S_256K (64M)";
            flash_size = 64 * 1024 * 1024;
        }
    }
    else if (manufactureCode == 0x20)
    {
        if (density == 0x21)
        {
            label = "2x N25Q00A (128MiB) in parallel (256MiB total)";
            flash_size = 0x8000000UL * 2UL; /* 1 Gib on each flash in parallel */
        }
        else if (density == 0x22)
        {
            label = "2x mt25qu02g (256MiB) in parallel (512MiB total)";
            flash_size = 0x10000000UL * 2UL; /* 2Gib on each flash in parallel */
        }
    }

    if (label == NULL)
    {
        printf("error: flash: unknown device: 0x%02" PRIx32 "/0x%02" PRIx32 "/0x%02" PRIx32 "\n",
               manufactureCode, memIfaceType, density);
        return false;
    }

    printf("       Flash: %s\n", label);

    if (flash_size != FLARE_FLASH_SIZE)
        printf("error: flash: size mismatch\n");

    return true;
}

int
main(void)
{
    const char* fb_reason = "";
    boot_Script script;
    bool        rc;

    wdog_control(false);

    board_timer_reset();

    led_normal();

    if (flash_Check())
    {
        rc = flare_FilesystemMount(false);
        if (rc == 0)
        {
            /*
             * The SSBL is specific to a release and these releases have a
             * fixed boot script.
             */
            if (rc)
            {
                rc = BootLoad(&script, FLARE_EXECUTABLE_BASE, FLARE_EXECUTABLE_SIZE);
                if (rc)
                {
                    flare_DataSafe_FlareSet(QSPI_MODE, "", script.executable, 0);
                    led_execute();
                    wdog_control(true);
                    out_flush();
                    cache_disable();
                    board_handoff_exit(FLARE_EXECUTABLE_BASE);
                }
                else
                {
                    fb_reason = "Invalid executable";
                }
            }
            else
            {
                fb_reason = "Invalid boot script";
            }
        }
        else
        {
            fb_reason = "Flash filesystem failure";
        }
    }
    else
    {
        fb_reason = "Flash device failure";
    }

    printf("Factory Boot: %s\n", fb_reason);
    led_failure();
    flare_DataSafe_RequestFactoryBoot();
    printf("Reset ..... ");
    out_flush();
    reset();
    return 0;
}
