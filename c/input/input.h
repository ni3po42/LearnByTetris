#ifndef __input_h_
#define __input_h_

#include "../generator/generator.h"

#include "../game/game.h"

typedef struct DropIntervalInput {
    interval_t interval;
    EventStreamHandle eventStreamHandle;
} DropIntervalInput;

typedef struct InputLoopArgument {
    EventStreamHandle eventStreamHandle;
} InputLoopArgument;

void startInputLoop(EventStreamHandle eventStreamHandle);
//void stopInputLoop();


void startDropInterval(EventStreamHandle eventStreamHandle, interval_t interval);
void resetDropInterval(interval_t interval);
//void stopDropInterval();

#endif