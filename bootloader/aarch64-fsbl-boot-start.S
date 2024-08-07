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


.global _boot
.global __stack
.global __vector_table
.global _reset

/* Stack Pointer locations for boot code */
.set SYS_stack,    __stack

.set vector_base,  __vector_table

.section .boot,"ax"
_boot:
  /* Set up the stack poniter */
  ldr  x0, =SYS_stack      /* SYS stack pointer */
  mov  sp, x0

  /*
   * Set vector table offsets. All set to 0x100 
   * to share the same vector table and avoid NULL
   * pointer accesses.
   */
  ldr x0, =vector_base
  msr VBAR_EL3, x0
  msr VBAR_EL2, x0
  msr VBAR_EL1, x0

  bl pre_main

  /* Set up mmu and caching */
  //bl aarch64_setup_mmu_and_cache

  /* Branch to main */
  b main

/* Zero registers and jump to boot */
_reset:
  mov X0, XZR
  mov X1, XZR
  mov X2, XZR
  mov X3, XZR
  mov X4, XZR
  mov X5, XZR
  mov X6, XZR
  mov X7, XZR
  mov X8, XZR
  mov X9, XZR
  mov X10, XZR
  mov X11, XZR
  mov X12, XZR
  mov X13, XZR
  mov X14, XZR
  mov X15, XZR
  mov X16, XZR
  mov X17, XZR
  mov X18, XZR
  mov X19, XZR
  mov X20, XZR
  mov X21, XZR
  mov X22, XZR
  mov X23, XZR
  mov X24, XZR
  mov X25, XZR
  mov X26, XZR
  mov X27, XZR
  mov X28, XZR
  mov X29, XZR
  mov X30, XZR
  b _boot
