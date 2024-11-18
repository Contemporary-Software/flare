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
 ==================================================================
 * Copyright (C) 2014 Southern Innovation International Pty Ltd.  *
 * All rights reserved.                                           *
 *                                                                *
 * Unauthorised copying, distribution, use, modification, reverse *
 * engineering or disclosure is strictly prohibited.              *
 *                                                                *
 * This file, its contents and the  fact of its disclosure to you *
 * is  Confidential  Information  and  Proprietary Information of *
 * Southern   Innovation   International  Pty  Ltd  and  Southern *
 * Innovation Trading Pty Ltd.   Unauthorised  disclosure of such *
 * information is strictly prohibited.                            *
 *                                                                *
 * THIS SOURCE CODE AND  SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY *
 * REPRESENTATIONS  OR WARRANTIES OTHER  THAN THOSE WHICH MAY NOT *
 * BE  EXCLUDED BY LAW.   IN NO EVENT  SHALL  SOUTHERN INNOVATION *
 * INTERNATIONAL PTY LTD  OR  SOUTHERN INNOVATION TRADING PTY LTD *
 * BE LIABLE  FOR ANY SPECIAL, DIRECT, INDIRECT  OR CONSEQUENTIAL *
 * DAMAGES   HOWSOEVER  INCURRED  (INCLUDING  BY  NEGLIGENCE)  IN *
 * CONNECTION WITH THE USE OF THIS SOURCE CODE AND SOFTWARE.      *
 * WHERE SUCH LIABILITY  MAY ONLY BE LIMITED AT LAW, LIABILITY IS *
 * LIMITED TO THE MAXIMUM EXTENT PERMITTED BY LAW.                *
 ==================================================================
*/

/**
 * General Purpose IO.
 */

#if !defined(_GPIO_H_)
#define _GPIO_H_

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Max number of pins.
 */
#define GPIO_MAX_PINS (53)

/*
 * Voltages.
 */
typedef enum
{
    gpio_LVCMOS18,
    gpio_LVCMOS25,
    gpio_LVCMOS33,
    gpio_HSTL
} gpio_pin_volts;

/*
 * Pin definition.
 */
typedef struct
{
    int            pin;       /* The pin number. */
    bool           output;    /* True for output, false for input. */
    bool           on;        /* True default output is on */
    bool           outen;     /* True for output enable on, false for off. */
    gpio_pin_volts volts;     /* The voltage level. */
    bool           pullup;    /* True to enable the pull up. */
    bool           fast;      /* True to be a fast CMOS edge */
    bool           tristate;  /* True to enable tri-state. */
} gpio_Pin_Def;

/*
 * Error codes.
 */
typedef enum
{
    GPIO_NO_ERROR = 0,
    GPIO_INVALID_PIN,
    GPIO_INVALID_VOLTAGE
} gpio_error;

gpio_error gpio_SetupPin(const gpio_Pin_Def* pin);
gpio_error gpio_SetupPins(const gpio_Pin_Def* pins, size_t size);
gpio_error gpio_OutputEnable(int pin, bool on);
gpio_error gpio_Output(int pin, bool on);
gpio_error gpio_Input(int pin, bool* on);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
