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
 * Board UART.
 */

#if !defined(BOARD_UART_H)
#define BOARD_UART_H

#include <stdbool.h>
#include <stdint.h>

void board_uart_setup(uint32_t uart);
void board_uart_send(uint32_t uart, uint8_t c);
bool board_uart_tx_idle(uint32_t uart);
uint8_t board_uart_receive(uint32_t uart);
bool board_uart_receive_idle(uint32_t uart);

#endif
