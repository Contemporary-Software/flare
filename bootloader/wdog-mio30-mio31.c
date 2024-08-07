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

static bool Gpio_Setup;
static const gpio_Pin_Def gpio_WDog_EN =
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
static const gpio_Pin_Def gpio_WDog_TD =
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
board_WDogSetup(void)
{
    if (!Gpio_Setup)
    {
        gpio_error ge;
        ge = gpio_SetupPin(&gpio_WDog_EN);
        if (ge != GPIO_NO_ERROR)
            return false;
        ge = gpio_SetupPin(&gpio_WDog_TD);
        if (ge != GPIO_NO_ERROR)
            return false;
        Gpio_Setup = true;
    }
    return true;
}

void
board_WDogToggle (void)
{
    if (board_WDogSetup())
    {
        gpio_Output(gpio_WDog_TD.pin, WDOG_WDI_HIGH);
        gpio_Output(gpio_WDog_TD.pin, WDOG_WDI_LOW);
    }
}

void
board_WDogControl(bool enable)
{
    if (board_WDogSetup())
    {
        gpio_Output(gpio_WDog_EN.pin, enable);
    }
}
