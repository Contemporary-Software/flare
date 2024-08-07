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
 * Wrapper for tinfl.c
 */

#include "tzlib.h"

#if !TZLIB_USE_ZLIB

#define MINIZ_NO_MALLOC 1

#include "tinfl.c"

static int tinfl_result(tinfl_status status)
{
    int result;
    switch (status)
    {
        default:
            result = Z_OK;
            break;
        case TINFL_STATUS_BAD_PARAM:
        case TINFL_STATUS_ADLER32_MISMATCH:
        case TINFL_STATUS_FAILED:
        case TINFL_STATUS_NEEDS_MORE_INPUT:
            result = Z_DATA_ERROR;
            break;
        case TINFL_STATUS_HAS_MORE_OUTPUT:
            result = Z_BUF_ERROR;
            break;
    }
    return result;
}

int
raw_uncompress (Bytef*       dest,
                uLongf*      destLen,
                const Bytef* source,
                uLongf       sourceLen)
{
    tinfl_decompressor decomp;
    tinfl_status       status;
    size_t             inLen = (size_t) sourceLen;
    size_t             outLen = (size_t) *destLen;
    tinfl_init(&decomp);
    status = tinfl_decompress(&decomp,
                              (const mz_uint8*) source,
                              &inLen,
                              (mz_uint8*) dest,
                              (mz_uint8*) dest,
                              &outLen,
                              TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
    if (status == TINFL_STATUS_DONE)
        *destLen = outLen;
    return tinfl_result(status);
}

int
uncompress (Bytef*       dest,
            uLongf*      destLen,
            const Bytef* source,
            uLongf       sourceLen)
{
    tinfl_decompressor decomp;
    tinfl_status       status;
    size_t             inLen = (size_t) sourceLen;
    size_t             outLen = (size_t) *destLen;
    tinfl_init(&decomp);
    status = tinfl_decompress(&decomp,
                              (const mz_uint8*) source,
                              &inLen,
                              (mz_uint8*) dest,
                              (mz_uint8*) dest,
                              &outLen,
                              TINFL_FLAG_PARSE_ZLIB_HEADER | \
                              TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
    if (status == TINFL_STATUS_DONE)
        *destLen = outLen;
    return tinfl_result(status);
}

#endif
