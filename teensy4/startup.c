#include <stdint.h>
#include "utils.h"
#include "imxrt.h"

extern int main(void);
extern uint32_t _sitcm, _itcm_start, _itcm_end;
extern uint32_t _sdata, _dtcm_data_start, _dtcm_data_end;
extern uint32_t _sbss, _ebss;
extern void (* const VectorTable[])(void);

// peripherals
extern void usb1_init(void);

__attribute__((section(".startup"), used))
void ResetHandler(void)
{ 
    // FlexRAM allocation
    // RAM size = 512KB, total 16 banks of 32KB section
    // 0b00 = not used, 0b01 = OCRAM, 0b10 = DTCM, 0b11 = ITCM
    // 0b11111111111111111010101001010101
    // 0x   F   F   F   F   A   A   5   5 
    // TODO: Dynamic configiration within linker script
    IOMUXC_GPR_GPR16 |= (1 << 2);
    IOMUXC_GPR_GPR17 = 0xFFFFAA55;
    // Unknown purpose but this will write into reserved bits
    // IOMUXC_GPR_GPR14 = 0x00AA0000;

    // Vector table pointer set
    SCB_VTOR = (uint32_t) &VectorTable;

    // move executable code to ITCM
    // due to the limited space, prioritize ISRs!
    memcpy(&_itcm_start, &_sitcm, (size_t)&_itcm_end - (size_t)&_itcm_start);

    // Both data and bss are intended to be in DTCM
    // move .data to DTCM
    memcpy(&_dtcm_data_start, &_sdata, (size_t)&_dtcm_data_end - (size_t)&_dtcm_data_start);
    // clear .bss
    memset(&_sbss, 0, (size_t)&_ebss - (size_t)&_sbss);

    // GPIO B0_03 (PIN 13) for output
    IOMUXC_GPR_GPR27 = 0xFFFFFFFF;
    GPIO7_GDIR |= (1 << 3);
    
    // enable USB OTG 1 for CDC
    usb1_init();

    
    main();
}