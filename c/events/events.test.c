
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

#include "events.h"
#include "events.internal.h"

#include "../testing/utils.h"

void TEST_can_create_stream_handle() {
 
    EventStreamHandle handle = createEventStreamHandle();
    
    assert(handle != (EventStreamHandle)NULL);
    
    freeEventStream(&handle);
    
}

void TEST_emit_enqueues_message() {
    EventStreamHandle handle = createEventStreamHandle();
    
    EventQueue* Q = (EventQueue*)handle;
    
    event_type type1 = 1;
    event_type type2 = 2;
    
    EventMessage message1 = { .type = type1 };
    EventMessage message2 = { .type = type2 };
    
    assert(Q->front == NULL);
    assert(Q->back == NULL);
    
    emit(handle, message1);
    
    assert(Q->front != NULL);
    assert(Q->back != NULL);
    assert(Q->front == Q->back);
    
    assert(Q->front->message.type == message1.type);
    
    emit(handle, message2);
    
    assert(Q->front != NULL);
    assert(Q->back != NULL);
    assert(Q->front->next == Q->back);
   
    assert(Q->front->next->message.type == message2.type);
    
    freeEventStream(&handle);
}

void TEST_handles_when_nexting_stopped_stream() {
    EventStreamHandle handle = createEventStreamHandle();
    
    GeneratorHandle genHandle = eventStreamAsGenerator(handle);
    
    EventMessage message1 = { .type = 3, .data = NULL };
    EventMessage messageRecieved;
    emit(handle, message1);
    //sleep(1);
    //kill stream while gen handle is alive
    freeEventStream(&handle);

    gen_next(genHandle, NULL, &messageRecieved);
    freeGenerator(&genHandle);
    
}


void TEST_can_yield_event() {
    EventStreamHandle handle = createEventStreamHandle();
    
    GeneratorHandle genHandle = eventStreamAsGenerator(handle);
    
    EventMessage messageSent = { .type = 3, .data = NULL };
    EventMessage messageRecieved;
      
    sleep(1);  
    emit(handle, messageSent);
    
    gen_next(genHandle, NULL, &messageRecieved);
    
    assert(messageRecieved.type == messageSent.type);

    freeGenerator(&genHandle);
    freeEventStream(&handle);
    
}

void TEST_can_yield_event_multiple() {
    EventStreamHandle handle = createEventStreamHandle();
    
    GeneratorHandle genHandle = eventStreamAsGenerator(handle);
    
    EventMessage messageSent = { .type = 0, .data = NULL };
    EventMessage messageRecieved;
    sleep(1);
    int i;
    for(i=0;i<5;i++) {
        messageSent.type = i;
        emit(handle, messageSent);
        gen_next(genHandle, NULL, &messageRecieved);
        assert(messageRecieved.type == messageSent.type);
        assert(messageRecieved.type == i);
    }
    
    freeGenerator(&genHandle);    
    freeEventStream(&handle);
}

void TEST_can_yield_event_with_sleep() {
    EventStreamHandle handle = createEventStreamHandle();
    
    GeneratorHandle genHandle = eventStreamAsGenerator(handle);
    
    sleep(1);
    EventMessage messageSent = { .type = 4, .data = NULL };
    EventMessage messageRecieved;
    
    emit(handle, messageSent);
    
    gen_next(genHandle, NULL, &messageRecieved);
    
    assert(messageRecieved.type == messageSent.type);
    
    freeGenerator(&genHandle);
    freeEventStream(&handle);
}

int main(int argc, char *argv[]) 
{
    run(TEST_can_create_stream_handle);
    
    run(TEST_emit_enqueues_message);
    
    run(TEST_can_yield_event);
    
    run(TEST_can_yield_event_multiple);
    
    run(TEST_can_yield_event_with_sleep);

    run(TEST_handles_when_nexting_stopped_stream);
    
    printf("\ndone\n");
}