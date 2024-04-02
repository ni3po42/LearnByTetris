#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>
#include <pthread.h>

#include "../events/events.h"
#include "input.h"

void initInput() {

    initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/

}

void freeInput() {
    endwin();
}

static void* startInputLoop_func(void* input) {
 
    InputLoopArgument* argument = (InputLoopArgument*)input;
    EventStreamHandle handle = argument->eventStreamHandle;
    free(argument);
    
    int c;
    bool keepReading = true;
    EventMessage message = { .type = NONE };
    while(keepReading)
    {	
        c = getch();
        
        switch(c)
        {
            case KEY_DOWN:
                message.type = DROP;
                break;
            case KEY_LEFT:
                message.type = LEFT;
                break;
            case KEY_RIGHT:
                message.type = RIGHT;
                break;
            case 'z':
                message.type = TURN_LEFT;
                break;
            case 'c':
                message.type = TURN_RIGHT;
                break;
            case 27:
                message.type = EXIT_GAME;
                keepReading = false;
                break;
            default:
                message.type = NONE;
        }
        
        if (message.type != NONE) {
            emit(handle, message);
        }
    }
    return NULL;
}

void startInputLoop(EventStreamHandle eventStreamHandle) {
    
//     raw();	
//  	keypad(stdscr, TRUE);
//     noecho();

    pthread_t thread_id;
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    InputLoopArgument* argument = (InputLoopArgument*)malloc(sizeof(InputLoopArgument));
    argument->eventStreamHandle = eventStreamHandle;
    pthread_create(&thread_id, &attr, startInputLoop_func, argument);
}

static void* resetDropInterval_func(void* input) {
    
    DropIntervalInput* argument = (DropIntervalInput*)input;
    EventStreamHandle handle = argument->eventStreamHandle;
    interval_t interval = argument->interval;
    
    free(argument);
    
    struct timespec ts;
    
    EventMessage message = { .type = DROP };
    
    while(1) {
    
        ts.tv_sec = interval / 1000;
        ts.tv_nsec = (interval % 1000) * 1000000;
    
        nanosleep(&ts, &ts);
        
        emit(handle, message);
    }
    
}

void startDropInterval(EventStreamHandle eventStreamHandle, interval_t interval) {
    pthread_t thread_id;
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    DropIntervalInput* argument = (DropIntervalInput*)malloc(sizeof(DropIntervalInput));
    argument->eventStreamHandle = eventStreamHandle;
    argument->interval = interval;
    pthread_create(&thread_id, &attr, resetDropInterval_func, argument);
}

void resetDropInterval(interval_t interval) {
   
}



