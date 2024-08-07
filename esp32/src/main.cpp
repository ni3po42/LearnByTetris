#ifndef UNIT_TEST



#include "Arduino.h"
// #include "events.h"
// #include "game/game.h"
// #include "io/io.h"
// #include "board.h"
// #include "core.h"
#include <cstdio>
#include <cstddef>
#include <driver/gpio.h>
#include <hal/wdt_hal.h>
#include <driver/gptimer.h>
#include <driver/rmt_common.h>
#include <driver/rmt_tx.h>
#include <cmath>


// __asm__(
// ".global vidtestf\n"
// "vidtestf:\n"
// "ret\n"
// );

// #include <driver/timer.h>

#define VLINES 525
#define HLINES 800
#define FPS 60
#define HFRONT_PORCH (16)
#define HBACK_PORCH (48)
#define HPIX (640)//640

#define HSYNC 96

// void gameGraphicsTask(void *args) {
//     //for(;;) {

//         //TickType_t ticks = xTaskGetTickCount();
//         //Serial.printf("t: %d\n", ticks);
//         //GPIO.out_w1ts = 1 << 23;
        
//         // GPIO.out_w1tc = 1 << 23;
//     //}
// }

// void gameEventUpdater(void *args) {
//     initInput();

//     for(;;) {
//         TickType_t shortDelay = 250 / portTICK_PERIOD_MS;
//         vTaskDelay(shortDelay);  
//         emit(DROP);
//     }
// }

// char messageBuffer[100];

// void gameStatusUpdate(GameStatus* status) {
//     if (status == NULL) {       
//         //renderDebug("Hello NONE!\n"); 
//         TickType_t shortDelay = 250 / portTICK_PERIOD_MS;
//         vTaskDelay(shortDelay);        
//         return;
//     } else {        
//         //sprintf(messageBuffer, "id: %d; active: %d\n", getPieceId(getCurrentPiece()), getPieceRow(getCurrentPiece()));
//         //renderDebug(messageBuffer); 
//         renderScreen(status);
//     }
//     //renderScreen(status);
//     //fprintf(stderr, "l:%d, s:%d", status.level, status.score);
    
//     //if (status.gameover) {
//     //    return;
//     //}
    
//     //if(status.leveledUp) {        
//         //interval = getInterval(level);
//         //resetDropInterval(interval);
//     //}
// }

// void gameLoopTask(void *args) {
//     gamelevel_t level = (gamelevel_t)args;
//     doGameLoop(level, gameStatusUpdate);
//     // renderInit();
//     // GameStatus status;   
//     // for(;;) {
//     //     TickType_t shortDelay = 500 / portTICK_PERIOD_MS;
//     //     vTaskDelay(shortDelay);
//     //     renderScreen(status);
//     // }
// }


volatile uint8_t vram[76800];
volatile uint8_t* vram_buffer1 = (uint8_t*)vram;
volatile uint8_t* vram_buffer2 = (uint8_t*)(vram + 38400);
volatile uint8_t* vram_buffer = vram_buffer1;

extern "C" void doVram(volatile uint8_t* ram, volatile uint32_t* gpio);

inline void wait_ticks(int ticks) {    
    int __ccountE;
    do { 
        __asm__ __volatile__("rsr.ccount %0" : "=a"(__ccountE)); 
    } while(__ccountE < ticks);
} 

inline void reset_ticks() {
    int ___ccountS = 0;
    //__asm__ __volatile__("wsr.ccount %0" : "=a"(___ccountS)); 
    __asm__ __volatile__("wsr.ccount %0" :: "a"(___ccountS):"memory"); \
}

