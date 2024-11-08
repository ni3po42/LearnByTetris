// #include <stdint.h>

// void processActiveScanline(uint8_t* ram, uint32_t* gpio) {

//     uint8_t* end = ram + 160;        
//     uint32_t mask = 0xF0FFFFFF & *gpio;
    
//     while (ram < end) {   
    
//         *gpio = mask | ((uint32_t)(*ram & 0xF0) << 20);
//         asm ("nop \n\t");
//         *gpio = mask | ((uint32_t)(*ram & 0x0F) << 24);        
        
//         ram++;
//     }
    
// }