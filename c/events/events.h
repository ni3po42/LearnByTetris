#ifndef _events_h_
#define _events_h_

#include <stdint.h>
#include "../generator/generator.h"

// opaque type to event queue
struct EventQueue;
typedef struct EventQueue* EventStreamHandle;

typedef uint64_t event_type;

/**
 * A basic event message
*/
typedef struct {
    
    // type of event
    event_type type;

} EventMessage;

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