
#include <stdint.h>

#define IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03 (*(volatile uint32_t *)0x401F8148)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_03 (*(volatile uint32_t *)0x401F8338)

#define IOMUXC_GPR_GPR27 (*(volatile uint32_t *)0x400AC06C)

#define GPIO7_GDIR      (*(volatile uint32_t *)0x42004004)

#define GPIO7_DR_SET    (*(volatile uint32_t *)0x42004084)
#define GPIO7_DR_CLEAR  (*(volatile uint32_t *)0x42004088)

#define IOMUXC_PAD_DSE(n) ((uint32_t)(((n)&0x07) << 3))

#define DELAY_LENGTH 10000000

// Freq = 600MHz
void delay(int duration) {
    volatile unsigned int i = 0;
    while(i < duration) {
        i++;
    }
}

int main()
{
    // Configure GPIO B0_03 (PIN 13) for output
    IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03 = 5;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_03 = IOMUXC_PAD_DSE(7);
    IOMUXC_GPR_GPR27 = 0xFFFFFFFF;
    GPIO7_GDIR |= (1 << 3);

    for(;;) {

        // HIGH
        GPIO7_DR_SET = (1 << 3);
        delay(DELAY_LENGTH);

        // LOW
        GPIO7_DR_CLEAR = (1 << 3);
        delay(DELAY_LENGTH);
    }
}
