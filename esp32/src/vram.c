#include <stdint.h>

void doVram(volatile uint8_t* ram, volatile uint32_t* gpio) {

    volatile uint8_t* end = ram + 160;    
    volatile uint8_t* ramByte = (uint8_t*)gpio + 3;    
    uint8_t mask = 0x0F & *gpio;
    uint8_t temp;

    while (ram < end) {
        temp = *ram;
        *gpio = mask | (temp << 28);
        asm(
//            ".rept 5 \n\t"
            "nop \n\t"
            "nop \n\t"
            "nop \n\t"
            "nop \n\t"
  //          ".endr \n\t"
        );
        *gpio = mask | (temp << 24);
        asm(
    //        ".rept 5 \n\t"
            "nop \n\t"
            "nop \n\t"
            "nop \n\t"
            "nop \n\t"
     //       ".endr \n\t"
        );
        ram++;
    }
}