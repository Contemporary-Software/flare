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

/*
 * Xilinx uses their own handoff params for ATF. A pointer to the
 * xil_atf_handoff_params struct is placed in PMU_GLOBAL.GLOBAL_GEN_STORAGE6.
 * ATF then uses the last secure entry for BL32 and last non-secure entry
 * for BL33.
 *
 * The implementation for this can be found in
 * arm-trusted-firmware/plat/xilinx/common/plat_startup.c
 */

#ifndef _FLARE_ARCH_AARCH64_AARCH64_ATF_H_
#define _FLARE_ARCH_AARCH64_AARCH64_ATF_H_

#include <stdint.h>

#define ATF_DEFAULT_ENTRY 0xFFFEA000

#define PMU_GLOBAL_GLOBAL_GEN_STORAGE6 0xFFD80048

#define FLARE_ATF_PARAMS_NUM_ENTRIES (1U)

#define XILINX_ATF_MAGIC_VALUE_0 'X'
#define XILINX_ATF_MAGIC_VALUE_1 'L'
#define XILINX_ATF_MAGIC_VALUE_2 'N'
#define XILINX_ATF_MAGIC_VALUE_3 'X'

#define ATF_ENTRY_FLAGS_EXEC_STATE_AARCH64  0x0U
#define ATF_ENTRY_FLAGS_EXEC_STATE_AARCH32  0x1U

#define ATF_ENTRY_FLAGS_LITTLE_ENDIAN       0x0U
#define ATF_ENTRY_FLAGS_BIG_ENDIAN          0x2U

#define ATF_ENTRY_FLAGS_NON_SECURE          0x0U
#define ATF_ENTRY_FLAGS_SECURE              0x4U

#define ATF_ENTRY_FLAGS_EL3                 (3U << 3U)
#define ATF_ENTRY_FLAGS_EL2                 (2U << 3U)
#define ATF_ENTRY_FLAGS_EL1                 (1U << 3U)
#define ATF_ENTRY_FLAGS_EL0                 (0U << 3U)

#define ATF_ENTRY_FLAGS_CPU_A53_0           (0U << 5U)
#define ATF_ENTRY_FLAGS_CPU_A53_1           (1U << 5U)
#define ATF_ENTRY_FLAGS_CPU_A53_2           (2U << 5U)
#define ATF_ENTRY_FLAGS_CPU_A53_3           (3U << 5U)

#define FLARE_ATF_ENTRY_FLAGS (ATF_ENTRY_FLAGS_EXEC_STATE_AARCH64 | \
    ATF_ENTRY_FLAGS_LITTLE_ENDIAN | ATF_ENTRY_FLAGS_NON_SECURE | \
    ATF_ENTRY_FLAGS_EL2 | ATF_ENTRY_FLAGS_CPU_A53_0)

struct xil_atf_handoff_entry {
    uint64_t entry_point;
    uint64_t flags;
};

struct xil_atf_handoff_params {
    char magic_value[4];
    uint32_t num_entries;
    struct xil_atf_handoff_entry entries[FLARE_ATF_PARAMS_NUM_ENTRIES];
};

void aarch64_atf_handoff();

#endif /* _FLARE_ARCH_AARCH64_AARCH64_ATF_H_ */
