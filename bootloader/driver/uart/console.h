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
 * Console support.
 */

#if !defined(CONSOLE_H)
#define CONSOLE_H

#define ZYNQ_UART_0        0xe0001000
#define ZYNQMP_UART_0      0xff000000
#define ZYNQMP_UART_1      0xff010000
#define VERSAL_UART_0      0xff000000
#ifdef FLARE_ZYNQ7000
  #define STDOUT_BASEADDRESS ZYNQ_UART_0
  #define STDIN_BASEADDRESS  ZYNQ_UART_0
#elif FLARE_ZYNQMP
  #define STDOUT_BASEADDRESS ZYNQMP_UART_1
  #define STDIN_BASEADDRESS  ZYNQMP_UART_1
#else
  #define STDOUT_BASEADDRESS VERSAL_UART_0
  #define STDIN_BASEADDRESS  VERSAL_UART_0
#endif

char inbyte(void);
int  inbyte_available(void);
void outbyte(char c);
void console_flush(void);

#endif
