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

/**
 * Low level hardware access to the Flare board.
 */

#if !defined(_HWIO_H_)
#define _HWIO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef uintptr_t flare_addr;

/*
 * Write to a device memory location.
 */
static inline void
flare_reg_write (flare_addr address, uint32_t value)
{
    volatile uint32_t* ap = (uint32_t*) address;
    *ap = value;
}

/*
 * Read from a device memory location.
 */
static inline uint32_t
flare_reg_read (flare_addr address)
{
    volatile uint32_t* ap = (uint32_t*) address;
    return *ap;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
