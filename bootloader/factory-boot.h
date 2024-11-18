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
 * Factory Boot support.
 */

#if !defined(FACTORY_BOOT_H)
#define FACTORY_BOOT_H

#include <stdbool.h>

/*
 * Factory boot.
 */
void FactoryBoot(const char* why);

/*
 * Platform specific factory boot
 */
void platform_factory_booter(uint8_t* header, size_t header_size);

/*
 * Load factory image
 */
bool factory_load_image(const char* label,
                 uint32_t    base,
                 size_t      size,
                 uint8_t*    loadTo,
                 size_t*     loadSize,
                 uint8_t*    cs,
                 bool        compressed);

uint32_t
FactoryLoadImage_Get32(uint8_t* table, uint32_t index, uint32_t offset);

uint8_t
FactoryLoadImage_Get8(uint8_t* table, uint32_t index, uint32_t offset);
#endif
