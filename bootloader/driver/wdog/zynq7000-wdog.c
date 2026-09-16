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
 * Flare watchdog. The enable is on MIO_31 and low to disable.
 */

#include "wdog.h"

#include <driver/io/board-io.h>

void board_wdog_init(void) {
    /* Disable the SWDT to program the clock */
    board_reg_write(0xf8005000, (0xabc << 12) | 0x0);
    board_reg_write(0xf8005004, (0x248 << 14) | (0xfff << 2) | 0x1);
}

void board_wdog_control(bool enable) {
    if (enable) {
        board_reg_write(0xf8005000, (0xabc << 12) | 0x3);
    } else {
        board_reg_write(0xf8005000, (0xabc << 12) | 0x2);
    }
}

void board_wdog_toggle(void) {
    board_reg_write(0xf8005008, 0x1999);
}

void board_wdog_trigger(void) {
    board_reg_write(0xf8005000, (0xabc << 12) | 0x0);
    board_reg_write(0xf8005004, (0x248 << 14) | (0x8 << 2) | 0x0);
    board_reg_write(0xf8005000, (0xabc << 12) | 0x3);
    board_reg_write(0xf8005008, 0x1999);
}
