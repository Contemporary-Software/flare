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
 * Flare Boot Loader.
 */

#if !defined(_FLARE_BOOT_H_)
#define _FLARE_BOOT_H_

#include <stdbool.h>

#define FLARE_BOOT_BASE       (0x00000000 + 0x00010000)
#define FLARE_BOOT_LENGTH     ((256 - 64) * 1024)

#define FLARE_TRUE 1
#ifdef FLARE_TRUE
  #define FLARE_SSBL_ENABLED
  #define FLARE_SSBL_BASE       0x10000UL
  #define FLARE_EXECUTABLE_BASE 0x00010000UL
#else
  #define FLARE_SSBL_BASE       0x8000000UL
  #define FLARE_EXECUTABLE_BASE 0x10000000UL
#endif
#define FLARE_EXECUTABLE_SIZE (128UL * 1024UL * 1024UL)

#ifdef FLARE_SSBL_ENABLED
  #define FLARE_HANDOFF_ADDRESS FLARE_SSBL_BASE
#else
  #define FLARE_HANDOFF_ADDRESS FLARE_EXECUTABLE_BASE
#endif

#endif
