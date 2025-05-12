#include <stdint.h>
#include "utils.h"
#include "imxrt.h"

extern int main(void);
extern uint32_t _sitcm, _itcm_start, _itcm_end;
extern uint32_t _sdata, _dtcm_data_start, _dtcm_data_end;
extern uint32_t _sbss, _ebss;

void uart_init(void);
void uart1_putchar(char c);
char uart1_getchar(void);

__attribute__((section(".startup"), used))
void ResetHandler(void)
{ 
    // move executable code to ITCM
    memcpy(&_itcm_start, &_sitcm, (size_t)&_itcm_end - (size_t)&_itcm_start);
    // move .data to DTCM
    memcpy(&_dtcm_data_start, &_sdata, (size_t)&_dtcm_data_end - (size_t)&_dtcm_data_start);
    // clear .bss
    memset(&_sbss, 0, (size_t)&_ebss - (size_t)&_sbss);

    // Enable UART1
    uart_init();

    print("Hello, world!\n");
    print("Press any key to continue...\n");
    main();
}

void uart_init(void)
{
    // LPUART1 clock enable
    CCM_CCGR5 |= CCM_CCGR5_LPUART1(CCM_CCGR_ON);
    // LPUART1 TX/RX sets to ALT2
    IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_12 = 2;
    IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_13 = 2;

    LPUART1_CTRL &= ~(1 << 18);
    LPUART1_CTRL &= ~(1 << 19);

    LPUART1_BAUD = 0xE;

    LPUART1_CTRL = 0;

    LPUART1_CTRL |= (1 << 18) | (1 << 19);

}

void uart1_putchar(char c) {
    while (!(LPUART1_STAT & (1 << 23))); // wait for TXEMPT
    LPUART1_DATA = c;
}

char uart1_getchar(void) {
    while (!(LPUART1_STAT & (1 << 21))); // wait for RDRF
    return LPUART1_DATA;
}

void print(const char *s) {
    while (*s) {
        if (*s == '\n') uart1_putchar('\r'); // optional CR before LF
        uart1_putchar(*s++);
    }
}