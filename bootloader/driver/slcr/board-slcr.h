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
 * Board SLCR Control.
 */

#if !defined(BOARD_SLCR_H)
#define BOARD_SLCR_H

#include <stdint.h>
#include <driver/io/board-io.h>

/*
 * Unlock and lock the SLCR.
 */
void board_slcr_unlock(void);
void board_slcr_lock(void);

#endif
