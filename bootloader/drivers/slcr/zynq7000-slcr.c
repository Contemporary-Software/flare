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
 * Xilinx Zynq SLCR Control.
 */

#include "board-slcr.h"

#include <io/board-io.h>

void
board_slcr_unlock(void)
{
  board_reg_write(0xf8000008, 0xdf0d);
}

void
board_slcr_lock(void)
{
  board_reg_write(0xf8000004, 0x767b);
}
