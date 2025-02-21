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
 * Leds. This is low level led support.
 */

#include <stdbool.h>

#include "gpio.h"
#include "leds.h"

/**
 * Leds we support.
 */
#define LED_STATUS_GREEN 0
#define LED_STATUS_RED   1

/*
 * Status leds
 */
static bool Gpio_Setup;
static bool led_state[2];
static const gpio_pin_def gpio_Leds[2] =
{
  {
    pin:      33,
    output:   true,
    on:       false,
    outen:    true,
    volts:    gpio_LVCMOS33,
    pullup:   true,
    fast:     false,
    tristate: false
  },
  {
    pin:      32,
    output:   true,
    on:       false,
    outen:    true,
    volts:    gpio_LVCMOS33,
    pullup:   true,
    fast:     false,
    tristate: false
  }
};

void
led_init(void)
{
  gpio_error ge;
  ge = gpio_setup_pins(gpio_Leds, 2);
  if (ge == GPIO_NO_ERROR)
    Gpio_Setup = true;
}

static void
led_on(int led)
{
  if (Gpio_Setup && led < 2)
  {
    led_state[led] = true;
    gpio_output(gpio_Leds[led].pin, true);
  }
}

static void
led_off(int led)
{
  if (Gpio_Setup && led < 2)
  {
    led_state[led] = false;
    gpio_output(gpio_Leds[led].pin, false);
  }
}

void
led_normal(void)
{
  led_off(LED_STATUS_GREEN);
  led_on(LED_STATUS_RED);
}

void
led_execute(void)
{
    led_on(LED_STATUS_GREEN);
    led_on(LED_STATUS_RED);
}

void
led_failure(void)
{
  led_off(LED_STATUS_GREEN);
  led_on(LED_STATUS_RED);
}

void
led_toggle(void)
{
  int l;
  for (l = 0; l < 2; ++l)
  {
    if (led_state[l])
      led_off(l);
    else
      led_on(l);
  }
}
