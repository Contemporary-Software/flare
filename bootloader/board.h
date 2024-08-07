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

#if !defined(BOARD_H)
#define BOARD_H

#ifdef FLARE_AARCH64_VERSAL
  #include "versal.h"
#elif FLARE_AARCH64_ZYNQMP
  #include "zynqmp.h"
#else
  #include "zynq7000.h"
#endif

/*
 * Board set up.
 */
void board_hardware_setup(void);

#endif
