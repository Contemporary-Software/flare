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
