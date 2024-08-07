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
 * Flare FSBL JFFS2 Factory Boot support.
 */

#include <stdio.h>
#include <string.h>

#include "cache.h"
#include "datasafe.h"
#include "factory-boot.h"
#include "flash-map.h"
#include "flash.h"
#include "flare-boot.h"
#include "md5.h"
#include "reset.h"
#include "tzlib.h"
#include "wdog.h"
#include "board-handoff.h"
#include "board.h"

/*
 * A common label.
 */
static const char* const factory_boot_label = "EXE";

/*
 * The factory boot base and the number of file slots.
 */
#define FACTORY_BOOT_BASE       FLARE_FLASH_EXE_BASE
#define FACTORY_BOOT_FILE_SLOTS FLASH_SLOT_FILES

 /*
  * Three files:
  *  1: bitfile     * load
  *  2: executable  * load
  *  3: file image  - ignore
  */
#define FLASH_SLOT_BASE      FLARE_FLASH_EXE_BASE
#define FLASH_SLOT_SIZE      FLARE_FLASH_EXE_SIZE
#define FLASH_SLOT_FILES     (3)
#define EXECUTABLE_LOAD_BASE FLARE_EXECUTABLE_BASE
#define EXECUTABLE_LOAD_SIZE FLARE_EXECUTABLE_SIZE

#define MD5_CHECKSUM_SIZE   16

#define MASK_N_DIV(x, n) ((x) & ~((n) - 1))
#define MASK_N_MOD(x, n) ((x) & ((n) - 1))
#define PAD_n(x, n)      (MASK_N_MOD(x, n) == 0 ? (x) : MASK_N_DIV((x) + ((n) - 1), n))
#define PAD_4(x)         PAD_n(x, 4)

static uint32_t
FactoryLoadImage_Get32(uint8_t* table, uint32_t index, uint32_t offset)
{
    return *((uint32_t*) IMAGE_HEADER_RECORD(table, index, offset));
}

static uint8_t
FactoryLoadImage_Get8(uint8_t* table, uint32_t index, uint32_t offset)
{
    return *((uint8_t*) IMAGE_HEADER_RECORD(table, index, offset));
}

static bool
FactoryLoadImage(const char* label,
                 uint32_t    base,
                 size_t      size,
                 uint8_t*    loadTo,
                 size_t*     loadSize,
                 uint8_t*    cs,
                 bool        compressed)
{
    uint8_t     checksum[MD5_CHECKSUM_SIZE];
    flash_error fe;
    int         i;

    fe = flash_Read(base, loadTo, size);
    if (fe != FLASH_NO_ERROR)
    {
        printf("error: load %s factory image: %d\n", label, fe);
        return false;
    }

    md5(loadTo, size, checksum);

    for (i = 0; i < MD5_CHECKSUM_SIZE; ++i, ++cs)
    {
        if (*cs != checksum[i])
        {
            printf("error: invalid %s checksum\n", label);
            return false;
        }
    }

    if (compressed)
    {
        size_t offset;
        uLongf dsize;
        int    ze;

        /*
         * We only support a gzip format file:
         *
         *    http://www.gzip.org/zlib/rfc-gzip.html
         *
         * Check the header.
         */

        if ((loadTo[0] != 0x1f) || (loadTo[1] != 0x8b) || (loadTo[2] != 0x08))
        {
            printf("error: invalid %s header\n", label);
            return false;
        }

        offset = 10;

        /* FLG.FEXTRA */
        if ((loadTo[3] & (1 << 2)) != 0)
            offset += ((loadTo[offset] << 8) | loadTo[offset + 1]) + 2;
        /* FLG.FNAME */
        if ((loadTo[3] & (1 << 3)) != 0)
        {
            while (loadTo[offset] != 0)
            {
                if (offset >= size)
                {
                    printf("error: invalid %s header: fname\n", label);
                    return false;
                }
                ++offset;
            }
            ++offset;
        }
        /* FLG.FCOMMENT */
        if ((loadTo[3] & (1 << 4)) != 0)
        {
            while (loadTo[offset] != 0)
            {
                if (offset >= size)
                {
                    printf("error: invalid %s header: fcomment\n", label);
                    return false;
                }
                ++offset;
            }
            ++offset;
        }
        /* FLG.FHCRC */
        if ((loadTo[3] & (1 << 1)) != 0)
            offset += 2;

        dsize = *loadSize - PAD_4(size);

        ze = raw_uncompress(loadTo + PAD_4(size),
                            &dsize,
                            loadTo + offset,
                            size - offset - 8);
        if (ze != Z_OK)
        {
            printf("error: %s uncompress failure: %d\n", label, ze);
            return false;
        }

        *loadSize = dsize;

        memmove(loadTo, loadTo + PAD_4(size), dsize);
    }

    return true;
}

