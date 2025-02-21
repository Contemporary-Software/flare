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
 * Flare Boot Loader File System Interface.
 */

#if !defined(BOOT_BUFFER_H)
#define BOOT_BUFFER_H

#include <stddef.h>

/*
 * Get the pointer to the global buffer.
 */
char* flare_get_read_buffer(void);

/*
 * Get the pointer to the global buffer.
 */
size_t flare_get_read_bufferSize(void);

#endif
