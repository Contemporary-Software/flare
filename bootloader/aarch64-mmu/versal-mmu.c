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

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxVersal
 *
 * @brief This source file contains the default MMU tables and setup.
 */

/*
 * Copyright (C) 2021 Gedare Bloom <gedare@rtems.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <aarch64-mmu/aarch64-mmu.h>
#include <aarch64-mmu/aarch64-mmu-vmsav8-64.h>

uintptr_t bsp_translation_table_base[];

static const aarch64_mmu_config_entry
versal_mmu_config_table[] = {
  {  /* RAM Region 0 */
    .begin = 0x00000000U,
    .end = 0x80000000U,
    .flags = AARCH64_MMU_DATA_RW_CACHED
  }, {  /* Devices */
    .begin = 0xf0000000U,
    .end = 0xf2000000U,
    .flags = AARCH64_MMU_DEVICE
  }, {  /* APU GIC */
    .begin = 0xf9000000U,
    .end = 0xf90c0000U,
    .flags = AARCH64_MMU_DEVICE
  }, { /* FPD CSRs */
    .begin = 0xfd000000U,
    .end = 0xfe000000U,
    .flags = AARCH64_MMU_DEVICE
  }, { /* LPD CSRs */
    .begin = 0xfe000000U,
    .end = 0xfe800000U,
    .flags = AARCH64_MMU_DEVICE
  }, { /* LPD IOP CSRs and LPD peripherals */
    .begin = 0xff000000U,
    .end = 0xffc00000U,
    .flags = AARCH64_MMU_DEVICE
  }
};

void aarch64_setup_mmu_and_cache( void )
{
  versal_setup_mmu_and_cache();
}

void
versal_setup_mmu_and_cache( void )
{
  aarch64_mmu_setup();

  aarch64_mmu_setup_translation_table(
    &versal_mmu_config_table[ 0 ],
    RTEMS_ARRAY_SIZE( versal_mmu_config_table )
  );

  aarch64_mmu_enable();
}
