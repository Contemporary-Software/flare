/*
 * Copyright 2026 Contemporary Software
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

#ifndef _FLARE_DRIVERS_PM_IPI_H_
#define _FLARE_DRIVERS_PM_IPI_H_

#include <stdint.h>

#if FLARE_VERSAL
/* Unsupported */
#elif FLARE_ZYNQ7000
/* Unsupported */
#else
#include "zynqmp-ipi.h"
#endif /* ARCH */

#define IPI_PAYLOAD_ARG_CNT 8

int pm_ipi_send(uint32_t payload[IPI_PAYLOAD_ARG_CNT]);

int pm_ipi_read(uint32_t response[IPI_PAYLOAD_ARG_CNT]);

#endif /* _FLARE_DRIVERS_PM_IPI_H_ */
