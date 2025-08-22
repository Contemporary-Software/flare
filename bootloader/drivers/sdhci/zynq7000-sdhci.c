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

#include <sdhci/sdhci.h>
#include <sdhci/sdhci_hw.h>
#include <sdhci/zynq7000-sdhci.h>
#include <timer/sleep.h>

uint32_t sdhci_address() {
    return SDHCI0_REG_BASE;
}

void disable_bus_power() {
    uint16_t hc_ver =
        sdhci_reg_read_16(ZYNQ7000_HOST_CTRL_VER_OFFSET) & ZYNQ7000_HC_SPEC_VER_MASK;

    if (hc_ver == ZYNQ7000_HC_SPEC_V3) {
        sdhci_reg_write_8(ZYNQ7000_POWER_CTRL_OFFSET, ZYNQ7000_PC_EMMC_HW_RST_MASK);
    } else {
        sdhci_reg_write_8(ZYNQ7000_POWER_CTRL_OFFSET, 0x0);
    }
    usleep(1000);
}

void enable_bus_power() {
    uint16_t hc_ver =
        sdhci_reg_read_16(ZYNQ7000_HOST_CTRL_VER_OFFSET) & ZYNQ7000_HC_SPEC_VER_MASK;

    if (hc_ver == ZYNQ7000_HC_SPEC_V3) {
        sdhci_reg_write_8(ZYNQ7000_POWER_CTRL_OFFSET,
            (ZYNQ7000_PC_BUS_VSEL_3V3_MASK | ZYNQ7000_PC_BUS_PWR_MASK) &
            ~ZYNQ7000_PC_EMMC_HW_RST_MASK);
    } else {
        sdhci_reg_write_8(ZYNQ7000_POWER_CTRL_OFFSET,
            (ZYNQ7000_PC_BUS_VSEL_3V3_MASK | ZYNQ7000_PC_BUS_PWR_MASK));
    }
    usleep(200);
}

