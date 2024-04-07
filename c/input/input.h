#ifndef __input_h_
#define __input_h_

#include "../generator/generator.h"
#include "../game/game.h"

void startInputLoop(EventStreamHandle eventStreamHandle);
void startDropInterval(EventStreamHandle eventStreamHandle, interval_t interval);
void resetDropInterval(interval_t interval);
void initInput();
void freeInput();

#endif