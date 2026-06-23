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

#ifndef _FLARE_DRIVERS_PM_ZYNQMP_IPI_H_
#define _FLARE_DRIVERS_PM_ZYNQMP_IPI_H_

#define FLARE_IPI_BASE 0xFF300000U
#define FLARE_IPI_MSG_BASE 0xFF990000U

#define IPI_BUF_OFF_GROUP  (8U * 2U * 32U)
#define IPI_BUF_OFF_TARGET (2U * 32U)
#define IPI_BUF_OFF_RESP   (32U)

#define IPI_INDEX_RPU_0 0
#define IPI_INDEX_RPU_1 1
#define IPI_INDEX_APU_0 2
#define IPI_INDEX_PL_0  3
#define IPI_INDEX_PL_1  4
#define IPI_INDEX_PL_2  5
#define IPI_INDEX_PL_3  6
#define IPI_INDEX_PMU_0 7
#define IPI_INDEX_PMU_1 7
#define IPI_INDEX_PMU_2 7
#define IPI_INDEX_PMU_3 7

#define IPI_APU_0_OFF 0x0
#define IPI_PMU_0_OFF 0x30000
#define IPI_PMU_1_OFF 0x31000

#define IPI_TRIG_OFF 0x0
#define IPI_OBS_OFF  0x4
#define IPI_ISR_OFF  0x10
#define IPI_IMR_OFF  0x14
#define IPI_IER_OFF  0x18
#define IPI_IDR_OFF  0x1C

#define IPI_TRIG(d) (FLARE_IPI_BASE + IPI_TRIG_OFF + d)
#define IPI_OBS(d) (FLARE_IPI_BASE + IPI_OBS_OFF + d)
#define IPI_ISR(d) (FLARE_IPI_BASE + IPI_ISR_OFF + d)
#define IPI_IMR(d) (FLARE_IPI_BASE + IPI_IMR_OFF + d)
#define IPI_IER(d) (FLARE_IPI_BASE + IPI_IER_OFF + d)
#define IPI_IDR(d) (FLARE_IPI_BASE + IPI_IDR_OFF + d)

#define IPI_MASK_PL_3 (1U << 27)
#define IPI_MASK_PL_2 (1U << 26)
#define IPI_MASK_PL_1 (1U << 25)
#define IPI_MASK_PL_0 (1U << 24)

#define IPI_MASK_PMU_3 (1U << 19)
#define IPI_MASK_PMU_2 (1U << 18)
#define IPI_MASK_PMU_1 (1U << 17)
#define IPI_MASK_PMU_0 (1U << 16)

#define IPI_MASK_RPU_1 (1U << 9)
#define IPI_MASK_RPU_0 (1U << 8)

#define IPI_MASK_APU (1U << 0)

#endif /* _FLARE_DRIVERS_PM_ZYNQMP_IPI_H_ */
