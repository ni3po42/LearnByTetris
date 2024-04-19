#ifndef UNIT_TEST

#include "Arduino.h"
#include "events.h"
#include "game/game.h"
#include "io/io.h"
#include "board.h"
#include "core.h"
#include <cstdio>
#include <cstddef>

        
void gameGraphicsTask(void *args) {

}

void gameEventUpdater(void *args) {
    for(;;) {
        TickType_t shortDelay = 250 / portTICK_PERIOD_MS;
        vTaskDelay(shortDelay);  
        emit(DROP);
    }
}

char messageBuffer[100];

void gameStatusUpdate(GameStatus* status) {

    if (status == NULL) {       
        //renderDebug("Hello NONE!\n"); 
        TickType_t shortDelay = 250 / portTICK_PERIOD_MS;
        vTaskDelay(shortDelay);        
        return;
    } else {        
        //sprintf(messageBuffer, "id: %d; active: %d\n", getPieceId(getCurrentPiece()), getPieceRow(getCurrentPiece()));
        //renderDebug(messageBuffer); 
        renderScreen(status);
    }
    //renderScreen(status);
    //fprintf(stderr, "l:%d, s:%d", status.level, status.score);
    
    //if (status.gameover) {
    //    return;
    //}
    
    //if(status.leveledUp) {        
        //interval = getInterval(level);
        //resetDropInterval(interval);
    //}
}

void gameLoopTask(void *args) {
    gamelevel_t level = (gamelevel_t)args;
    doGameLoop(level, gameStatusUpdate);
    // renderInit();
    // GameStatus status;   
    // for(;;) {
    //     TickType_t shortDelay = 500 / portTICK_PERIOD_MS;
    //     vTaskDelay(shortDelay);
    //     renderScreen(status);
    // }
}

/*
    Initializes and prepares game to run
*/
void setup() 
{
    initRand();    
    // initInput();
    
    gamelevel_t level = 10;
    interval_t interval = getInterval(level);
    
    constructBoard(CONSTRUCT_WHOLE_BOARD);
    
    renderInit();

    resetDropInterval(interval);

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