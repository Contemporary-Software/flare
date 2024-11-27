/* SPDX-License-Identifier: Apache-2.0 */

/*
 * Copyright 2021 XIA LLC, All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file crc.hpp
 * @brief Defines utility functions and data structures related to CRC32 useful throughout the SDK.
 */

#ifndef PIXIESDK_UTIL_CRC_HPP
#define PIXIESDK_UTIL_CRC_HPP

#include <stdint.h>

#define CRC_CHECKSUM_SIZE   8
/**
 * @brief Defines a function for a CRC32 checksum
 *
 * The polynomial is
 * `X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0`
 *
 * @see https://en.wikipedia.org/wiki/Cyclic_redundancy_check
 */
typedef uint32_t CRC32;

void crc32_clear(CRC32* crc);

void crc32_update(CRC32* crc, const unsigned char* data, int len);

void crc32_str(CRC32* crc, unsigned char* data);

#endif  //PIXIESDK_UTIL_CRC_HPP