void setVramPixel(size_t row, size_t col, uint8_t color) {
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

void runOnCore2(void* args) {
   
    setCpuFrequencyMhz(160); 

    for(size_t j=0;j<240;j++) {
    for(size_t i=0;i<320; i++) {        
        setVramPixel(j, i, 0x00);
    }
    }

    for(size_t color=0;color<8;color++) {
        
        for(size_t c=0;c<40;c++) {
            for(size_t r=0;r<120;r++) {
                setVramPixel(r, c + color * 40, color);
            }        
        }

    }

    for(size_t color=0;color<7;color++) {        
        for(size_t c=0;c<320;c++) {
            for(size_t r=0;r<10;r++) {
                setVramPixel(r + 120 + (10 * color), c, 7 - color);
            }        
        }
    }

    for(size_t j=0;j<4;j++) {
    for(size_t i=0;i<40; i++) {        
        setVramPixel(200 + i, i + j * 40, 0x07);
        setVramPixel(240 - i, i + j * 40, 0x07);
    }
    }

    for(size_t j=0;j<4;j++) {
    for(float angle = 0; angle < 360; angle++) {
        int r = round(20*sin(angle * 0.017453291));
        int c = round(20*cos(angle * 0.017453291));
        setVramPixel(r+200 + 20, c + 180 + (40 * j), 0x07);
    }
    }
    
    

    //test pattern    
    for(size_t i=0;i<240; i++) {       
        //left
        setVramPixel(i, 0, 0x07);

        //right
        setVramPixel(i, 319, 0x07);
    }
   
    for(size_t i=0;i<320; i++) {       
        // top
        setVramPixel(0, i, 0x07);

        // bottom
        setVramPixel(239, i, 0x07);
    }

    //points to bits 24-31, with 25,26,27 being the target GPIOs
    //uint8_t* rgb = (uint8_t*)(GPIO_OUT_REG + 0x0000);

    portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
    taskENTER_CRITICAL(&myMutex);
   
    volatile uint16_t line = 0;    
    
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
        if (line > 34 && line < 515) {
            volatile uint8_t* mem = vram_buffer + (((line-35)>>1) * 160); 
            doVram(mem, (volatile uint32_t*)GPIO_OUT_REG);
            REG_WRITE(GPIO_OUT_W1TC_REG, (1<<25)|(1<<26)|(1<<27)); // clears all channels
        }
        
        wait_ticks(4982);        
        //end active video

        // start front porch        
        line = (line + 1) % VLINES;
        wait_ticks(5084);
        //end front porch
    }
 //critical section
    taskEXIT_CRITICAL(&myMutex);

    vTaskDelete(NULL);
  
}

/*
    Initializes and prepares game to run
*/
void setup() 
{
//    Serial.begin(115200);
    // initRand();    
        
    // gamelevel_t level = 10;
    // interval_t interval = getInterval(level);
    
    // constructBoard(CONSTRUCT_WHOLE_BOARD);
    
    // renderInit();

    // resetDropInterval(interval);

    // TaskHandle_t gameLoopTaskHandle;
    // xTaskCreatePinnedToCore(
    //     gameLoopTask,"GameLoop", 10000, (void*)level,
    //     1, &gameLoopTaskHandle, 0
    // );

    // TaskHandle_t gameInputHandle;
    // xTaskCreatePinnedToCore(
    //     gameEventUpdater,"EventLoop", 10000, NULL,
    //     1, &gameInputHandle, 0
    // );
//rtc_wdt_protect_off();

//rtc_wdt_disable();



 wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};

 wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};

    //Disable IWDT (Timer Group 1)
    wdt_hal_write_protect_disable(&wdt1_context);    
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);

    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

//Serial.begin(115200);

   gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);


   gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
   gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);

    // pinMode(GPIO_NUM_5, OUTPUT);
    // pinMode(GPIO_NUM_23, OUTPUT);

    // pinMode(GPIO_NUM_25, OUTPUT);
//disableCore1WDT();

    TaskHandle_t videoHandle;
    xTaskCreatePinnedToCore(
        runOnCore2,"VideoTask", 1000, NULL,
        25, &videoHandle, 1
    );
   
    // TaskHandle_t gameGraphicsTaskHandle;
    // xTaskCreatePinnedToCore(
    //     gameGraphicsTask,"GameGraphics", 10000, NULL,
    //     1, &gameGraphicsTaskHandle, 1
    // );

}

void loop() {
    //do nothing
}

#endif
