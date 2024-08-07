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
 * Xilinx Board Handoff.
 */

#if !defined(BOARD_HANDOFF_H)
#define BOARD_HANDOFF_H

#include <stdint.h>

/*
 * Processor handoff to loaded code or JTAG.
 */
void board_handoff_exit(uint32_t address);
void board_handoff_exit_no_mmu_reset(uint32_t address);
void board_handoff_jtag_exit(void);

#endif
