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

#include <board-handoff.h>
#include <board.h>
#include <boot-factory-config.h>
#include <boot-load.h>
#include <boot-script.h>
#include <cache.h>
#include <datasafe.h>
#include <factory-boot.h>
#include <flare-boot.h>
#include <flare-build-id.h>
#include <flash-map.h>
#include <fs/boot-filesystem.h>
#include <reset.h>

#include <fatfs/ff.h>
#include <flash/flash.h>
#include <io/board-io.h>
#include <leds/leds.h>
#include <power-switch/power-switch.h>
#include <sdhci/sdhci.h>
#include <slcr/board-slcr.h>
#include <timer/board-timer.h>
#include <timer/sleep.h>
#include <uart/console.h>
#include <wdog/wdog.h>

static void
flare_boot_board_requests(void)
{
    if (flare_datasafe_factory_boot_requested()) {
        factory_boot("Requested");
    }
}

static void qspi_boot(const char** fb_reason) {
    bool        rc;
    boot_script script;
    const char* label;
    uint32_t    entry_point = 0;

    flash_error err = flash_open(&label);
    if (err == FLASH_NO_ERROR) {
        printf("       Flash: %s\n", label);
        factory_config_load();
        flare_boot_board_requests();

        rc = flare_filesystem_mount(FILESYSTEM_QSPI_JFFS2);
        if (rc == 0) {
            if (flare_power_on_pressed())
                factory_boot("Power button");

            rc = boot_script_load(FILESYSTEM_QSPI_JFFS2, "/flare-0", &script);
            if (rc) {
                rc = load_exe(&script, &entry_point);
                if (rc) {
                    flare_datasafe_set_boot(script.path, script.executable);
                    wdog_control(true);
                    cache_flush_invalidate();
                    console_flush();
                    board_handoff_exit(entry_point);
                } else {
                    *fb_reason = "Invalid executable";
                }
            } else {
                *fb_reason = "Invalid boot script";
            }
        } else {
            *fb_reason = "Flash filesystem failure";
        }
    }
    else
    {
        *fb_reason = "Flash device failure";
    }
}

void sdhci_boot(const char** fb_reason) {
    boot_script script;
    int rc;
    uint32_t entry_point = 0;

    rc = sdhci_open();
    if (rc == 0) {
        rc = flare_filesystem_mount(FILESYSTEM_SD_FATFS);
        if (rc == 0) {
            rc = boot_script_load(FILESYSTEM_SD_FATFS, "/flare-0", &script);
            if (rc) {
                rc = load_exe(&script, &entry_point);
                if (rc) {
                    flare_datasafe_set_boot(script.path, script.executable);
                    cache_flush_invalidate();
                    console_flush();
                    board_handoff_exit(entry_point);
                } else {
                    *fb_reason = "Invalid executable";
                }
            } else {
                printf("Boot script invalid: %d\n", rc);
                *fb_reason = "Invalid boot script";
            }
        } else {
            printf("FATFS mount failed: %d\n", rc);
            *fb_reason = "SD card filesystem failure";
        }
    } else {
        printf("SD card error: %d\n", rc);
        *fb_reason = "SD card device failure";
    }
}

static void jtag_boot(const char** fb_reason) {
    const uint32_t    wait_seconds = 2;
    volatile uint32_t seconds = 0;
    bool              pressed = false;

    printf("   JTAG boot: booting from SD in %1d   (^c for QSPI)"
        "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",
        wait_seconds);

    while (seconds++ < wait_seconds)
    {
      volatile uint32_t msecs = 0;

      printf("\b\b%1d ", wait_seconds - seconds + 1);

      while (msecs++ < 1000)
      {
        if (inbyte_available())
        {
            uint8_t ch = inbyte();
            if (ch == '\x3') {
                pressed = true;
                break;
            }
        }
        usleep(1000);
      }
    }

    printf("    \b\b\b\b\b\b\b\b\b\b\b\b%s                     \n",
        pressed ? "QSPI" : "SD");

    if (pressed) {
        qspi_boot(fb_reason);
    } else {
        sdhci_boot(fb_reason);
    }
}

int main(void) {
    const char* fb_reason = NULL;
    int bootmode = board_bootmode();

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

    flare_datasafe_init();

    if (bootmode == FLARE_DS_BOOTMODE_QSPI) {
        qspi_boot(&fb_reason);
    } else if (bootmode == FLARE_DS_BOOTMODE_SD_CARD) {
        sdhci_boot(&fb_reason);
    } else if (bootmode == FLARE_DS_BOOTMODE_JTAG) {
        jtag_boot(&fb_reason);
    } else {
        fb_reason = "Invalid bootmode";
    }

    led_failure();
    factory_boot(fb_reason);
    printf("Reset ..... ");
    console_flush();
    reset();
    return 0;
}
