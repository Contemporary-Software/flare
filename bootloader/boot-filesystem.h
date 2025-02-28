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

#if !defined(_BOOT_FILESYSTEM_H_)
#define _BOOT_FILESYSTEM_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Mount the file system.
 */
int flare_filesystem_mount(bool setup_cache);

/*
 * Read the file.
 */
int flare_read_file(const char* name, void* const buffer, uint32_t* size);

/*
 * Change directory.
 */
int flare_chdir(const char* path);

#endif
