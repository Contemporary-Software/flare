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
 * Flare FSBL Loader.
 */

#include <inttypes.h>
#include <stdio.h>

#include "boot-factory-config.h"
#include "boot-filesystem.h"
#include "boot-load.h"
#include "boot-script.h"
#include "cache.h"
#include "console.h"
#include "datasafe.h"
#include "factory-boot.h"
#include "flash-map.h"
#include "flash.h"
#include "flare-boot.h"
#include "leds.h"
#include "md5.h"
#include "power-switch.h"
#include "reset.h"
#include "sleep.h"
#include "wdog.h"
#include "board-handoff.h"
#include "board-io.h"
#include "board-slcr.h"
#include "board-timer.h"
#include "board.h"

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
            label = "S25FL128P_64K (16MiB)";
            flash_size = 16 * 1024 * 1024;
        }
        else if ((memIfaceType == 0x02) && (density == 0x20))
        {
            label = "S25FL512S_256K (64MiB)";
            flash_size = 64 * 1024 * 1024;
        }
    }
    else if (manufactureCode == 0x20)
    {
        if (density == 0x19)
        {
            label = "1x N25Q256A";
            flash_size = 0x2000000; /* 256 Mb on one flash */
        }
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

static void
FlareBootBoardRequests(void)
{
    if (flare_DataSafe_FactoryBootRequested())
        factory_boot("Requested");
}

static void
flare_JtagBoot(uint32_t bootMode)
{
    if (bootMode != JTAG_MODE)
    {
        const flare_DSFactoryConfig* fc = flare_DataSafe_FactoryConfig();
        if (fc)
        {
            const char* const jtag_label = "jtag";
            const char*       jtag = jtag_label;
            const char*       bopts = &fc->bootOptions[0];
            int               count = sizeof(fc->bootOptions) - 1;
            while (count && *bopts)
            {
                char c = *bopts;
                if ((c >= 'A') && (c <= 'Z'))
                    c = 'a' + (c - 'A');
                if (*jtag == c)
                {
                    ++jtag;
                    if (*jtag == '\0')
                    {
                        bootMode = JTAG_MODE;
                        break;
                    }
                }
                else
                {
                    jtag = jtag_label;
                }
                ++bopts;
                --count;
            }
        }
    }

    if (bootMode == JTAG_MODE)
    {
        flare_DataSafe_FsblSet("/jtag", "none");
        flare_DataSafe_FlareSet(JTAG_MODE, "", "none", false);
        printf("JTAG wait .. ");
        out_flush();
        cache_disable();
        board_handoff_jtag_exit();
    }
}

int
main(void)
{
    const char* fb_reason = "";
    boot_Script script;
    uint32_t    bootMode = board_reg_read(BOOT_MODE_REG) & BOOT_MODES_MASK;
    uint32_t    reset_status = board_reg_read(RESET_REASON_REG);
    bool        rc;
    int         i;
    uint8_t     header[IMAGE_HEADER_TOTAL(3)];
    uint32_t    entry_point = 0;

    board_hardware_setup();
    board_timer_reset();

    printf("\nFlare Apache 2.0 Licensed FSBL\n");

    cache_enable_dcache();
    cache_enable_icache();

    wdog_init();
    wdog_control(false);

    led_init();
    led_normal();

    flare_DataSafe_FsblInit(reset_status);

    if (flash_Check())
    {
        FactoryConfigLoad();
        //flare_JtagBoot(bootMode);
        FlareBootBoardRequests();

        rc = flare_FilesystemMount(true);
        if (rc == 0)
        {
            if (flare_PowerOnPressed())
                factory_boot("Power button");

            rc = BootScriptLoad("/flare-0", &script);
            if (rc)
            {
                rc = load_exe(&script, &entry_point);
                if (rc)
                {
                    flare_DataSafe_FsblSet(script.path, script.executable);
                    wdog_control(true);
                    cache_flush_invalidate();
                    out_flush();
                    board_handoff_exit(entry_point);
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

    led_failure();
    factory_boot(fb_reason);
    printf("Reset ..... ");
    out_flush();
    reset();
    return 0;
}
