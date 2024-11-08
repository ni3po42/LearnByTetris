#include "vga.h"
#include "Arduino.h"
#include <stdint.h>
#include <stdbool.h>
#include <cmath>

#include <freertos/semphr.h>
#include <cstddef>
#include <driver/gpio.h>

#include <hal/wdt_hal.h>
#include <driver/gptimer.h>

#include "freertos/xtensa_timer.h"
#include "esp_intr_alloc.h"

#define VLINES 525
#define HLINES 800
#define FPS 60
#define HFRONT_PORCH (16)
#define HBACK_PORCH (48)
#define HPIX (640)
#define HSYNC 96


uint64_t fonts[41] = {
    0x7EE3F3DBCFC7C37E,//0
    0x3C7C1C1C1C1CFFFF,//1
    0x7EE7C31E7CC0C3FF,//2
    0x7EE7C31E1EC3E77E,//3
    0x1C3C6CCCFFFF0C0C,//4
    0xFFFFC0FE07C3E77E,//5
    0x7EC7C0FEE7C3E77E,//6
    0x7FFF070E1C383838,//7
    0x7EE7C37EE7C3E77E,//8
    0x7EE7C3E77F03E77E,//9
    0x3C7EE7C3FFFFC3C3,//A
    0xFEFFC3FEFFC3FFFE,//B
    0x3F7FE0C0C0E07F3F,//C
    0xFEFFC7C3C3C7FFFE,//D
    0xFFFFC0FCFCC0FFFF,//E
    0xFFFFC0FCFCC0C0C0,//F
    0x7EFFE0CFCFE3FF7E,//G
    0xC3C3C3FFFFC3C3C3,//H
    0xFFFF18181818FFFF,//I
    0xFFFF181818D8F870,//J
    0xC7CFDCF8FCCEC7C7,//K
    0xC0C0C0C0C0C0FFFF,//L
    0xC3E7FFFFFFDBC3C3,//M
    0xC3E3F3FBDFCFC7C3,//N
    0x7EFFE7C3C3E7FF7E,//O
    0xFEFFC3C3FFFEC0C0,//P
    0x7EFFC3C3DBCFFE7B,//Q
    0xFEFFC3C3FFFCC6C7,//R
    0x7FFFC0FE7F03FFFE,//S
    0xFFFF181818181818,//T
    0xC3C3C3C3C3FFFF7E,//U
    0xC3C3C3C3E77E3C18,//V
    0xC3C3DBFFFFFFE7C3,//W
    0xC3E776381C6EE7C3,//X
    0xC3C3C3E77E3C1818,//Y
    0xFFFF071E78C0FFFF,//Z
    0x3C7E7E3C18001818,//!
    0x7EFFC31E18001818,//?
    0x0000000000001818,//.
    //0x7F415D5D5D417F00,// tetris piece
    0x7F415D5D5D417F00,// tetris piece
    0x0000000000000000// blank
};

wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};

volatile uint8_t vram[38400];
volatile uint8_t* vram_buffer = (uint8_t*)vram;

extern "C" void processActiveScanline(volatile uint8_t* ram, volatile uint32_t* gpio);

SemaphoreHandle_t vram_mutex;

volatile bool vramIsWritable = false;

void prepareWriteToVRAM() {
 //  xSemaphoreTake(vram_mutex, 900);
}

void completeWriteToVRAM() {
 //  xSemaphoreGive(vram_mutex);
}

void clearVRAM() {
    prepareWriteToVRAM();
    for(size_t i=0;i<38400;i++) {
        vram[i] = 0x00;
    }
    completeWriteToVRAM();
}

