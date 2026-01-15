/*******************************************************************/
/*                                                                 */
/* (c) Copyright 2012-2013 Xilinx, Inc. All rights reserved        */
/*                                                                 */
/* Description : FSBL Linker Script                                */
/*                                                                 */
/*******************************************************************/

_STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x6000;
_HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x2000;

_ABORT_STACK_SIZE = DEFINED(_ABORT_STACK_SIZE) ? _ABORT_STACK_SIZE : 1024;
_SUPERVISOR_STACK_SIZE = DEFINED(_SUPERVISOR_STACK_SIZE) ? _SUPERVISOR_STACK_SIZE : 2048;
_FIQ_STACK_SIZE = DEFINED(_FIQ_STACK_SIZE) ? _FIQ_STACK_SIZE : 1024;
_UNDEF_STACK_SIZE = DEFINED(_UNDEF_STACK_SIZE) ? _UNDEF_STACK_SIZE : 1024;

AARCH64_MMU_TRANSLATION_TABLE_PAGES = 64;
AARCH64_MMU_TT_PAGE_SIZE = 0x1000 * AARCH64_MMU_TRANSLATION_TABLE_PAGES;

bsp_translation_table_base = 0x00020000;
bsp_translation_table_end = bsp_translation_table_base + AARCH64_MMU_TT_PAGE_SIZE;

VECTOR_TABLE_BASE = 0x800;
VECTOR_TABLE_LENGTH = 0x800;

/* Define Memories in the system */

MEMORY
{
   DDRMC0_region0_mem_BASEADDR : ORIGIN = 0x00000000000, LENGTH = 0x00800000000
}

/* Specify the default entry point to the program */

ENTRY(_boot)

