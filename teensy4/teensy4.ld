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
  	.text.nor_config : {
		KEEP(*(.flashconfig))
		. = ALIGN(4);
	} > FLASH

	.text.boot : {
		. = ORIGIN(FLASH) + 0x1000;
		KEEP(*(.ivt))
		KEEP(*(.bootdata))
		KEEP(*(.vector_table))
		. = ALIGN(4);
	} > FLASH

	.text.code : {
		KEEP(*(.startup))
		. = ALIGN(4);
	} > FLASH


	_flashorigin = ORIGIN(FLASH);
  	_sp_top = ORIGIN(RAM) + LENGTH(RAM);
	_flashimagelen = SIZEOF(.text.nor_config);
}