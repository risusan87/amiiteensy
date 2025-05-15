
# include "imxrt.h"
#include <stdint.h>

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

    for(;;) {
        // HIGH
    GPIO7_DR_SET = (1 << 3);
    delay(10000000);

    // LOW
    GPIO7_DR_CLEAR = (1 << 3);
    delay(10000000);
    }
}