SECTIONS
{
.vectors VECTOR_TABLE_BASE : {
   . = ALIGN(0x800);
   _vector_table = .;
   __vector_table = _vector_table;
   *(.vectors)
   . = _vector_table + VECTOR_TABLE_LENGTH;
} > DDRMC0_region0_mem_BASEADDR

.boot : {
   . = ALIGN(0x1000);
   *(.boot)
} > DDRMC0_region0_mem_BASEADDR

.text : {
   *(.text)
   *(.text.*)
   *(.gnu.linkonce.t.*)
   *(.plt)
   *(.gnu_warning)
   *(.gcc_execpt_table)
   *(.glue_7)
   *(.glue_7t)
   *(.vfp11_veneer)
   *(.ARM.extab)
   *(.gnu.linkonce.armextab.*)
} > DDRMC0_region0_mem_BASEADDR

.init : {
   KEEP (*(.init))
} > DDRMC0_region0_mem_BASEADDR

.fini : {
   KEEP (*(.fini))
} > DDRMC0_region0_mem_BASEADDR

.rodata : {
   __rodata_start = .;
   *(.rodata)
   *(.rodata.*)
   *(.gnu.linkonce.r.*)
   __rodata_end = .;
} > DDRMC0_region0_mem_BASEADDR

.rodata1 : {
   __rodata1_start = .;
   *(.rodata1)
   *(.rodata1.*)
   __rodata1_end = .;
} > DDRMC0_region0_mem_BASEADDR

.sdata2 : {
   __sdata2_start = .;
   *(.sdata2)
   *(.sdata2.*)
   *(.gnu.linkonce.s2.*)
   __sdata2_end = .;
} > DDRMC0_region0_mem_BASEADDR

.sbss2 : {
   __sbss2_start = .;
   *(.sbss2)
   *(.sbss2.*)
   *(.gnu.linkonce.sb2.*)
   __sbss2_end = .;
} > DDRMC0_region0_mem_BASEADDR

.data : {
   __data_start = .;
   *(.data)
   *(.data.*)
   *(.gnu.linkonce.d.*)
   *(.jcr)
   *(.got)
   *(.got.plt)
   __data_end = .;
} > DDRMC0_region0_mem_BASEADDR

.data1 : {
   __data1_start = .;
   *(.data1)
   *(.data1.*)
   __data1_end = .;
} > DDRMC0_region0_mem_BASEADDR

.got : {
   *(.got)
} > DDRMC0_region0_mem_BASEADDR

.ctors : {
   __CTOR_LIST__ = .;
   ___CTORS_LIST___ = .;
   KEEP (*crtbegin.o(.ctors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .ctors))
   KEEP (*(SORT(.ctors.*)))
   KEEP (*(.ctors))
   __CTOR_END__ = .;
   ___CTORS_END___ = .;
} > DDRMC0_region0_mem_BASEADDR

.dtors : {
   __DTOR_LIST__ = .;
   ___DTORS_LIST___ = .;
   KEEP (*crtbegin.o(.dtors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .dtors))
   KEEP (*(SORT(.dtors.*)))
   KEEP (*(.dtors))
   __DTOR_END__ = .;
   ___DTORS_END___ = .;
} > DDRMC0_region0_mem_BASEADDR

.fixup : {
   __fixup_start = .;
   *(.fixup)
   __fixup_end = .;
} > DDRMC0_region0_mem_BASEADDR

.eh_frame : {
   *(.eh_frame)
} > DDRMC0_region0_mem_BASEADDR

.eh_framehdr : {
   __eh_framehdr_start = .;
   *(.eh_framehdr)
   __eh_framehdr_end = .;
} > DDRMC0_region0_mem_BASEADDR

.gcc_except_table : {
   *(.gcc_except_table)
} > DDRMC0_region0_mem_BASEADDR

.ARM.exidx : {
   __exidx_start = .;
   *(.ARM.exidx*)
   *(.gnu.linkonce.armexidix.*.*)
   __exidx_end = .;
} > DDRMC0_region0_mem_BASEADDR

.preinit_array : {
   __preinit_array_start = .;
   KEEP (*(SORT(.preinit_array.*)))
   KEEP (*(.preinit_array))
   __preinit_array_end = .;
} > DDRMC0_region0_mem_BASEADDR

.init_array : {
   __init_array_start = .;
   KEEP (*(SORT(.init_array.*)))
   KEEP (*(.init_array))
   __init_array_end = .;
} > DDRMC0_region0_mem_BASEADDR

.fini_array : {
   __fini_array_start = .;
   KEEP (*(SORT(.fini_array.*)))
   KEEP (*(.fini_array))
   __fini_array_end = .;
} > DDRMC0_region0_mem_BASEADDR

.ARM.attributes : {
   __ARM.attributes_start = .;
   *(.ARM.attributes)
   __ARM.attributes_end = .;
} > DDRMC0_region0_mem_BASEADDR

.sdata : {
   __sdata_start = .;
   *(.sdata)
   *(.sdata.*)
   *(.gnu.linkonce.s.*)
   __sdata_end = .;
} > DDRMC0_region0_mem_BASEADDR

.sbss (NOLOAD) : {
   __sbss_start = .;
   *(.sbss)
   *(.sbss.*)
   *(.gnu.linkonce.sb.*)
   __sbss_end = .;
} > DDRMC0_region0_mem_BASEADDR

.tdata : {
   __tdata_start = .;
   *(.tdata)
   *(.tdata.*)
   *(.gnu.linkonce.td.*)
   __tdata_end = .;
} > DDRMC0_region0_mem_BASEADDR

.tbss : {
   __tbss_start = .;
   *(.tbss)
   *(.tbss.*)
   *(.gnu.linkonce.tb.*)
   __tbss_end = .;
} > DDRMC0_region0_mem_BASEADDR

.bss (NOLOAD) : {
   __bss_start = .;
   *(.bss)
   *(.bss.*)
   *(.gnu.linkonce.b.*)
   *(COMMON)
   __bss_end = .;
} > DDRMC0_region0_mem_BASEADDR

_SDA_BASE_ = __sdata_start + ((__sbss_end - __sdata_start) / 2 );

_SDA2_BASE_ = __sdata2_start + ((__sbss2_end - __sdata2_start) / 2 );

/* Generate Stack and Heap definitions */

.heap (NOLOAD) : {
   . = ALIGN(16);
   _heap = .;
   HeapBase = .;
   _heap_start = .;
   . += _HEAP_SIZE;
   _heap_end = .;
   HeapLimit = .;
} > DDRMC0_region0_mem_BASEADDR

.stack (NOLOAD) : {
   . = ALIGN(16);
   _stack_end = .;
   . += _STACK_SIZE;
   _stack = .;
   __stack = _stack;
   . = ALIGN(16);
   _irq_stack_end = .;
   . += _STACK_SIZE;
   __irq_stack = .;
   _supervisor_stack_end = .;
   . += _SUPERVISOR_STACK_SIZE;
   . = ALIGN(16);
   __supervisor_stack = .;
   _abort_stack_end = .;
   . += _ABORT_STACK_SIZE;
   . = ALIGN(16);
   __abort_stack = .;
   _fiq_stack_end = .;
   . += _FIQ_STACK_SIZE;
   . = ALIGN(16);
   __fiq_stack = .;
   _undef_stack_end = .;
   . += _UNDEF_STACK_SIZE;
   . = ALIGN(16);
   __undef_stack = .;
} > DDRMC0_region0_mem_BASEADDR

_end = .;
}
