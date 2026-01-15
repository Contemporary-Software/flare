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

#ifndef DRIVERS_SDHCI_SDHCI_H_
#define DRIVERS_SDHCI_SDHCI_H_

#include <stdbool.h>
#include <stdint.h>

#define SDHCI_BLK_SIZE 512

typedef enum
{
    SDHCI_NO_ERROR = 0,
    SDHCI_CARD_NOT_PRESENT,
    SDHCI_CARD_NOT_SUPPORTED,
    SDHCI_RESET_FAILED,
    SDHCI_BUSY,
    SDHCI_TRANSFER_FAILED,
    SDHCI_CLK_ERROR,
    SDHCI_RESPONSE_TIMEOUT,
} sdhci_error;

sdhci_error sdhci_open(void);
sdhci_error sdhci_close(void);

bool sdhci_initialised();

sdhci_error sdhci_read(uint32_t sector, uint32_t count, char* buffer);

#endif /* DRIVERS_SDHCI_SDHCI_H_ */
