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

#include "gpio.h"

#include "flare-io.h"

#define GPIO_BASE         (0xe000a000)
#define GPIO_DIR_BASE     (GPIO_BASE + 0x204)
#define GPIO_DIR_OFF      (0x040)
#define GPIO_OUTEN_BASE   (GPIO_BASE + 0x208)
#define GPIO_OUTEN_OFF    (0x040)
#define GPIO_MASK_BASE    (GPIO_BASE + 0x000)
#define GPIO_DATA_RO_BASE (GPIO_BASE + 0x060)

static uint32_t
gpio_Bank(int pin)
{
    return pin > 31 ? 1 : 0;
}

static void
gpio_SetOutputEnable(int pin, bool state)
{
    const uint32_t reg = GPIO_OUTEN_BASE + (gpio_Bank(pin) * GPIO_OUTEN_OFF);
    flare_reg_write(reg,
                    flare_reg_read(reg) | (state ? 1 : 0) << (pin > 31 ? pin - 32 : pin));
}

static void
gpio_SetDirection(int pin, bool state)
{
    const uint32_t reg = GPIO_DIR_BASE + (gpio_Bank(pin) * GPIO_DIR_OFF);
    flare_reg_write(reg,
                    flare_reg_read(reg) | (state ? 1 : 0) << (pin > 31 ? pin - 32 : pin));
}

gpio_error
gpio_SetupPin(const gpio_Pin_Def* pin)
{
    uint32_t value;

    if ((pin->pin < 0) || (pin->pin > GPIO_MAX_PINS))
        return GPIO_INVALID_PIN;

    switch (pin->volts)
    {
        case gpio_LVCMOS18:
            value = (1 << 9);
            break;
        case gpio_LVCMOS25:
            value = (2 << 9);
            break;
        case gpio_LVCMOS33:
            value = (3 << 9);
            break;
        case gpio_HSTL:
            value = (4 << 9) | (pin->output ? (1 << 13) : 0);
            break;
        default:
            return GPIO_INVALID_VOLTAGE;
    }

    value |= pin->pullup   ? (1 << 12) : 0;
    value |= pin->fast     ? (1 << 8) : 0;
    value |= pin->tristate ? (1 << 0) : 0;

    flare_reg_write(0xF8000008, 0xDF0D);
    flare_reg_write(0xF8000700 + (pin->pin * 4), value);
    flare_reg_write(0xF8000004, 0x767B);

    gpio_SetDirection(pin->pin, pin->output);
    if (pin->output)
    {
        gpio_SetOutputEnable(pin->pin, pin->outen);
        gpio_Output(pin->pin, pin->on);
    }

    return GPIO_NO_ERROR;
}

gpio_error
gpio_SetupPins(const gpio_Pin_Def* pins, size_t size)
{
    size_t pin;
    for (pin = 0; pin < size; ++pin, ++pins)
    {
        gpio_error ge = gpio_SetupPin(pins);
        if (ge != GPIO_NO_ERROR)
            return ge;
    }
    return GPIO_NO_ERROR;
}

gpio_error
gpio_OutputEnable(int pin, bool on)
{
    if ((pin < 0) || (pin > GPIO_MAX_PINS))
        return GPIO_INVALID_PIN;

    gpio_SetOutputEnable(pin, on);

    return GPIO_NO_ERROR;
}

gpio_error
gpio_Output(int pin, bool on)
{
    uint32_t reg;

    if ((pin < 0) || (pin > GPIO_MAX_PINS))
        return GPIO_INVALID_PIN;

    reg = GPIO_MASK_BASE + ((pin / 16) * 4);
    pin %= 16;

    flare_reg_write(reg,
                    ((~(1 << pin) & 0xffff) << 16) | ((on ? 1 : 0) << pin));

    return GPIO_NO_ERROR;
}

gpio_error
gpio_Input(int pin, bool* on)
{
    uint32_t reg;

    if ((pin < 0) || (pin > GPIO_MAX_PINS))
        return GPIO_INVALID_PIN;

    reg = GPIO_DATA_RO_BASE + ((pin / 32) * 4);
    pin %= 32;

    *on = (flare_reg_read(reg) & (1<< pin)) == 0 ? false : true;

    return GPIO_NO_ERROR;
}
