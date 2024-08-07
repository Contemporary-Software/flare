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
 * Hide the includes for the low level parts. This will help move away from
 * Xilinx.
 */

#if !defined(BOOTLOADER_H)
#define BOOTLOADER_H

#include <zynq.h>


#include "xil_types.h"
#include "xil_cache.h"
#include "xil_cache_l.h"
#include "xil_exception.h"
#include "xil_printf.h"

/*
 * Use printf.
 */
#define printf(...) xil_printf (__VA_ARGS__)

#endif
