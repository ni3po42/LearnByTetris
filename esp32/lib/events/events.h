#ifndef _events_h_
#define _events_h_

#include <stdbool.h>

#define NO_EVENT 0

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned long EventMessage;

/**
 * Inits the event queue
*/
void initEventQueue();

/**
 * Signals listeners that an event has occured
 * @param message - message to send
 * @return true if message added, false if not
*/
bool emit(EventMessage message);

/**
 * Creates a new event stream (queue)
 * @param gets next message on queue
*/
void getNextMessage(EventMessage* message);

#ifdef __cplusplus
}
#endif


#endif