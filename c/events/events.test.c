#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include "events.h"
#include "events.internal.h"

#include "../testing/utils.h"

void TEST_can_create_stream_handle() {
 
    EventStreamHandle* handle = createEventStreamHandle();
    
    assert(handle != (EventStreamHandle*)NULL);
    
    freeEventStream(&handle);
    
}

void TEST_emit_enqueues_message() {
    EventStreamHandle* handle = createEventStreamHandle();
    
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
    
    assert(Q->front->type == message1.type);
    
    emit(handle, message2);
    
    assert(Q->front != NULL);
    assert(Q->back != NULL);
    assert(Q->front->next == Q->back);
   
    assert(Q->front->next->type == message2.type);
    
    freeEventStream(&handle);
}

void TEST_handles_when_nexting_stopped_stream() {
    EventStreamHandle* handle = createEventStreamHandle();
    
    GeneratorHandle* genHandle = eventStreamAsGenerator(handle);
    
    EventMessage message1 = { .type = 3 };
    EventMessage messageRecieved;
    emit(handle, message1);

    //kill stream while gen handle is alive
    freeEventStream(&handle);

    gen_next(genHandle, &messageRecieved);
    freeGenerator(&genHandle);
    
}


void* can_yield_event_thread_func(void* input) {

    EventMessage messageRecieved;
    GeneratorHandle* handle = (GeneratorHandle*)input;

    gen_next(handle, &messageRecieved);
    assert(messageRecieved.type == 3);
    return NULL;
}
// single call to gen_next and gen_yield works single threaded
// only on first call, this is not an intended
void TEST_can_yield_event() {
    EventStreamHandle* handle = createEventStreamHandle();
    
    GeneratorHandle* genHandle = eventStreamAsGenerator(handle);
    
    EventMessage messageSent = { .type = 3 };
    
    pthread_t thread;
  
    pthread_create(&thread, NULL, can_yield_event_thread_func,(void*)genHandle);

    emit(handle, messageSent);

    pthread_join(thread, NULL);
 
    freeGenerator(&genHandle);
    
    freeEventStream(&handle);   
}

void* can_yield_multi_event_thread_func(void* input) {

    EventMessage messageRecieved;
    GeneratorHandle* handle = (GeneratorHandle*)input;
    int i;
    for (i=0;i<5;i++) {
        gen_next(handle, &messageRecieved);
        assert(messageRecieved.type == i);
    }
    return NULL;
}

void TEST_can_yield_event_multiple() {
    EventStreamHandle* handle = createEventStreamHandle();
    
    GeneratorHandle* genHandle = eventStreamAsGenerator(handle);
    
    pthread_t thread;

    pthread_create(&thread, NULL, can_yield_multi_event_thread_func,(void*)genHandle);
    int i;
    for(i=0;i<5;i++) {
        EventMessage messageSent = { .type = i };
        emit(handle, messageSent);
        sleep(1);
        fprintf(stderr, "type: %d\n", i);
    }
    
    pthread_join(thread, NULL);

    freeGenerator(&genHandle);
    freeEventStream(&handle);  
}


int main(int argc, char *argv[]) 
{
    run(TEST_can_create_stream_handle);
    
    run(TEST_emit_enqueues_message);
    
    run(TEST_can_yield_event);
    
    run(TEST_can_yield_event_multiple);

    run(TEST_handles_when_nexting_stopped_stream);
    
    printf("\ndone\n");
}