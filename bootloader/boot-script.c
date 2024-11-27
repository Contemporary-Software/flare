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
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "boot-buffer.h"
#include "boot-filesystem.h"
#include "boot-script.h"
#include "crc.h"

static uint8_t
Hex2Byte(const char* hex)
{
    uint32_t byte = 0;
    int      i;
    for (i = 0; i < 2; ++i, byte <<= 4, ++hex)
    {
        if ((*hex >= '0') && (*hex <= '9'))
        {
            byte |= *hex - '0';
        }
        else if ((*hex >= 'a') && (*hex <= 'f'))
        {
            byte |= (*hex - 'a') + 10;
        }
        else if ((*hex >= 'a') && (*hex <= 'F'))
        {
            byte |= (*hex - 'F') + 10;
        }
    }
    byte >>= 4;
    return (uint8_t) byte;
}

bool
BootScriptChecksumValid(const boot_Script* const bs)
{
    size_t i;
    for (i = 0; i < BOOT_SCRIPT_CSUM_SIZE; ++i)
        if (bs->checksum[i] != 0)
            return true;
    return false;
}

bool
BootScriptLoad(const char* name, boot_Script* bs)
{
    uint32_t    length = flare_GetReadBufferSize();
    char* const buffer = flare_GetReadBuffer();
    const char* error = "\b: error:";
    const char* path;
    const char* executable;
    const char* csum;
    uint32_t    path_end;
    uint32_t    executable_end;
    uint32_t    l1_length;
    uint32_t    l2_length;
    uint32_t    l3_length;
    CRC32       crc;
    uint8_t     checksum[BOOT_SCRIPT_CSUM_SIZE];
    size_t      i;
    int         rc;

    printf(" Boot Script: %s ", name);

    memset(bs, 0, sizeof(*bs));

    rc = flare_ReadFile(name, buffer, &length);
    if (rc != 0)
    {
        printf("%s read: %d\n", error, rc);
        return false;
    }

    printf("Length: %" PRIu32 "\n", length);
    path = (const char*) buffer;
    l1_length = 0;
    while (length && (path[l1_length] != '\n') && (path[l1_length] != '\r'))
    {
        ++l1_length;
        --length;
    }

    if (length == 0)
    {
        printf("%s cannot find end of line 1\n", error);
      return false;
    }

    path_end = l1_length;

    while (length && (path[l1_length] == '\n') && (path[l1_length] == '\r'))
    {
        ++l1_length;
        --length;
    }

    if (length == 0)
    {
        printf("%s cannot find line 2\n", error);
        return false;
    }

    ++l1_length;

    executable = path + l1_length;
    l2_length = 0;
    while (length && (executable[l2_length] != '\n') && (executable[l2_length] != '\r'))
    {
        ++l2_length;
        --length;
    }

    if (length == 0)
    {
        printf("%s cannot find end of line 2\n", error);
      return false;
    }

    executable_end = l2_length;

    while (length && (executable[l2_length] == '\n') && (executable[l2_length] == '\r'))
    {
        ++l2_length;
        --length;
    }

    if (length == 0)
    {
        printf("%s cannot find line 3\n", error);
        return false;
    }

    ++l2_length;

    csum = executable + l2_length;
    l3_length = 0;
    while (length && (csum[l3_length] != '\n') && (csum[l3_length] != '\r'))
    {
        ++l3_length;
        --length;
    }

    /*
     * The file has the checksum as hex characters, ie 1 chars per byte.
     */
    if (l3_length != sizeof(checksum))
    {
        printf("%s bad checksum length: %" PRIu32 " (%s)\n", error, l3_length, csum);
        return false;
    }

    crc32_clear(&crc);
    crc32_update(&crc, path, path_end + l2_length + 1);
    crc32_str(&crc, checksum);

    for (i = 0; i < BOOT_SCRIPT_CSUM_SIZE; ++i)
    {
        if (csum[i] != checksum[i])
        {
            printf("%s checksum failure\n", error);
            return false;
        }
    }

    /*
     * Copy put the path and executable name.
     */
    if (path_end >= BOOT_SCRIPT_MAX_PATH)
    {
        printf("%s path too long: %" PRIu32 "\n", error, path_end);
        return false;
    }

    buffer[path_end] = '\0';

    memcpy(&bs->path[0], &path[0], path_end);

    /*
     * See if the executable has a checksum. This is a ',' the CRC32 checksum
     * length as hex characters from the end of the string. Set the checksum
     * to 0 is none is found.
     */
    if (buffer[l1_length + executable_end - (BOOT_SCRIPT_CSUM_SIZE) - 1] == ',')
    {
        csum = &buffer[l1_length + executable_end - (BOOT_SCRIPT_CSUM_SIZE)];
        for (i = 0; i < BOOT_SCRIPT_CSUM_SIZE; ++i)
            bs->checksum[i] = csum[i];
        executable_end -= (BOOT_SCRIPT_CSUM_SIZE) + 1;
    }
    else
    {
        for (i = 0; i < BOOT_SCRIPT_CSUM_SIZE; ++i)
            bs->checksum[i] = 0;
    }

    if (executable_end >= BOOT_SCRIPT_MAX_PATH)
    {
        printf("%s executable too long: %" PRIu32 "\n", error, executable_end);
        return false;
    }

    buffer[l1_length + executable_end] = '\0';

    memcpy(&bs->executable[0], &executable[0], executable_end);

    return true;
}
