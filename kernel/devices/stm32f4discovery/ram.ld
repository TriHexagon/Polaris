/* The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Christopher Cichos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/* Linkerscript for Polaris kernel (ARM Cortex M4) */

/* Handler for startup */
ENTRY(handler_reset)

MEMORY
{
	rom (rx)	: ORIGIN = 0x08000000,	LENGTH = 1M
	ram (rwx)	: ORIGIN = 0x20000000,	LENGTH = 128K
	ccm (rw)	: ORIGIN = 0x10000000, 	LENGTH = 64K
}

/* Sections */
SECTIONS
{
	/* interrupt vector table */
	.ivector :
	{
		. = ALIGN(4);
		KEEP(*(.ivector))
		. = ALIGN(4);
	} > ram

	/* extended interrupt vector (implemented by drivers) */
	.ivector_ext :
	{
		. = ALIGN(4);
		KEEP(*(.ivector_ext))
		. = ALIGN(4);
	} > ram

	/* Program code and read only data (RAM) */
	.text :
	{
		. = ALIGN(4);
		_textStart = .;
		*(.text)           /* .text sections (code) */
    	*(.text*)          /* .text* sections (code) */
    	*(.rodata)         /* .rodata sections (constants, strings, etc.) */
    	*(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
		. = ALIGN(4);
		_textEnd = .;
		_textSize = _textEnd - _textStart;
	} > ram

	/* initialized data */
	.data :
	{
		. = ALIGN(4);
		_dataStart = .;
		*(.data)           /* .data sections */
    	*(.data*)          /* .data* sections */
		. = ALIGN(4);
		_dataEnd = .;
		_dataSize = _dataEnd - _dataStart;
	} > ram

	/* uninitialized data */
	.bss :
	{
		. = ALIGN(4);
		_bssStart = .;
		*(.bss)
    	*(.bss*)
    	*(COMMON)
		. = ALIGN(4);
		_bssEnd = .;
		_bssSize = _bssEnd - _bssStart;
	} > ram

	/* kernel stack */
	.stack :
	{
		/* align stack end with 32 because of stack corruption detection */
		. = ALIGN(32);
		_stackSize = 5K;
		_stackEnd = .;
		. = . + _stackSize;
		_stackStart = .;
		. = ALIGN(4);
	} > ccm

	/* kernel heap */
	.heap :
	{
		. = ALIGN(4);
		_heapStart = .;
		. = ORIGIN(ccm) + LENGTH(ccm);
		_heapEnd = .;
		_heapSize = _heapEnd - _heapStart;
	} > ccm

	/* userspace */
	.userspace :
	{
		_userspaceStart = .;
		. = ORIGIN(ram) + LENGTH(ram);
		_userspaceEnd = .;
		_userspaceSize = _userspaceEnd - _userspaceStart;
	} > ram
}