static void
FactoryBooter(uint8_t* header, size_t header_size)
{
    uint32_t flash_offset;
    size_t   size;
    size_t   load_size;
    bool     rc;

    //flare_DataSafe_SetFactoryBoot();
    //flare_DataSafe_FsblSet("/FB_" SITORO_GIT_SHA1, SITORO_GIT_SHA1 ".bt");

    size = FactoryLoadImage_Get32(header, 0, IMAGE_HEADER_SIZE);
    if (size > EXECUTABLE_LOAD_SIZE)
    {
        printf("error: %s factory image too big\n", factory_boot_label);
        return;
    }

    flash_offset = FLASH_SLOT_BASE + header_size;

    size = FactoryLoadImage_Get32(header, 1, IMAGE_HEADER_SIZE);
    if (size > EXECUTABLE_LOAD_SIZE)
    {
        printf("error: %s factory image too big\n", factory_boot_label);
        return;
    }

    printf("  Executable: FB_%s/%s\n",
           "GITHASH",
           IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_NAME));

    load_size = EXECUTABLE_LOAD_SIZE;

    rc = FactoryLoadImage(factory_boot_label,
                          flash_offset,
                          size,
                          (uint8_t*) EXECUTABLE_LOAD_BASE,
                          &load_size,
                          IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_MD5),
                          true);
    if (!rc)
        return;
    /*
    flare_DataSafe_FlareSet(QSPI_MODE,
                            (const char*) IMAGE_HEADER_RECORD(header, 0, IMAGE_HEADER_NAME),
                            (const char*) IMAGE_HEADER_RECORD(header, 1, IMAGE_HEADER_NAME),
                            0);

    flare_DataSafe_ClearFactoryBootRequest();
    */
    wdog_control(true);
    cache_disable();
    board_handoff_exit(EXECUTABLE_LOAD_BASE);
}

void
FactoryBoot(const char* why)
{
    uint8_t     header[IMAGE_HEADER_TOTAL(FACTORY_BOOT_FILE_SLOTS)];
    uint8_t     checksum[IMAGE_HEADER_HEADER_MD5_SIZE];
    flash_error fe;
    bool        ok = true;
    int         i;

    printf("Factory Boot: %s/%s (%s)\n", factory_boot_label, "HASH", why);

    fe = flash_Read(FACTORY_BOOT_BASE, header, sizeof(header));

    if (fe != FLASH_NO_ERROR)
    {
        printf("error: reading %s factory header: %d\n", factory_boot_label, fe);
        return;
    }

    md5(header + IMAGE_HEADER_HEADER_REC,
        sizeof(header) - IMAGE_HEADER_HEADER_MD5_SIZE,
        checksum);

    for (i = 0; i < IMAGE_HEADER_HEADER_MD5_SIZE; ++i)
    {
        if (header[IMAGE_HEADER_HEADER_MD5 + i] != checksum[i])
        {
            printf("error: invalid %s header checksum\n", factory_boot_label);
            ok = false;
            break;
        }
    }

    if (ok)
        FactoryBooter(header, sizeof(header));

    reset();
}
