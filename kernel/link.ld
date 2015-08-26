/* Linkerscript for Aurora kernel (ARM Cortex M4) */

/* Handler for startup */
ENTRY(jmpCode)

/* Sections */
SECTIONS
{
	. = 0x20000000;

	.jmpCode :
	{
		. = ALIGN(4);
		*(.jmpCode*)
		. = ALIGN(4);
	}

	. = 0x20000004;
	
	/* interrupt vector table */
	.vector_table :
	{
		. = ALIGN(4);
		KEEP(*(.vector_table))
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