void testPattern() {
    for(size_t j=0;j<240;j++) {
    for(size_t i=0;i<320; i++) {        
        setPixel(j, i, 0x00);
    }
    }

    for(size_t color=0;color<8;color++) {
        
        for(size_t c=0;c<40;c++) {
            for(size_t r=0;r<120;r++) {
                setPixel(r, c + color * 40, color);
            }        
        }

    }

    for(size_t color=0;color<7;color++) {        
        for(size_t c=0;c<320;c++) {
            for(size_t r=0;r<10;r++) {
                setPixel(r + 120 + (10 * color), c, 7 - color);
            }        
        }
    }

    for(size_t j=0;j<2;j++) {
    for(size_t i=0;i<40; i++) {        
        setPixel(200 + i, i + j * 40, 0x07);
        setPixel(240 - i, i + j * 40, 0x07);
    }
    }

    for(size_t j=0;j<80;j+=2) {
        for(size_t i=0;i<40;i++) {

            setPixel(i+200, j + 80, 0x07);
        }
    }

    for(size_t j=0;j<2;j++) {
    for(float angle = 0; angle < 360; angle++) {
        int r = round(20*sin(angle * 0.017453291));
        int c = round(20*cos(angle * 0.017453291));
        setPixel(r+200 + 20, c + 180 + (40 * j), 0x07);
    }
    }

    for(size_t j=0;j<40;j+=2) {
        for(size_t i=0;i<80;i++) {

            setPixel(j + 200,i+240, 0x07);
        }
    }
    
    

    //test pattern    
    for(size_t i=0;i<240; i++) {       
        //left
        setPixel(i, 0, 0x07);

        //right
        setPixel(i, 319, 0x07);
    }
   
    for(size_t i=0;i<320; i++) {       
        // top
        setPixel(0, i, 0x07);

        // bottom
        setPixel(239, i, 0x07);
    }
}

void vga_init() {
    vram_mutex = xSemaphoreCreateMutex();

    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);

    //testPattern();
    
    // renderMessage("HEY ALBERT!", (size_t)10, (size_t)10, (uint8_t)0x07, (uint8_t)0x00);
    // renderMessage("I CREATED MY OWN FONTS!", (size_t)20, (size_t)10, (uint8_t)0x07, (uint8_t)0x00);
    // renderMessage("0123456789", (size_t)30, (size_t)10, (uint8_t)0x07, (uint8_t)0x00);
    // renderMessage("ABCDEFGHIJKLM", (size_t)40, (size_t)10, (uint8_t)0x07, (uint8_t)0x00);
    // renderMessage("NOPQRSTUVWZYZ", (size_t)50, (size_t)10, (uint8_t)0x07, (uint8_t)0x00);
    // renderMessage("...???!!!   aaa", (size_t)60, (size_t)10, (uint8_t)0x07, (uint8_t)0x00);
    //
    //

}

void killTheDogs() {
    wdt_hal_write_protect_disable(&wdt1_context);  
    wdt_hal_disable(&wdt1_context);

    //wdt_hal_feed(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);

    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    //wdt_hal_feed(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);
}

void feedTheDogs() {
    wdt_hal_write_protect_disable(&wdt1_context);  
    //wdt_hal_disable(&wdt1_context);

    wdt_hal_feed(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);

    wdt_hal_write_protect_disable(&wdt0_context);
    //wdt_hal_disable(&wdt0_context);
    wdt_hal_feed(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);
}

inline void wait_ticks(int ticks) {    
    int __ccountE;
    do { 
        asm volatile("rsr.ccount %0" : "=a"(__ccountE)); 
    } while(__ccountE < ticks);
} 

inline void reset_ticks() {
    int ___ccountS = 0;
    asm volatile("wsr.ccount %0" :: "a"(___ccountS):"memory"); \
}

