ENTRY(_start)

SECTIONS {
	. = 0x00000;

	.vectors 0x00000 : ALIGN(32) {
		KEEP(*(.vectors))
	}

	. = 0x10000;

	.text : {
		*(.text*)
	}

	.rodata : {
		*(.rodata*)
	}

	.data : {
		__data_start = .;
		*(.data*)
		__data_end = .;
		__data_load = LOADADDR(.data);
	}

	.bss : {
		__bss_start = .;
		*(.bss*)
		*(COMMON)
		__bss_end = .;
	}


	__stack_top = 0x10000;
}
