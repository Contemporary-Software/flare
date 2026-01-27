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

.globl _begin_lo
.globl _begin_hi

/* 
 * This code relocates the OCM mapped low to being mapped high.
 * This includes moving flare to the high mapped OCM. _begin_hi is
 * copied to OCM that starts mapped high (0xFFFF0000). This code then
 * maps all of the OCM to high and jumps to _boot at the new address.
 **/

.section .begin_lo
_begin_lo:
    ldr r7, =0xFFFF0000
    ldr r0, =_begin_hi_start_addr
    ldr r3, =_begin_hi_end_addr
    and r1, r1, #0
    ldr r2, [r0, r1]
    str r2, [r7, r1]
    add r1, r1, #4
    cmp r1, r3
    sublo pc, pc, #0x18
    bx r7 /* _begin_hi */

.section .begin_hi
_begin_hi:
    ldr r7, =0xF8000000
    ldr r8, =0x0000DF0D
    str r8, [r7, #0x8] /* Unlock SLCR */
    ldr r8, [r7, #0x910] /* Read OCM_CFG */
    orr r8, r8, #0xF
    str r8, [r7, #0x910] /* Set all OCM to high */
    and r8, r8, #0
    ldr r9, =0xF8F00040
    str r8, [r9] /* Set MPCORE Filter start addr to 0x0 */
    ldr r8, =0x0000767B
    str r8, [r7, #0x4] /* Lock SLCR */
    ldr r7, =_exe_offset
    ldr r8, =_boot
    add r7, r7, r8
    bx r7 /* _boot */
