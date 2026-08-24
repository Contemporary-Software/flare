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
 * Some brain dead versions of libc functions we need.
 *
 * These calls can be builtins for gcc and so we do not enable LTO.
 */

#include <stddef.h>
#include <stdint.h>

void* memset(void* dst, int c, size_t len) {
  volatile uint8_t* ud = dst;
  while (len--)
    *ud++ = c;
  return dst;
}
