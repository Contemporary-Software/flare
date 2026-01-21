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

.org 0
.text

.global __vector_table

.section .vectors
__vector_table:
/* EL is using SP_EL0 stack */
.balign 0x80
/* Synchronous Exception - EL is using SP_EL0 stack */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #0
  b SWInterrupt

.balign 0x80
/* IRQ Exception - EL is using SP_EL0 stack */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #0
  b IRQInterrupt

.balign 0x80
/* FIQ Exception - EL is using SP_EL0 stack */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #0
  b FIQInterrupt

.balign 0x80
/* SError Exception - EL is using SP_EL0 stack */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #0
  b SError

/* EL is using SP_ELx stack */
.balign 0x80
/* Synchronous Exception - EL is using SP_ELx stack */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #1
  b SWInterrupt

.balign 0x80
/* IRQ Exception - EL is using SP_ELx stack */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #1
  b IRQInterrupt

.balign 0x80
/* FIQ Exception - EL is using SP_ELx stack */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #1
  b FIQInterrupt

.balign 0x80
/* SError Exception - EL is using SP_ELx stack */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #1
  b SError

/* From lower EL in AArch64 */
.balign 0x80
/* Synchronous Exception - From lower EL in AArch64 */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #2
  b SWInterrupt

.balign 0x80
/* IRQ Exception - From lower EL in AArch64 */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #2
  b IRQInterrupt

.balign 0x80
/* FIQ Exception - From lower EL in AArch64 */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #2
  b FIQInterrupt

.balign 0x80
/* SError Exception - From lower EL in AArch64 */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #2
  b SError

/* From lower EL in AArch32 */
.balign 0x80
/* Synchronous Exception - From lower EL in AArch32 */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #3
  b SWInterrupt

.balign 0x80
/* IRQ Exception - From lower EL in AArch32 */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #3
  b IRQInterrupt

.balign 0x80
/* FIQ Exception - From lower EL in AArch32 */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #3
  b FIQInterrupt

.balign 0x80
/* SError Exception - From lower EL in AArch32 */
  sub sp, sp, #8
  str x30, [sp, #0]
  bl _save_state
  mov x1, #3
  b SError

_save_state:
  sub sp, sp, #256
  stp x29, x28, [sp, #0]
  stp x27, x26, [sp, #16]
  stp x25, x24, [sp, #32]
  stp x23, x22, [sp, #48]
  stp x21, x20, [sp, #64]
  stp x19, x18, [sp, #80]
  stp x17, x16, [sp, #96]
  stp x15, x14, [sp, #112]
  stp x13, x12, [sp, #128]
  stp x11, x10, [sp, #144]
  stp x9, x8, [sp, #160]
  stp x7, x6, [sp, #176]
  stp x5, x4, [sp, #192]
  stp x3, x2, [sp, #208]
  stp x1, x0, [sp, #224]
  mrs x0, CurrentEL
  lsr x0, x0, #2
  and x0, x0, #3
  mov x1, #3
  cmp x0, x1
  b.eq _EL3_registers
  mov x1, #2
  cmp x0, x1
  b.eq _EL2_registers
_EL1_registers:
  mrs x2, ELR_EL1
  mrs x3, ESR_EL1
  mrs x4, FAR_EL1
  str x2, [sp, #240]
  stp x3, x4, [sp, #248]
  b _save_state_out
_EL2_registers:
  mrs x2, ELR_EL2
  mrs x3, ESR_EL2
  mrs x4, FAR_EL2
  str x2, [sp, #240]
  stp x3, x4, [sp, #248]
  b _save_state_out
_EL3_registers:
  mrs x2, ELR_EL3
  mrs x3, ESR_EL3
  mrs x4, FAR_EL3
  str x2, [sp, #240]
  stp x3, x4, [sp, #248]
_save_state_out:
  ret
