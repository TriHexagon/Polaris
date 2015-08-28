/* Linkerscript for Aurora kernel (ARM Cortex M4) */

/* Handler for startup */
ENTRY(handler_reset)

/* Sections */
SECTIONS
{
	. = 0x20000000;
	
	/* interrupt vector table */
	.ivector :
	{
		. = ALIGN(4);
		KEEP(*(.ivector))
		. = ALIGN(4);
	}

	/* extended interrupt vector (implemented by drivers) */
	.ivector_ext :
	{
		. = ALIGN(4);
		KEEP(*(.ivector_ext))
		. = ALIGN(4);
	}

	/* Program code and readonly data (RAM) */
	.text :
	{
		. = ALIGN(4);
		*(.text)           /* .text sections (code) */
    	*(.text*)          /* .text* sections (code) */
    	*(.rodata)         /* .rodata sections (constants, strings, etc.) */
    	*(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
		. = ALIGN(4);
	}

	/* initialized data */
	.data :
	{
		. = ALIGN(4);
		_dataStart = .;
		*(.data)           /* .data sections */
    	*(.data*)          /* .data* sections */
		. = ALIGN(4);
		_dataEnd = .;
	}

	/* unitialized data */
	.bss :
	{
		. = ALIGN(4);
		_bssStart = .;
		*(.bss)
    	*(.bss*)
    	*(COMMON)
		. = ALIGN(4);
		_bssEnd = .;
	}
}

/* Stack config */
_stackSize = 5120; /* 5 kiB */
_stackStart = _bssEnd + _stackSize;
