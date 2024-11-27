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
 * Boot Script.
 *
 * The boot script is a file 3 lines in length. The first line is the boot
 * path, the second line is the executable file name and optionally a MD5 hash
 * of the executable and the third line is the MD5 hash of the first two lines
 * without white space line terminators.
 *
 * The boot path is the default path or configuration directory for the
 * software that is booting. The boot loader can be relative to the boot path
 * or an absolute path.
 *
 */

#if !defined(BOOT_SCRIPT_H)
#define BOOT_SCRIPT_H

#include <stdbool.h>
#include <stdint.h>

/*
 * The maximum path or file name.
 */
#define BOOT_SCRIPT_MAX_PATH (64)

/*
 * Checksum size (CRC32)
 */
#define BOOT_SCRIPT_CSUM_SIZE (8)

/*
 * The results of reading the boot script.
 */
typedef struct boot_Script
{
    char    path[BOOT_SCRIPT_MAX_PATH];         /* installation path */
    char    executable[BOOT_SCRIPT_MAX_PATH];   /* executable name */
    uint8_t checksum[BOOT_SCRIPT_CSUM_SIZE];    /* executable MD5 checksum */
} boot_Script;


/*
 * Load the boot script.
 */
bool BootScriptLoad(const char* name, boot_Script* bs);

/*
 * Check if the checksum is valid, ie any byte is not zero.
 */
bool BootScriptChecksumValid(const boot_Script* const bs);

#endif
