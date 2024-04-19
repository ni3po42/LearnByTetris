#ifndef _events_internal_h_
#define _events_internal_h_

#include "events.h"
#include <stdlib.h>

#define eventListSize 16
static EventMessage circularEventQueue[eventListSize];
static size_t frontIndex;
static size_t endIndex;

#endif