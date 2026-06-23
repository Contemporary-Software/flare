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

#ifndef _FLARE_DRIVERS_PM_ZYNQMP_PM_H_
#define _FLARE_DRIVERS_PM_ZYNQMP_PM_H_

#define PMU_GLOBAL_BASE_ADDR 0xFFD80000U

#define PMU_GLOBAL_GLOBAL_CNTRL (PMU_GLOBAL_BASE_ADDR + 0)

#define PMU_GLOBAL_GLOBAL_CNTRL_FW_IS_PRESENT_MASK 0x00000010U

#define PM_VERSION_MAJOR_MASK  0xFFFF0000U
#define PM_VERSION_MAJOR_SHIFT 16U
#define PM_VERSION_MINOR_MASK  0x0000FFFFU
#define PM_VERSION_MINOR_SHIFT 0U

#endif /* _FLARE_DRIVERS_PM_ZYNQMP_PM_H_ */
