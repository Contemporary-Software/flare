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
 * Boot Load.
 *
 * Load the image into memory and checkum it is required.
 */

#if !defined(BOOT_LOAD_H)
#define BOOT_LOAD_H

#include <stdbool.h>
#include <stdint.h>

#include "boot-script.h"

/*
 * Load the image into the memory at base until the length.
 */
bool BootLoad(const boot_Script* const script, uintptr_t base, uint32_t length);

#endif
