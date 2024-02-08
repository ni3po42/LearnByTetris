#ifndef _events_h_
#define _events_h_

#include <stdint.h>
#include "../generator/generator.h"

typedef int event_fd_t;
//typedef uintptr_t EventStreamHandle;


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

typedef struct {
    event_type type;
    void* data;
} EventMessage;

typedef void EventListener(const EventMessage* message);

void startEventStream(EventStreamHandle handle);
void stopEventStream(EventStreamHandle handle);
void freeEventStream(EventStreamHandle* handle);
void emit(EventStreamHandle handle, EventMessage message);

void addEventListener(EventStreamHandle handle, EventListener* listener);
GeneratorHandle eventStreamAsGenerator(EventStreamHandle handle);

EventStreamHandle createEventStreamHandle();

#endif