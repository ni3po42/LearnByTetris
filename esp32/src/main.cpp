#ifndef UNIT_TEST

#include "esp32-hal-cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "events.h"
#include "game/game.h"
#include "io/io.h"
#include "board.h"
#include "core.h"
#include "vga.h"

#include <cstdio>
#include <cstddef>
#include <driver/gpio.h>

void gameEventUpdater(void *args) {
    renderMessage("fuck2", 20, 20, 0x07, 0x08);   
    TickType_t shortDelay = 500 / portTICK_PERIOD_MS;
    for(;;) {              
        vTaskDelay(shortDelay);  
        emit(DROP);
    }
    
    vTaskDelete(NULL);  
}

char messageBuffer[100];

void gameStatusUpdate(GameStatus* status) {
      
    if (status == NULL) { 
    
        TickType_t shortDelay = 250 / portTICK_PERIOD_MS;
        vTaskDelay(shortDelay);          
        return;
    } else {   
        renderScreen(status);
    }
    
    if (status->gameover) {
       return;
    }
    
    if(status->leveledUp) {        
        interval_t interval = getInterval(status->level);
        resetDropInterval(interval);
    }
}

void gameLoopTask(void *args) {
    renderMessage("fuck", 10, 10, 0x07, 0x08);
    gamelevel_t level = (gamelevel_t)args;
    doGameLoop(level, gameStatusUpdate);    
    vTaskDelete(NULL);  
}



void handleVideoTask(void* args) {   
    setCpuFrequencyMhz(160); 
    doVgaSignals();
    vTaskDelete(NULL);  
}

/*
    Initializes and prepares game to run
*/
//void setup() 
void setup()
{
    initRand();    
        
    gamelevel_t level = 10;
    interval_t interval = getInterval(level);
    
    constructBoard(CONSTRUCT_WHOLE_BOARD);
    
    renderInit();
    // resetDropInterval(interval);

   initInput();

    TaskHandle_t gameLoopTaskHandle;
    xTaskCreatePinnedToCore(
        gameLoopTask,"GameLoop", 10000, (void*)level,
        1, &gameLoopTaskHandle, 0
    );

    TaskHandle_t gameInputHandle;
    xTaskCreatePinnedToCore(
        gameEventUpdater,"EventLoop", 10000, NULL,
        1, &gameInputHandle, 0
    );

    TaskHandle_t videoHandle;
    xTaskCreatePinnedToCore(
        handleVideoTask,"VideoTask", 1000, NULL,
        25, &videoHandle, 1
    );

   //
}

void loop() {
    vTaskDelete(NULL); 
}

#endif
