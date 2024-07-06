#include <stdint.h>

void doVram(volatile uint8_t* ram, volatile uint32_t* gpio) {

    volatile uint8_t* end = ram + 160;    
    volatile uint8_t* ramByte = (uint8_t*)gpio + 3;    
    uint8_t mask = 0x0F & *gpio;
    uint8_t temp;
    
    while (ram < end) {
        temp = *ram;
        *gpio = mask | (temp << 28);
        *gpio = mask | (temp << 24);
        ram++;
    }
    
}