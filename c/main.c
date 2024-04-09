
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "events/events.h"
#include "generator/generator.h"
#include "game/game.h"
#include "render/render.h"
#include "input/input.h"
#include "utilities.h"
#include "board/board.h"

/*
    Initializes and prepares game to run
*/
int main(int argc, char *argv[]) 
{
    initRand();    
    initInput();
    
    gamelevel_t level = 10;
    interval_t interval = getInterval(level);
    GameStatus status;

    constructBoard(CONSTRUCT_WHOLE_BOARD);

    renderInit();

    EventStreamHandle eventStream = createEventStreamHandle();
    GeneratorHandle eventGeneratorHandle =  eventStreamAsGenerator(eventStream);
    
    startInputLoop(eventStream);
    
    GeneratorHandle gameLoop = doGameLoop(eventGeneratorHandle, level);
    
    startDropInterval(eventStream, interval);
    
    while(gen_next(gameLoop, NULL, &status)) {
        
        renderScreen(status);
     
        if (status.gameover) {
            sleep(3);
            break;
        }
        
        if(level != status.level) {
            level = status.level;
            //interval = getInterval(level);
            //resetDropInterval(interval);
        }
    }
    
    renderCleanup();
    
    freeGenerator(&gameLoop);
    freeGenerator(&eventGeneratorHandle);
    //freeGenerator(&dropInterval);
    freeEventStream(&eventStream);
    
    freeInput();
}

