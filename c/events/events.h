#ifndef _events_h_
#define _events_h_

#include <stdint.h>
#include "../generator/generator.h"

// opaque pointer to event queue
typedef struct EventQueue* EventStreamHandle;

typedef uint64_t event_type;

//if this becomes more generic, this will move somewhere else
#define NONE 0
#define DROP 1
#define LEFT 2
#define RIGHT 3
#define TURN_LEFT 4
#define TURN_RIGHT 5
#define PAUSE 6
#define EXIT_GAME 7

/**
 * A basic event message
*/
typedef struct {
    // type of event
    event_type type;

    // currently only used for testing
    void* data;
} EventMessage;

// Define event handler signature
typedef void EventListener(const EventMessage* message);

/**
 * Signal the start of the event queue. Will start handling event after this is called
 * @param handle - opaque pointer to queue context
*/
void startEventStream(EventStreamHandle handle);

/**
 * Signal the stop of the event queue.
 * @param handle - opaque pointer to queue context
*/
void stopEventStream(EventStreamHandle handle);

/**
 * free event queue resources
 * @param handle - opaque reference pointer to queue context
*/
void freeEventStream(EventStreamHandle* handle);

/**
 * Signals listeners that an event has occured
 * @param handle - opaque pointer to queue context
*/
void emit(EventStreamHandle handle, EventMessage message);

/**
 * Adds a direct listener to events
 * @param handle - opaque pointer to queue context
 * @param listener - function pointer to handle to execute upon emit
*/
void addEventListener(EventStreamHandle handle, EventListener* listener);

/**
 * Creates a generator function (coroutine) that yields next event message.
 * Caller must free GeneatorHandle when done
 * @param handle - opaque pointer to queue context
 * @return a generator handle. Caller is responsible for freeing this handle
*/
GeneratorHandle eventStreamAsGenerator(EventStreamHandle handle);

/**
 * Creates a new event stream (queue)
 * @return an opaque pointer to an event queue. Caller is responsible for freeing handle with freeEventStream
*/
EventStreamHandle createEventStreamHandle();

#endif