void doVgaSignals() {
    //testPattern();
    killTheDogs();

    //points to bits 24-31, with 25,26,27 being the target GPIOs
    //uint8_t* rgb = (uint8_t*)(GPIO_OUT_REG + 0x0000);

   // portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
  //  taskENTER_CRITICAL(&myMutex);
   
    volatile uint16_t line = 0;    
     ESP_INTR_DISABLE(XT_TIMER_INTNUM);
        // super time critical stuff
       // ESP_INTR_ENABLE(XT_TIMER_INTNUM);
    for(;;) {
        reset_ticks();

        //start horz signal
        REG_WRITE(GPIO_OUT_W1TC_REG, (1<<5));

        if (line == 0) {
            //start vertical signal
            REG_WRITE(GPIO_OUT_W1TC_REG, (1<<23));    
        } else if (line == 2) {
            //end vertical signal
            REG_WRITE(GPIO_OUT_W1TS_REG, (1<<23));    
        }
                
        wait_ticks(610);
        REG_WRITE(GPIO_OUT_W1TS_REG, (1<<5));       
        //end horz signal

        //start back porch       
        wait_ticks(915);//.193644489
        //end back porch

        // start active video
        if (line == 34) {
           // xSemaphoreTake(vram_mutex, portMAX_DELAY);
         //   vramIsWritable = false;
        } else if (line > 34 && line < 515) {
            volatile uint8_t* mem = vram_buffer + (((line-35)>>1) * 160); 
            processActiveScanline(mem, (volatile uint32_t*)GPIO_OUT_REG);
            REG_WRITE(GPIO_OUT_W1TC_REG, (1<<25)|(1<<26)|(1<<27)); // clears all channels
        } else if (line == 515) {
           // vramIsWritable = true;
         //   xSemaphoreGive(vram_mutex);
        } else if (line == 516) {
            //
            //feedTheDogs();
        }
       
        wait_ticks(4982);        
        //end active video

        
        // start front porch        
        line = (line + 1) % VLINES;
        wait_ticks(5084);
        //end front porch
    }
 //critical section
    //taskEXIT_CRITICAL(&myMutex);
}

void setPixel(size_t row, size_t col, uint8_t color) {
    if (row >= 240 || col >= 320 || row < 0 || col < 0) {
        return;
    }

    size_t index = row * 160 + (col >> 1);
    if ((col & 0x01) == 0) {
        vram_buffer[index] = (vram_buffer[index] & 0x0F) | ((color << 5) & 0xF0);
    } else {
        vram_buffer[index] = (vram_buffer[index] & 0xF0) | ((color << 1) & 0x0F);
    }
}
static size_t maxMessageSize = 320;
void renderMessage(const char* message, size_t row, size_t col, uint8_t foregroundColor, uint8_t backgroundColor) {

    if (message == nullptr) {
        return;
    }

    prepareWriteToVRAM();
    char* messagePtr = (char*)message;
    char currentChar = *messagePtr;
    size_t placement = 0;
    size_t renderedSize = 0;
    while (currentChar != '\0' && renderedSize < maxMessageSize) {
        size_t pos;

        if (currentChar >= '0' && currentChar <= '9') {
            pos = currentChar - '0';
        } else if (currentChar >= 'A' && currentChar <= 'Z') {
            pos = currentChar - 'A' + 10;
        } else if (currentChar == '!') {
            pos = 36;
        } else if (currentChar == '?') {
            pos = 37;
        } else if (currentChar == '.') {
            pos = 38;
        } else if (currentChar == ' ') {
            pos = 40;
        } else {
            pos = 39;
        }

        uint64_t data = fonts[pos];

        for(size_t pr = 0;pr<8;pr++) {
            for(size_t pc = 0; pc<8;pc++) {
                uint64_t p = data & 0x8000000000000000;
                if (p > 0x0) {
                    setPixel(row + pr, col + pc + placement, foregroundColor);
                } else if ((backgroundColor & 0x08) != 0x08) {//ignore background otherwise
                    setPixel(row + pr, col + pc + placement, backgroundColor);
                }
                
                data = data << 1;
            }
        }
        messagePtr+=1;
        currentChar = *messagePtr;
        placement+=10;
        renderedSize++;
    }
    completeWriteToVRAM();
}
