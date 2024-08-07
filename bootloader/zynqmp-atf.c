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

#include <stdio.h>

#include <board-handoff.h>
#include <cache.h>
#include <wdog.h>
#include <zynqmp-atf.h>
#include <zynqmp-factory-boot.h>

void do_atf(void) {
    board_handoff_exit_no_mmu_reset(ATF_ENTRYPOINT);
}

void atf_return(void) {
    //main();
}

void factory_atf_return(void) {
    cache_enable_dcache();
    cache_enable_icache();
    wdog_init();
    wdog_control(false);

    printf("Returned to Flare\n");

    zynqmp_factory_second_stage_boot();
}