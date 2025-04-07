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
#include "flare-build-id.h"
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
flash_check(void)
{
    uint32_t    flash_size = 0;
    uint32_t    manufacture_code;
    uint32_t    mem_iface_type;
    uint32_t    density;
    const char* label = NULL;
    flash_error fe;

    fe = flash_read_id(&manufacture_code,
                      &mem_iface_type,
                      &density);
    if (fe != FLASH_NO_ERROR)
    {
        printf("flash: device id read failed: %d\n", fe);
        return false;
    }

    if (manufacture_code == 1)
    {
        if ((mem_iface_type == 0x20) && (density == 0x18))
        {
            label = "S25FL128P_64K (16MiB)";
            flash_size = 16 * 1024 * 1024;
        }
        else if ((mem_iface_type == 0x02) && (density == 0x20))
        {
            label = "S25FL512S_256K (64MiB)";
            flash_size = 64 * 1024 * 1024;
        }
    }
    else if (manufacture_code == 0x20)
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
               manufacture_code, mem_iface_type, density);
        return false;
    }

    printf("       Flash: %s\n", label);

    if (flash_size != FLARE_FLASH_SIZE)
        printf("error: flash: size mismatch\n");

    return true;
}

static void
flare_boot_board_requests(void)
{
    if (flare_datasafe_factory_boot_requested()) {
        factory_boot("Requested");
    }
}

static void
flare_jtag_boot(uint32_t bootmode)
{
    /*
    if (bootmode != JTAG_MODE)
    {
        const flare_DSFactoryConfig* fc = flare_datasafe_FactoryConfig();
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
                        bootmode = JTAG_MODE;
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
    */

    if (bootmode == JTAG_MODE)
    {
        flare_datasafe_set_boot("/jtag", "none");
        flare_datasafe_set_bootmode(JTAG_MODE, "", "none", false);
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
    boot_script script;
    uint32_t    bootmode = board_reg_read(BOOT_MODE_REG) & BOOT_MODES_MASK;
    uint32_t    reset_status = board_reg_read(RESET_REASON_REG);
    bool        rc;
    int         i;
    uint8_t     header[IMAGE_HEADER_TOTAL(3)];
    uint32_t    entry_point = 0;

    board_hardware_setup();
    board_timer_reset();

    printf("\nFlare Apache 2.0 Licensed FSBL\n");
    printf("    Build ID: %08x\n", flare_build_id());

    cache_enable_dcache();
    cache_enable_icache();

    wdog_init();
    wdog_control(false);

    led_init();
    led_normal();

    flare_datasafe_init(reset_status);

    if (flash_check())
    {
        factory_config_load();
        /* flare_jtag_boot(bootmode); */
        flare_boot_board_requests();

        rc = flare_filesystem_mount(true);
        if (rc == 0)
        {
            if (flare_power_on_pressed())
                factory_boot("Power button");

            rc = boot_script_load("/flare-0", &script);
            if (rc)
            {
                rc = load_exe(&script, &entry_point);
                if (rc)
                {
                    flare_datasafe_set_boot(script.path, script.executable);
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
