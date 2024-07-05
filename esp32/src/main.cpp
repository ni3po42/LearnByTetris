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

# define XTHAL_GET_CCOUNT()	({ int __ccount; \
				__asm__ __volatile__("rsr.ccount %0" : "=a"(__ccount)); \
				__ccount; })

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

void setVramPixel(size_t row, size_t col, uint8_t color) {
    size_t index = row * 160 + (col >> 1);
    if (col & 0x01 == 0) {
        vram_buffer[index] = (vram_buffer[index] & 0x0F) | ((color << 4) & 0xF0);
    } else {
        vram_buffer[index] = (vram_buffer[index] & 0xF0) | (color & 0x0F);
    }
}

void runOnCore2(void* args) {
   
    setCpuFrequencyMhz(160); 

    //test pattern
    uint8_t val = 0;
    for(size_t i=0;i<240; i++) {
       
        //left
        setVramPixel(i, 0, 0x0E);

        //right
        setVramPixel(i, 319, 0x0E);
    }
   
    for(size_t i=0;i<320; i++) {
       
    //    size_t row = (i * 3) / 4;
    //     setVramPixel(row, i, 0x0E);
    //     setVramPixel(239 - row, i, 0x0E);

        // top
        setVramPixel(0, i, 0x0E);

        // bottom
        setVramPixel(239, i, 0x0E);

    }

    //points to bits 24-31, with 25,26,27 being the target GPIOs
    //uint8_t* rgb = (uint8_t*)(GPIO_OUT_REG + 0x0000);

    portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
    taskENTER_CRITICAL(&myMutex);
   
    volatile uint16_t line = 0;
    
    for(;;) {

        //start horz signal
        REG_WRITE(GPIO_OUT_W1TC_REG, (1<<5));
        bool cond = (line < 2);
        uint32_t reg = cond * GPIO_OUT_W1TC_REG + (!cond) * GPIO_OUT_W1TS_REG;
        REG_WRITE(reg, (1<<23));
        
        asm volatile(
            ".rept 610 \n\t"//605
            "nop \n\t"
            ".endr \n\t"
            //:::
        );

        REG_WRITE(GPIO_OUT_W1TS_REG, (1<<5));
        //end horz signal
       
        //start back porch
        asm volatile(
            ".rept 305 \n\t"//305
            "nop \n\t"
            ".endr \n\t"
            //:::
        );
        //end back porch


        // active video
        bool belowTop = line > 34;
        bool aboveBottom = line < 515;

        bool inRange = belowTop & aboveBottom;//not an error, we want a bit op, no short circuit call!
        
        //reg = inRange * GPIO_OUT_W1TS_REG + (!inRange) * GPIO_OUT_W1TC_REG;
        //REG_WRITE(reg, (1<<25));
        
        //end active video
       
        

        if (inRange) {
           // int c = XTHAL_GET_CCOUNT();
            volatile uint8_t* mem = vram_buffer + (((line-35)>>1) * 160); 
            doVram(mem, (volatile uint32_t*)GPIO_OUT_REG);
            //c = XTHAL_GET_CCOUNT();
        } else {
             asm volatile(//3954
            ".rept 4067 \n\t"//3950
            "nop \n\t"
            ".endr \n\t"            
            );
        }

        // clear video channel
        REG_WRITE(GPIO_OUT_W1TC_REG, (1<<25)|(1<<26)|(1<<27));

        line = line + 1;
        line = line % VLINES;

        //REG_WRITE(GPIO_OUT_W1TC_REG, (1<<25));

        //start front porch
        asm volatile(
            ".rept 101 \n\t"
            "nop \n\t"
            ".endr \n\t"
            //:::
        );
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

gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);

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
