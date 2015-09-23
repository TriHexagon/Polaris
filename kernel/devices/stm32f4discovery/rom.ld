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
	/* interrupt vector table */
	.ivector :
	{
		. = ALIGN(4);
		KEEP(*(.ivector))
		. = ALIGN(4);
	} > rom

	/* extended interrupt vector (implemented by drivers) */
	.ivector_ext :
	{
		. = ALIGN(4);
		KEEP(*(.ivector_ext))
		. = ALIGN(4);
	} > rom

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
	} > rom

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
	} > ccm AT > rom
	_dataSourceStart = LOADADDR(.data);

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
	} > ccm

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
		. = ORIGIN(ram);
		_userspaceStart = .;
		. = ORIGIN(ram) + LENGTH(ram);
		_userspaceEnd = .;
		_userspaceSize = _userspaceEnd - _userspaceStart;
	} > ram
}
