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
 */

#include <stdio.h>

#include "boot-filesystem.h"
#include "boot-load.h"
#include "md5.h"

bool
BootLoad(const boot_Script* const script, uintptr_t base, uint32_t length)
{
    uint8_t           checksum[sizeof(script->checksum)];
    bool              csum_valid = BootScriptChecksumValid(script);
    const char* const error = "\b: error:";
    size_t            i;
    int               rc;

    printf("  Executable: %s/%s ", script->path, script->executable);

    rc = flare_Chdir(script->path);
    if (rc != 0)
    {
        printf("%s chdir: %d\n", error, rc);
        return false;
    }

    rc = flare_ReadFile(script->executable, (char*) base, &length);
    if (rc != 0)
    {
        printf("%s read: %d\n", error, rc);
        return false;
    }

    md5((const void*) base, length, checksum);

    printf("%s(", csum_valid ? "" : "[NOT CHECKED] ");
    for (i = 0; i < sizeof(script->checksum); ++i)
        printf("%02x", checksum[i]);
    printf(") ");

    if (csum_valid)
    {
        for (i = 0; i < sizeof(script->checksum); ++i)
        {
            if (script->checksum[i] != checksum[i])
            {
                printf("%s invalid checksum\n", error);
                return false;
            }
        }
    }

    printf("\n");

    return true;
}
