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

OCM_RAM_BASEADDR = 0x00FFFC0000;

ATF_RETURN_ADDRESS = OCM_RAM_BASEADDR;
ATF_RETURN_LENGTH = 0x100;

FACTORY_ATF_RETURN_ADDRESS = ATF_RETURN_ADDRESS + ATF_RETURN_LENGTH;

VECTOR_TABLE_BASE = ATF_RETURN_ADDRESS + ATF_RETURN_LENGTH;
VECTOR_TABLE_LENGTH = 0x800;


/* Define Memories in the system */

MEMORY
{
   OCM_RAM_0 : ORIGIN = OCM_RAM_BASEADDR, LENGTH = 0x000003D000
}

/* Specify the default entry point to the program */

ENTRY(_boot)

SECTIONS
{
.atf_return_sec ATF_RETURN_ADDRESS : {
  *(.atf_return_sec)
  . = ALIGN(0x80);
  *(.factory_atf_return_sec)
} > OCM_RAM_0

.vectors VECTOR_TABLE_BASE : {
   . = ALIGN(0x800);
   _vector_table = .;
   __vector_table = _vector_table;
   *(.vectors)
   . = _vector_table + VECTOR_TABLE_LENGTH;
} > OCM_RAM_0

.boot : {
   . = ALIGN(0x1000);
   *(.boot)
} > OCM_RAM_0


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
} > OCM_RAM_0

.init : {
   KEEP (*(.init))
} > OCM_RAM_0

.fini : {
   KEEP (*(.fini))
} > OCM_RAM_0

.rodata : {
   __rodata_start = .;
   *(.rodata)
   *(.rodata.*)
   *(.gnu.linkonce.r.*)
   __rodata_end = .;
} > OCM_RAM_0

.rodata1 : {
   __rodata1_start = .;
   *(.rodata1)
   *(.rodata1.*)
   __rodata1_end = .;
} > OCM_RAM_0

.sdata2 : {
   __sdata2_start = .;
   *(.sdata2)
   *(.sdata2.*)
   *(.gnu.linkonce.s2.*)
   __sdata2_end = .;
} > OCM_RAM_0

.sbss2 : {
   __sbss2_start = .;
   *(.sbss2)
   *(.sbss2.*)
   *(.gnu.linkonce.sb2.*)
   __sbss2_end = .;
} > OCM_RAM_0

.data : {
   __data_start = .;
   *(.data)
   *(.data.*)
   *(.gnu.linkonce.d.*)
   *(.jcr)
   *(.got)
   *(.got.plt)
   __data_end = .;
} > OCM_RAM_0

.data1 : {
   __data1_start = .;
   *(.data1)
   *(.data1.*)
   __data1_end = .;
} > OCM_RAM_0

.got : {
   *(.got)
} > OCM_RAM_0

.ctors : {
   __CTOR_LIST__ = .;
   ___CTORS_LIST___ = .;
   KEEP (*crtbegin.o(.ctors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .ctors))
   KEEP (*(SORT(.ctors.*)))
   KEEP (*(.ctors))
   __CTOR_END__ = .;
   ___CTORS_END___ = .;
} > OCM_RAM_0

.dtors : {
   __DTOR_LIST__ = .;
   ___DTORS_LIST___ = .;
   KEEP (*crtbegin.o(.dtors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .dtors))
   KEEP (*(SORT(.dtors.*)))
   KEEP (*(.dtors))
   __DTOR_END__ = .;
   ___DTORS_END___ = .;
} > OCM_RAM_0

.fixup : {
   __fixup_start = .;
   *(.fixup)
   __fixup_end = .;
} > OCM_RAM_0

.eh_frame : {
   *(.eh_frame)
} > OCM_RAM_0

.eh_framehdr : {
   __eh_framehdr_start = .;
   *(.eh_framehdr)
   __eh_framehdr_end = .;
} > OCM_RAM_0

.gcc_except_table : {
   *(.gcc_except_table)
} > OCM_RAM_0

.ARM.exidx : {
   __exidx_start = .;
   *(.ARM.exidx*)
   *(.gnu.linkonce.armexidix.*.*)
   __exidx_end = .;
} > OCM_RAM_0

.preinit_array : {
   __preinit_array_start = .;
   KEEP (*(SORT(.preinit_array.*)))
   KEEP (*(.preinit_array))
   __preinit_array_end = .;
} > OCM_RAM_0

.init_array : {
   __init_array_start = .;
   KEEP (*(SORT(.init_array.*)))
   KEEP (*(.init_array))
   __init_array_end = .;
} > OCM_RAM_0

.fini_array : {
   __fini_array_start = .;
   KEEP (*(SORT(.fini_array.*)))
   KEEP (*(.fini_array))
   __fini_array_end = .;
} > OCM_RAM_0

.ARM.attributes : {
   __ARM.attributes_start = .;
   *(.ARM.attributes)
   __ARM.attributes_end = .;
} > OCM_RAM_0

.sdata : {
   __sdata_start = .;
   *(.sdata)
   *(.sdata.*)
   *(.gnu.linkonce.s.*)
   __sdata_end = .;
} > OCM_RAM_0

.sbss (NOLOAD) : {
   __sbss_start = .;
   *(.sbss)
   *(.sbss.*)
   *(.gnu.linkonce.sb.*)
   __sbss_end = .;
} > OCM_RAM_0

.tdata : {
   __tdata_start = .;
   *(.tdata)
   *(.tdata.*)
   *(.gnu.linkonce.td.*)
   __tdata_end = .;
} > OCM_RAM_0

.tbss : {
   __tbss_start = .;
   *(.tbss)
   *(.tbss.*)
   *(.gnu.linkonce.tb.*)
   __tbss_end = .;
} > OCM_RAM_0

.bss (NOLOAD) : {
   __bss_start = .;
   *(.bss)
   *(.bss.*)
   *(.gnu.linkonce.b.*)
   *(COMMON)
   __bss_end = .;
} > OCM_RAM_0

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
} > OCM_RAM_0

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
} > OCM_RAM_0

_end = .;
}
