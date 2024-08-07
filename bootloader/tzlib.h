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

#if !defined(_TZLIB_H_)
#define _TZIB_H_

#if !defined(TZLIB_USE_ZLIB)
#define TZLIB_USE_ZLIB 0
#endif

#if TZLIB_USE_ZLIB
#include <zlib.h>
#else
#include <stdint.h>

typedef uint8_t Bytef;
typedef uint32_t uLongf;

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)

int raw_uncompress (Bytef*       dest,
                    uLongf*      destLen,
                    const Bytef* source,
                    uLongf       sourceLen);

int uncompress (Bytef*       dest,
                uLongf*      destLen,
                const Bytef* source,
                uLongf       sourceLen);
#endif
#endif
