/* Linkerscript for Cubix kernel (ARM Cortex M4) */

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
	. = ORIGIN(ram);

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
		*(.text)           /* .text sections (code) */
    	*(.text*)          /* .text* sections (code) */
    	*(.rodata)         /* .rodata sections (constants, strings, etc.) */
    	*(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
		. = ALIGN(4);
	} > ram

	/* initialized data */
	.data :
	{
		. = ALIGN(4);
		*(.data)           /* .data sections */
    	*(.data*)          /* .data* sections */
		. = ALIGN(4);
	} > ram

	/* uninitialized data */
	.bss :
	{
		. = ALIGN(4);
		*(.bss)
    	*(.bss*)
    	*(COMMON)
		. = ALIGN(4);
	} > ram
}

/* constants */
_textStart = ADDR(.text);
_textSize = SIZEOF(.text);
_textEnd = _textStart + _textSize;

_dataStart = ADDR(.data);
_dataSize = SIZEOF(.data);
_dataEnd = _dataStart + _dataSize;

_bssStart = ADDR(.bss);
_bssSize = SIZEOF(.bss);
_bssEnd = _bssStart + _bssSize;

/* Stack configuration */
_stackSize = 5K;
_stackEnd = ALIGN(_bssEnd, 32); /* align stack end with 32 because of stack corruption detection */
_stackStart = _stackEnd + _stackSize;

/* Heap configuration */
_heapStart = ORIGIN(ccm); /* ccm ram start */
_heapSize = LENGTH(ccm);

/* Userspace configuration */
_userspaceStart = _stackStart + 4;
_userspaceEnd = ORIGIN(ram) + LENGTH(ram);
_userspaceSize = _userspaceEnd - _userspaceStart;
