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

/*
 * Entry point for the boot loader.
 */

#include "xil_errata.h"

.globl _start
.globl _vector_table

.set PSS_L2CC_BASE_ADDR, 0xF8F02000

.set TblBase ,		0x00020000
.set L2CCWay,		(PSS_L2CC_BASE_ADDR + 0x077C)	/*(PSS_L2CC_BASE_ADDR + PSS_L2CC_CACHE_INVLD_WAY_OFFSET)*/
.set L2CCSync,		(PSS_L2CC_BASE_ADDR + 0x0730)	/*(PSS_L2CC_BASE_ADDR + PSS_L2CC_CACHE_SYNC_OFFSET)*/
.set L2CCCrtl,		(PSS_L2CC_BASE_ADDR + 0x0100)	/*(PSS_L2CC_BASE_ADDR + PSS_L2CC_CNTRL_OFFSET)*/
.set L2CCAuxCrtl,	(PSS_L2CC_BASE_ADDR + 0x0104)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_AUX_CNTRL_OFFSET)*/
.set L2CCTAGLatReg,	(PSS_L2CC_BASE_ADDR + 0x0108)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_TAG_RAM_CNTRL_OFFSET)*/
.set L2CCDataLatReg,	(PSS_L2CC_BASE_ADDR + 0x010C)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_DATA_RAM_CNTRL_OFFSET)*/
.set L2CCIntClear,	(PSS_L2CC_BASE_ADDR + 0x0220)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_IAR_OFFSET)*/
.set L2CCIntRaw,	(PSS_L2CC_BASE_ADDR + 0x021C)	/*(PSS_L2CC_BASE_ADDR + XPSS_L2CC_ISR_OFFSET)*/

.set CRValMmuCac,	0b01000000000101	/* Enable IDC, and MMU */

.set L2CCAuxControl,	0x72360000		/* Enable all prefetching, Cache replacement policy, Parity enable,
        						Event monitor bus enable and Way Size (64 KB) */
.set L2CCTAGLatency,	0x0111			/* latency for TAG RAM */
.set L2CCDataLatency,	0x0121			/* latency for DATA RAM */

/* Stack Pointer locations for boot code */
.set Undef_stack,	__undef_stack
.set FIQ_stack,		__fiq_stack
.set Abort_stack,	__abort_stack
.set SPV_stack,		__supervisor_stack
.set IRQ_stack,		__irq_stack
.set SYS_stack,		__stack

        .section .vectors
	.arm
_vector_table:
	b	_boot

        .section .boot, "ax"
_boot:
	/*set scu enable bit in scu*/
	ldr	r7, =0xf8f00000
	ldr	r0, [r7]
	orr	r0, r0, #0x1
	str	r0, [r7]

	/*invalidate scu*/
	ldr	r7, =0xf8f0000c
	ldr	r6, =0xffff
	str	r6, [r7]

	/* Write to ACTLR */
	mrc	p15, 0, r0, c1, c0, 1		/* Read ACTLR*/
	orr	r0, r0, #(0x01 << 6)		/* set SMP bit */
	orr	r0, r0, #(0x01 )		/* */
	mcr	p15, 0, r0, c1, c0, 1		/* Write ACTLR*/

	/* Invalidate caches and TLBs */
	mov	r0,#0				/* r0 = 0  */
	mcr	p15, 0, r0, c8, c7, 0		/* invalidate TLBs */
	mcr	p15, 0, r0, c7, c5, 0		/* invalidate icache */
	mcr	p15, 0, r0, c7, c5, 6		/* Invalidate branch predictor array */
	bl	invalidate_dcache		/* invalidate dcache */

	ldr	r0,=L2CCCrtl			/* Load L2CC base address base + control register */
	mov	r1, #0				/* force the disable bit */
	str	r1, [r0]			/* disable the L2 Caches */

	ldr	r0,=L2CCAuxCrtl			/* Load L2CC base address base + Aux control register */
	ldr	r1,[r0]				/* read the register */
	ldr	r2,=L2CCAuxControl		/* set the default bits */
	orr	r1,r1,r2
	str	r1, [r0]			/* store the Aux Control Register */

	ldr	r0,=L2CCTAGLatReg		/* Load L2CC base address base + TAG Latency address */
	ldr	r1,=L2CCTAGLatency		/* set the latencies for the TAG*/
	str	r1, [r0]			/* store the TAG Latency register Register */

	ldr	r0,=L2CCDataLatReg		/* Load L2CC base address base + Data Latency address */
	ldr	r1,=L2CCDataLatency		/* set the latencies for the Data*/
	str	r1, [r0]			/* store the Data Latency register Register */

	ldr	r0,=L2CCWay			/* Load L2CC base address base + way register*/
	ldr	r2, =0xFFFF
	str	r2, [r0]			/* force invalidate */

	ldr	r0,=L2CCSync			/* need to poll 0x730, PSS_L2CC_CACHE_SYNC_OFFSET */
						/* Load L2CC base address base + sync register*/
	/* poll for completion */
