/*
 * Copyright 2025 Contemporary Software
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

#ifndef DRIVER_SDHCI_ZYNQ7000_H_
#define DRIVER_SDHCI_ZYNQ7000_H_

#include "sdhci.h"

#define SDHCI0_REG_BASE 0xE0100000

#define ZYNQ7000_HOST_CTRL_VER_OFFSET  0xFEU
#define   ZYNQ7000_HC_SPEC_VER_MASK      0x00FFU
#define   ZYNQ7000_HC_SPEC_V3            0x0002U

#define ZYNQ7000_POWER_CTRL_OFFSET       0x29U  /**< Power Control */
#define   ZYNQ7000_PC_BUS_PWR_MASK       0x00000001U /**< Bus Power Control */
#define   ZYNQ7000_PC_BUS_VSEL_MASK      0x0000000EU /**< Bus Voltage Select */
#define   ZYNQ7000_PC_BUS_VSEL_3V3_MASK  0x0000000EU /**< Bus Voltage 3.3V */
#define   ZYNQ7000_PC_BUS_VSEL_3V0_MASK  0x0000000CU /**< Bus Voltage 3.0V */
#define   ZYNQ7000_PC_BUS_VSEL_1V8_MASK  0x0000000AU /**< Bus Voltage 1.8V */
#define   ZYNQ7000_PC_EMMC_HW_RST_MASK   0x00000010U /**< HW reset for eMMC */

#endif /* DRIVER_SDHCI_ZYNQ7000_H_ */
