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

#define FLARE_EXECUTABLE_SIZE (128UL * 1024UL * 1024UL)

#define FLARE_IMAGE_STAGE_ADDR 0x30000000

#define FLARE_HANDOFF_ADDRESS FLARE_EXECUTABLE_BASE

#endif
