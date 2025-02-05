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

_exe_offset = 0xFFFC0000;
/* Define Memories in the system */

MEMORY
{
   ps7_ram_0_S_AXI_BASEADDR : ORIGIN = 0x00000000, LENGTH = 0x00030000
   ps7_ram_1_S_AXI_BASEADDR : ORIGIN = 0xFFFF0000, LENGTH = 0x0000FE00
}

/* Specify the default entry point to the program */

ENTRY(_begin_lo)


SECTIONS
{
.text : {
   *(.begin_lo)
   _begin_hi_start_addr = .;
   *(.begin_hi)
   _begin_hi_end_addr = .;
   _vector_table_addr = . + _exe_offset;
   *(.vectors)
   *(.boot)
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
} > ps7_ram_0_S_AXI_BASEADDR

.init : {
   KEEP (*(.init))
} > ps7_ram_0_S_AXI_BASEADDR

.fini : {
   KEEP (*(.fini))
} > ps7_ram_0_S_AXI_BASEADDR

.rodata : {
   __rodata_start = . + _exe_offset;
   *(.rodata)
   *(.rodata.*)
   *(.gnu.linkonce.r.*)
   __rodata_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.rodata1 : {
   __rodata1_start = . + _exe_offset;
   *(.rodata1)
   *(.rodata1.*)
   __rodata1_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.sdata2 : {
   __sdata2_start = . + _exe_offset;
   *(.sdata2)
   *(.sdata2.*)
   *(.gnu.linkonce.s2.*)
   __sdata2_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.sbss2 : {
   __sbss2_start = . + _exe_offset;
   *(.sbss2)
   *(.sbss2.*)
   *(.gnu.linkonce.sb2.*)
   __sbss2_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.data : {
   __data_start = . + _exe_offset;
   *(.data)
   *(.data.*)
   *(.gnu.linkonce.d.*)
   *(.jcr)
   *(.got)
   *(.got.plt)
   __data_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.data1 : {
   __data1_start = . + _exe_offset;
   *(.data1)
   *(.data1.*)
   __data1_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.got : {
   *(.got)
} > ps7_ram_0_S_AXI_BASEADDR

.ctors : {
   __CTOR_LIST__ = . + _exe_offset;
   ___CTORS_LIST___ = . + _exe_offset;
   KEEP (*crtbegin.o(.ctors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .ctors))
   KEEP (*(SORT(.ctors.*)))
   KEEP (*(.ctors))
   __CTOR_END__ = . + _exe_offset;
   ___CTORS_END___ = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.dtors : {
   __DTOR_LIST__ = . + _exe_offset;
   ___DTORS_LIST___ = . + _exe_offset;
   KEEP (*crtbegin.o(.dtors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .dtors))
   KEEP (*(SORT(.dtors.*)))
   KEEP (*(.dtors))
   __DTOR_END__ = . + _exe_offset;
   ___DTORS_END___ = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.fixup : {
   __fixup_start = . + _exe_offset;
   *(.fixup)
   __fixup_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.eh_frame : {
   *(.eh_frame)
} > ps7_ram_0_S_AXI_BASEADDR

.eh_framehdr : {
   __eh_framehdr_start = . + _exe_offset;
   *(.eh_framehdr)
   __eh_framehdr_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.gcc_except_table : {
   *(.gcc_except_table)
} > ps7_ram_0_S_AXI_BASEADDR

.ARM.exidx : {
   __exidx_start = . + _exe_offset;
   *(.ARM.exidx*)
   *(.gnu.linkonce.armexidix.*.*)
   __exidx_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.preinit_array : {
   __preinit_array_start = . + _exe_offset;
   KEEP (*(SORT(.preinit_array.*)))
   KEEP (*(.preinit_array))
   __preinit_array_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.init_array : {
   __init_array_start = . + _exe_offset;
   KEEP (*(SORT(.init_array.*)))
   KEEP (*(.init_array))
   __init_array_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.fini_array : {
   __fini_array_start = . + _exe_offset;
   KEEP (*(SORT(.fini_array.*)))
   KEEP (*(.fini_array))
   __fini_array_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.ARM.attributes : {
   __ARM.attributes_start = . + _exe_offset;
   *(.ARM.attributes)
   __ARM.attributes_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.sdata : {
   __sdata_start = . + _exe_offset;
   *(.sdata)
   *(.sdata.*)
   *(.gnu.linkonce.s.*)
   __sdata_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.sbss (NOLOAD) : {
   __sbss_start = . + _exe_offset;
   *(.sbss)
   *(.sbss.*)
   *(.gnu.linkonce.sb.*)
   __sbss_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.tdata : {
   __tdata_start = . + _exe_offset;
   *(.tdata)
   *(.tdata.*)
   *(.gnu.linkonce.td.*)
   __tdata_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.tbss : {
   __tbss_start = . + _exe_offset;
   *(.tbss)
   *(.tbss.*)
   *(.gnu.linkonce.tb.*)
   __tbss_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

.bss (NOLOAD) : {
   __bss_start = . + _exe_offset;
   *(.bss)
   *(.bss.*)
   *(.gnu.linkonce.b.*)
   *(COMMON)
   __bss_end = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

_SDA_BASE_ = __sdata_start + ((__sbss_end - __sdata_start) / 2 );

_SDA2_BASE_ = __sdata2_start + ((__sbss2_end - __sdata2_start) / 2 );

/* Generate Stack and Heap definitions */

.heap (NOLOAD) : {
   . = ALIGN(16);
   _heap = .;
   HeapBase = . + _exe_offset;
   _heap_start = .;
   . += _HEAP_SIZE;
   _heap_end = . + _exe_offset;
   HeapLimit = . + _exe_offset;
} > ps7_ram_0_S_AXI_BASEADDR

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
} > ps7_ram_1_S_AXI_BASEADDR

_end = .;
}
