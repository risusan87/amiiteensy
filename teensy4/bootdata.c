#include "imxrt.h"

#include <stdint.h>

__attribute__((weak)) void Default_Handler(void) { while (1); }
extern void ResetHandler(void);
extern unsigned long _flashimagelen;
extern uint32_t _flashorigin;
extern uint32_t _sp_top;

// ISR
extern void usb1_isr(void);

__attribute__((used, aligned(128), section(".vector_table")))
void (* const VectorTable[])(void) = 
{
    (void*)&_sp_top,	// stack pointer
    ResetHandler,		// program entry

	[IRQ_USB1 + 16] = usb1_isr	// USB1
}; 

__attribute__ ((section(".bootdata"), used))
const uint32_t BootData[3] = 
{
	(uint32_t) &_flashorigin,
	(uint32_t) &_flashimagelen,
	0
};

/**
*	Image vector table must be placed at location 0x60001000 as referred to image vector table offset.
*	page 256
*	pointer to this table must also be shared with SCB_VTOR
**/
__attribute__ ((section(".ivt"), used))
const uint32_t ImageVectorTable[8] = 
{
	0x402000D1,		// header
	(uint32_t)VectorTable,// program entry - Apparently this must point to the vector table
	0,			// reserved
	0,			// dcd (not supported in teensy4)
	(uint32_t)BootData,	// abs address of boot data
	(uint32_t)ImageVectorTable, // self
	0,		// command sequence file (ignored for now)
	0			// reserved
};

/**
* MCU_Flashloader_Reference_Manual.pdf, 8.2.1, Table 8-2, page 72-75
*
* Particularly Teensy 4.0 is serial NOR flash via FlexSPI:
* https://www.pjrc.com/teensy/IMXRT1060RM_rev3_annotations.pdf#page=222
**/
__attribute__ ((section(".flashconfig"), used))
uint32_t FlexSPI_NOR_Config[128] = 
{
	// Refered from core/teensy4
	0x42464346,		// Tag	Ascii: FCFB			0x00
	0x56010000,		// Version
	0,				// reserved
	0x00030301,		// columnAdressWidth,dataSetupTime,dataHoldTime,readSampleClkSrc

	0x00000000,		// waitTimeCfgCommands,-,deviceModeCfgEnable
	0,			// deviceModeSeq
	0, 			// deviceModeArg
	0x00000000,		// -,-,-,configCmdEnable

	0,			// configCmdSeqs		0x20
	0,
	0,
	0,

	0,			// cfgCmdArgs			0x30
	0,
	0,
	0,

	0x00000000,		// controllerMiscOption		0x40
	0x00030401,		// lutCustomSeqEnable, serialClkFreq, sflashPadType, deviceType
	0,			// reserved
	0,			// reserved


    /**
    * Teensy 4.0: 0x00200000
    * Teensy 4.1: 0x00800000
    * Teensy Micromod: 0x01000000
    **/
	0x00200000,		// sflashA1Size			0x50


	0,			// sflashA2Size
	0,			// sflashB1Size
	0,			// sflashB2Size

	0,			// csPadSettingOverride		0x60
	0,			// sclkPadSettingOverride
	0,			// dataPadSettingOverride
	0,			// dqsPadSettingOverride

	0,			// timeoutInMs			0x70
	0,			// commandInterval
	0,			// dataValidTime
	0x00000000,		// busyBitPolarity,busyOffset

	0x0A1804EB,		// lookupTable[0]		0x80
	0x26043206,		// lookupTable[1]
	0,			// lookupTable[2]
	0,			// lookupTable[3]

	0x24040405,		// lookupTable[4]		0x90
	0,			// lookupTable[5]
	0,			// lookupTable[6]
	0,			// lookupTable[7]

	0,			// lookupTable[8]		0xA0
	0,			// lookupTable[9]
	0,			// lookupTable[10]
	0,			// lookupTable[11]

	0x00000406,		// lookupTable[12]		0xB0
	0,			// lookupTable[13]
	0,			// lookupTable[14]
	0,			// lookupTable[15]

	0,			// lookupTable[16]		0xC0
	0,			// lookupTable[17]
	0,			// lookupTable[18]
	0,			// lookupTable[19]

	0x08180420,		// lookupTable[20]		0xD0
	0,			// lookupTable[21]
	0,			// lookupTable[22]
	0,			// lookupTable[23]

	0,			// lookupTable[24]		0xE0
	0,			// lookupTable[25]
	0,			// lookupTable[26]
	0,			// lookupTable[27]

	0,			// lookupTable[28]		0xF0
	0,			// lookupTable[29]
	0,			// lookupTable[30]
	0,			// lookupTable[31]

	0x081804D8,		// lookupTable[32]		0x100
	0,			// lookupTable[33]
	0,			// lookupTable[34]
	0,			// lookupTable[35]

	0x08180402,		// lookupTable[36]		0x110
	0x00002004,		// lookupTable[37]
	0,			// lookupTable[38]
	0,			// lookupTable[39]

	0,			// lookupTable[40]		0x120
	0,			// lookupTable[41]
	0,			// lookupTable[42]
	0,			// lookupTable[43]

	0x00000460,		// lookupTable[44]		0x130
	0,			// lookupTable[45]
	0,			// lookupTable[46]
	0,			// lookupTable[47]

	0,			// lookupTable[48]		0x140
	0,			// lookupTable[49]
	0,			// lookupTable[50]
	0,			// lookupTable[51]

	0,			// lookupTable[52]		0x150
	0,			// lookupTable[53]
	0,			// lookupTable[54]
	0,			// lookupTable[55]

	0,			// lookupTable[56]		0x160
	0,			// lookupTable[57]
	0,			// lookupTable[58]
	0,			// lookupTable[59]

	0,			// lookupTable[60]		0x170
	0,			// lookupTable[61]
	0,			// lookupTable[62]
	0,			// lookupTable[63]

	// LUT configurations are ignored since lutCustomSeqEnable is 0
	// https://www.pjrc.com/teensy/IMXRT1060RM_rev3_annotations.pdf#page=224
	0,			// LUT 0: Read			0x180
	0,			// LUT 1: ReadStatus
	0,			// LUT 3: WriteEnable
	0,			// LUT 5: EraseSector

	0,			// LUT 9: PageProgram		0x190
	0,			// LUT 11: ChipErase
	0,			// LUT 15: Dummy

	// RESERVED section (?)
	0,			// LUT unused?

	0,			// LUT unused?			0x1A0
	0,			// LUT unused?
	0,			// LUT unused?
	0,			// LUT unused?

	0,			// reserved			0x1B0
	0,			// reserved
	0,			// reserved
	0,			// reserved

	// 64 byte Serial NOR configuration block, 8.6.3.2, page 346
	// Section up to now is literally called "memCfg" in the reference manual

	256,			// pageSize			0x1C0
	4096,			// sectorSize
	1,			// ipCmdSerialClkFreq
	0,			// reserved

	0x00010000,		// block size			0x1D0
	0,			// reserved
	0,			// reserved
	0,			// reserved

	0,			// reserved			0x1E0
	0,			// reserved
	0,			// reserved
	0,			// reserved

	0,			// reserved			0x1F0
	0,			// reserved
	0,			// reserved
	0			// reserved
};