L2Sync:	ldr	r1, [r0]
	cmp	r1, #0
	bne	L2Sync

	ldr	r0,=L2CCIntRaw			/* clear pending interrupts */
	ldr	r1,[r0]
	ldr	r0,=L2CCIntClear
	str	r1,[r0]

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the irq stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x12			/* IRQ mode */
	msr	cpsr, r2
	ldr	r13,=IRQ_stack			/* IRQ stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the supervisor stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x13			/* supervisor mode */
	msr	cpsr, r2
	ldr	r13,=SPV_stack			/* Supervisor stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the Abort  stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x17			/* Abort mode */
	msr	cpsr, r2
	ldr	r13,=Abort_stack		/* Abort stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the FIQ stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x11			/* FIQ mode */
	msr	cpsr, r2
	ldr	r13,=FIQ_stack			/* FIQ stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the Undefine stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x1b			/* Undefine mode */
	msr	cpsr, r2
	ldr	r13,=Undef_stack		/* Undefine stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the system stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x1F			/* SYS mode */
	msr	cpsr, r2
	ldr	r13,=SYS_stack			/* SYS stack pointer */

	/* Disable MMU, if enabled */
	mrc	p15, 0, r0, c1, c0, 0		/* read CP15 register 1 */
	bic	r0, r0, #0x1			/* clear bit 0 */
	mcr	p15, 0, r0, c1, c0, 0		/* write value back */
	dsb
	isb

	/* enable MMU and cache */
	add	r0, pc, #1
	bx	r0
.thumb
	bl      _setupMMU                       /* Initialise the MMU table. */
	.align 2
	bx	pc
.arm
	ldr	r0,=TblBase			/* Load MMU translation table base */
	orr	r0, r0, #0x5B			/* Outer-cacheable, WB */
	mcr	15, 0, r0, c2, c0, 0		/* TTB0 */


	mvn	r0,#0				/* Load MMU domains -- all ones=manager */
	mcr	p15,0,r0,c3,c0,0

	/* Enable mmu, icahce and dcache */
	ldr	r0,=CRValMmuCac

	mcr	p15,0,r0,c1,c0,0		/* Enable cache and MMU */
	dsb					/* dsb	allow the MMU to start up */

	isb					/* isb	flush prefetch buffer */

	mrc	p15,0,r0,c1,c0,0		/* flow prediction enable */
	orr	r0, r0, #(0x01 << 11)		/* #0x8000 */
	mcr	p15,0,r0,c1,c0,0

	mrc	p15,0,r0,c1,c0,1		/* read Auxiliary Control Register */
	orr	r0, r0, #(0x1 << 2)		/* enable Dside prefetch */
	orr	r0, r0, #(0x1 << 1)		/* enable L2 Prefetch hint */
	mcr	p15,0,r0,c1,c0,1		/* write Auxiliary Control Register */

	b	_start
.Ldone:	b	.Ldone				/* Paranoia: we should never get here */

/*
 *************************************************************************
 *
 * invalidate_dcache - invalidate the entire d-cache by set/way
 *
 * Note: for Cortex-A9, there is no cp instruction for invalidating
 * the whole D-cache. Need to invalidate each line.
 *
 *************************************************************************
 */
invalidate_dcache:
	mrc	p15, 1, r0, c0, c0, 1		/* read CLIDR */
	ands	r3, r0, #0x7000000
	mov	r3, r3, lsr #23			/* cache level value (naturally aligned) */
	beq	finished
	mov	r10, #0				/* start with level 0 */
loop1:
	add	r2, r10, r10, lsr #1		/* work out 3xcachelevel */
	mov	r1, r0, lsr r2			/* bottom 3 bits are the Cache type for this level */
	and	r1, r1, #7			/* get those 3 bits alone */
	cmp	r1, #2
	blt	skip				/* no cache or only instruction cache at this level */
	mcr	p15, 2, r10, c0, c0, 0		/* write the Cache Size selection register */
	isb					/* isb to sync the change to the CacheSizeID reg */
	mrc	p15, 1, r1, c0, c0, 0		/* reads current Cache Size ID register */
	and	r2, r1, #7			/* extract the line length field */
	add	r2, r2, #4			/* add 4 for the line length offset (log2 16 bytes) */
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		/* r4 is the max number on the way size (right aligned) */
	clz	r5, r4				/* r5 is the bit position of the way size increment */
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		/* r7 is the max number of the index size (right aligned) */
loop2:
	mov	r9, r4				/* r9 working copy of the max way size (right aligned) */
loop3:
	orr	r11, r10, r9, lsl r5		/* factor in the way number and cache number into r11 */
	orr	r11, r11, r7, lsl r2		/* factor in the index number */
	mcr	p15, 0, r11, c7, c14, 2		/* clean & invalidate by set/way */
	subs	r9, r9, #1			/* decrement the way number */
	bge	loop3
	subs	r7, r7, #1			/* decrement the index */
	bge	loop2
skip:
	add	r10, r10, #2			/* increment the cache number */
	cmp	r3, r10
	bgt	loop1

finished:
	mov	r10, #0				/* swith back to cache level 0 */
	mcr	p15, 2, r10, c0, c0, 0		/* select current cache level in cssr */
	dsb
	isb

	bx	lr

.end
