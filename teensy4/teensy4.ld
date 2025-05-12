MEMORY
{
	ITCM (rwx):  ORIGIN = 0x00000000, LENGTH = 512K
	DTCM (rwx):  ORIGIN = 0x20000000, LENGTH = 512K
	RAM (rwx):   ORIGIN = 0x20200000, LENGTH = 512K
	FLASH (rwx): ORIGIN = 0x60000000, LENGTH = 1984K
}

ENTRY(ImageVectorTable)

SECTIONS
{
  	.text.boot : {
		KEEP(*(.flashconfig))
		FILL(0xFF)
		. = ORIGIN(FLASH) + 0x1000;
		KEEP(*(.ivt))
		. = ALIGN(16);
	} > FLASH

	.text.startup : {
		KEEP(*(.vector_table))
		KEEP(*(.bootdata))
		. = ALIGN(4);
	} > FLASH

	.text.code : {
		KEEP(*(.startup))
		. = ALIGN(4);
	} > FLASH

	.text.main : {
		*(.text*)
		. = ALIGN(4);
	} > ITCM AT > FLASH

	.stack (NOLOAD) : {
		. = ORIGIN(DTCM);
		*(.stack*)
		. = ORIGIN(DTCM) + 0x2000;
	} > DTCM

	.data : {
		*(.data*)
		. = . + 0x2000;
	} > DTCM AT > FLASH

	.bss : {
		*(.bss*)
		. = . + 0x2000;
	} > DTCM


	_sp_top = ORIGIN(DTCM) + 0x2000;

	_sitcm = LOADADDR(.text.main);
	_eitcm = _sitcm + SIZEOF(.text.main);
	_itcm_start = ADDR(.text.main);
	_itcm_end   = _itcm_start + SIZEOF(.text.main);

	_sdata = LOADADDR(.data);
	_edata = _sdata + SIZEOF(.data);
	_dtcm_data_start = ADDR(.data);
	_dtcm_data_end = _dtcm_data_start + SIZEOF(.data);

	_sbss = ADDR(.bss);
	_ebss = _sbss + SIZEOF(.bss);

	_flashorigin = ORIGIN(FLASH);
	_flashimagelen = SIZEOF(.text.boot) + SIZEOF(.text.startup) + SIZEOF(.text.code) + SIZEOF(.text.main);
}