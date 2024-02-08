
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/types.h>
#include <unistd.h>

#include "events/events.h"
#include "generator/generator.h"
#include "game/game.h"
#include "render/render.h"
#include "input/input.h"
#include "utilities.h"
#include "board/board.h"

int main(int argc, char *argv[]) 
{
    initRand();
    
    initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
    
    gamelevel_t level = 10;
    interval_t interval = getInterval(level);
    GameStatus status;

    constructBoard(-1);

    renderInit();

    EventStreamHandle eventStream = createEventStreamHandle();
    GeneratorHandle eventGeneratorHandle =  eventStreamAsGenerator(eventStream);
    
    startInputLoop(eventStream);
    startEventStream(eventStream);
    
    GeneratorHandle gameLoop = doGameLoop(eventGeneratorHandle, level);
    
    startDropInterval(eventStream, interval);
    
    while(gen_next(gameLoop, NULL, &status)) {
        
        renderScreen(status);
     
        if (status.gameover) {
            break;
        }
        
        if(level != status.level) {
            level = status.level;
            //interval = getInterval(level);
            //resetDropInterval(interval);
        }
    }
    
    stopEventStream(eventStream);
    
    renderCleanup();
    
    freeGenerator(&gameLoop);
    freeGenerator(&eventGeneratorHandle);
    //freeGenerator(&dropInterval);
    freeEventStream(&eventStream);
    
    endwin();
}

