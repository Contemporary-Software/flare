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

#
# Flare FSBL Support.
#

flare-connect
flare-fsbl-restart

load

#b Undefined
#b SVCHandler
#b PrefetchAbortHandler
#b DataAbortHandler
#b IRQHandler
#b FIQHandler

#b zynq_handoff_exit
#b zynq_handoff_exit_no_mmu_reset
b _exit

b   exception_undefined_instruction
b   exception_supervisor_call
b   exception_prefetch_abort
b   exception_data_abort
b   exception_irq
b   exception_fiq

load
