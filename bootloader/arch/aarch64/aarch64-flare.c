/*
 * Copyright 2026 Contemporary Software
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

#include <stdio.h>

#include <flare-build-id.h>

#include "aarch64-atf.h"

void aarch64_print_flare_id() {
    printf("\nFlare Apache 2.0 Licensed FSBL\n");
    printf("    Build ID: %s\n", flare_build_id());
    printf("    Handoff to ARM Trusted Firmware (BL31) at 0x%x\n\n",
        ATF_DEFAULT_ENTRY);

    /* ATF removes 64 characters and we don't want our messages to disappear */
    for (int i = 0; i < 64; i++) {
      printf(" ");
    }
}
