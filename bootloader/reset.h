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
 * Reset.
 *
 * Resetting a Zynq is more complicated than asserting SRST signal. You need a
 * reset POR reset and hardware to reset a secure eFuse running system.
 */

#if !defined (RESET_H)
#define RESET_H

void reset(void) __attribute__ ((noreturn));

#endif
