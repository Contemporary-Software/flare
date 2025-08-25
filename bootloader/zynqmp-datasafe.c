/*
 * Copyright 2025 Contemporary Software
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
 * Data safe hardware specific.
 */

#include <hw-datasafe.h>

int board_bootmode() {
    return FLARE_DS_BOOTMODE_ERROR;
}

void flare_datasafe_hw_init(flare_datasafe* ds) {
    ds->bootmode &= ~FLARE_DS_BOOTMODE_HW_MASK;
    ds->bootmode |= board_bootmode();

    ds->reset &= ~FLARE_DS_RESET_MASK;
    ds->reset |= FLARE_DS_RESET_ERR;
}
