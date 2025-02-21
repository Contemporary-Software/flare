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
 * Flare watchdog. The enable is on MIO_31 and low to disable.
 */

#include "wdog.h"
#include "gpio.h"

/*
 * Disable the watchdog.
 */
#define WDOG_WDI_PIN         30
#define WDOG_WDI_EN_PIN      31
#define WDOG_WDI_LOW         false
#define WDOG_WDI_HIGH        true
#define WDOG_WDI_EN_ENABLE   true
#define WDOG_WDI_EN_DISABLE  false

static bool gpio_setup;
static const gpio_pin_def gpio_wdog_en =
{
  pin:      WDOG_WDI_EN_PIN,
  output:   true,
  on:       WDOG_WDI_EN_ENABLE,
  outen:    true,
  volts:    gpio_LVCMOS33,
  pullup:   true,
  fast:     false,
  tristate: false
};
static const gpio_pin_def gpio_wdog_td =
{
  pin:      WDOG_WDI_PIN,
  output:   true,
  on:       WDOG_WDI_LOW,
  outen:    true,
  volts:    gpio_LVCMOS33,
  pullup:   true,
  fast:     false,
  tristate: false
};

static bool
board_wdog_setup(void)
{
    if (!gpio_setup)
    {
        gpio_error ge;
        ge = gpio_setup_pin(&gpio_wdog_en);
        if (ge != GPIO_NO_ERROR)
            return false;
        ge = gpio_setup_pin(&gpio_wdog_td);
        if (ge != GPIO_NO_ERROR)
            return false;
        gpio_setup = true;
    }
    return true;
}

void
board_wdog_toggle (void)
{
    if (board_wdog_setup())
    {
        gpio_output(gpio_wdog_td.pin, WDOG_WDI_HIGH);
        gpio_output(gpio_wdog_td.pin, WDOG_WDI_LOW);
    }
}

void
board_wdog_control(bool enable)
{
    if (board_wdog_setup())
    {
        gpio_output(gpio_wdog_en.pin, enable);
    }
}
