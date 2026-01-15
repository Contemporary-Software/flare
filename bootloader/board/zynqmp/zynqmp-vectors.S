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

/* Synchronous Exception */
.balign 0x80
_Synch_Except:
  sub sp, sp, #272
  stp x0, x1, [sp, #0]
  stp x2, x3, [sp, #16]
  stp x4, x5, [sp, #32]
  stp x6, x7, [sp, #48]
  stp x8, x9, [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  mrs x0, ELR_EL3
  stp x30, x0, [sp, #240]
  mrs x0, ESR_EL3
  mrs x1, FAR_EL3
  stp x0, x1, [sp, #256]
  b SWInterrupt

.balign 0x80
_IRQ_Except:
/* IRQ Exception */
  sub sp, sp, #272
  stp x0, x1, [sp, #0]
  stp x2, x3, [sp, #16]
  stp x4, x5, [sp, #32]
  stp x6, x7, [sp, #48]
  stp x8, x9, [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  mrs x0, ELR_EL3
  stp x30, x0, [sp, #240]
  mrs x0, ESR_EL3
  mrs x1, FAR_EL3
  b IRQInterrupt

.balign 0x80
_FIQ_Except:
/* FIQ Exception */
  sub sp, sp, #272
  stp x0, x1, [sp, #0]
  stp x2, x3, [sp, #16]
  stp x4, x5, [sp, #32]
  stp x6, x7, [sp, #48]
  stp x8, x9, [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  mrs x0, ELR_EL3
  stp x30, x0, [sp, #240]
  mrs x0, ESR_EL3
  mrs x1, FAR_EL3
  b FIQInterrupt

.balign 0x80
_SError_Except:
/* SError Exception */
  sub sp, sp, #272
  stp x0, x1, [sp, #0]
  stp x2, x3, [sp, #16]
  stp x4, x5, [sp, #32]
  stp x6, x7, [sp, #48]
  stp x8, x9, [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  mrs x0, ELR_EL3
  stp x30, x0, [sp, #240]
  mrs x0, ESR_EL3
  mrs x1, FAR_EL3
  b SError

/* EL is using SP_ELx stack */
.balign 0x80
  b _Synch_Except

.balign 0x80
  b _IRQ_Except

.balign 0x80
  b _FIQ_Except

.balign 0x80
  b _SError_Except

/* From lower EL in AArch64 */
.balign 0x80
  b _Synch_Except

.balign 0x80
  b _IRQ_Except

.balign 0x80
  b _FIQ_Except

.balign 0x80
  b _SError_Except

/* From lower EL in AArch32 */
.balign 0x80
  b _Synch_Except

.balign 0x80
  b _IRQ_Except

.balign 0x80
  b _FIQ_Except

.balign 0x80
  b _SError_Except
