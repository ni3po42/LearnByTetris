
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

void TEST_can_start_stream_handle() {
    
    EventStreamHandle handle = createEventStreamHandle();
    
    EventQueue* Q = (EventQueue*)handle;
    
    assert(Q->hasStarted == false);
    
    startEventStream(handle);
    
    assert(Q->hasStarted == true);
    
    stopEventStream(handle);
    
    assert(Q->hasStarted == false);
    
    freeEventStream(&handle);
}

void event_listener_aaa(const EventMessage* message) {
    //nothing gets run here
}
void TEST_can_add_listener_aaa() {
    EventStreamHandle handle = createEventStreamHandle();
    
    EventQueue* Q = (EventQueue*)handle;
    
    assert(Q->efds == NULL);
    assert(Q->efdsCount == 0);
    
    addEventListener(handle, event_listener_aaa);   
    
    assert(Q->efds != NULL);
    assert(Q->efds->efd != 0);
    assert(Q->efdsCount == 1);
    
    startEventStream(handle);
    stopEventStream(handle);
    
    freeEventStream(&handle);
}


void TEST_emit_enqueues_message() {
    EventStreamHandle handle = createEventStreamHandle();
    
    EventQueue* Q = (EventQueue*)handle;
    
    startEventStream(handle);
    
    event_type type1 = 1;
    event_type type2 = 2;
    
    EventMessage message1 = { .type = type1 };
    EventMessage message2 = { .type = type2 };
    
    assert(Q->tail == NULL);
    assert(Q->head == NULL);
    
    emit(handle, message1);
    
    assert(Q->tail != NULL);
    assert(Q->head != NULL);
    assert(Q->head == Q->tail);
    
    assert(Q->tail->message.type == message1.type);
    
    emit(handle, message2);
    
    assert(Q->tail != NULL);
    assert(Q->head != NULL);
    assert(Q->tail->next == Q->head);
   
    assert(Q->tail->next->message.type == message2.type);
    
    stopEventStream(handle);
    
    freeEventStream(&handle);
}

#define bbb 1
void event_listener_bbb(const EventMessage* message) {
    assert(message->type == bbb);
    *((int*)message->data) = 1;
}
void TEST_emit_triggers_listener() {
    EventStreamHandle handle = createEventStreamHandle();
    
    EventQueue* Q = (EventQueue*)handle;
    
    addEventListener(handle, event_listener_bbb);
    
    startEventStream(handle);
    volatile int data = 0;
    EventMessage message = { .type = bbb, .data = (void*)&data };
    
    emit(handle, message);
    
    sleep(1);
    
    stopEventStream(handle);
    freeEventStream(&handle);
    
    assert(data == 1);
}

typedef struct ccc {
    int a;
    int b;
    int c;
} ccc;


void event_listener_ccc_1(const EventMessage* message) {
    assert(message->type == 2);
    ccc* temp = (ccc*) message->data;
    temp->a += 10;
}
void event_listener_ccc_2(const EventMessage* message) {
    assert(message->type == 2);
    ccc* temp = (ccc*) message->data;
    temp->b += 20;
}
void event_listener_ccc_3(const EventMessage* message) {
    assert(message->type == 2);
    ccc* temp = (ccc*) message->data;
    temp->c += 30;
}
void TEST_emit_x2_triggers_listener_x3() {
    EventStreamHandle handle = createEventStreamHandle();
    
    EventQueue* Q = (EventQueue*)handle;
    
    addEventListener(handle, event_listener_ccc_1);
    addEventListener(handle, event_listener_ccc_2);
    addEventListener(handle, event_listener_ccc_3);
    
    startEventStream(handle);
    volatile struct ccc data1 = { .a = 1, .b = 2, .c = 3 }; 
    EventMessage message1 = { .type = 2, .data = (void*)&data1 };
    
    emit(handle, message1);
    
    sleep(1);
    volatile struct ccc data2 = { .a = 4, .b = 5, .c = 6 }; 
    EventMessage message2 = { .type = 2, .data = (void*)&data2 };
    
    emit(handle, message2);
    
    sleep(1);
    stopEventStream(handle);
    freeEventStream(&handle);
    assert(data2.a == 14);
    assert(data2.b == 25);
    assert(data2.c == 36);
}


void event_listener_ddd(const EventMessage* message) {
    assert(("Listener should not have fired", false));
}
void TEST_listener_does_not_fire_when_stream_is_stopped() {
    EventStreamHandle handle = createEventStreamHandle();
    
    addEventListener(handle, event_listener_ddd);
    
    EventMessage message1 = { .type = 3, .data = NULL };
    
    emit(handle, message1);
    
    startEventStream(handle);
    //do nothing
    stopEventStream(handle);
    
    emit(handle, message1);
    
    sleep(1);
    freeEventStream(&handle);
}


void TEST_can_yield_event() {
    EventStreamHandle handle = createEventStreamHandle();
    
    GeneratorHandle genHandle = eventStreamAsGenerator(handle);
    
    startEventStream(handle);
    
    EventMessage messageSent = { .type = 3, .data = NULL };
    EventMessage messageRecieved;
    
    emit(handle, messageSent);
    
    gen_next(genHandle, NULL, &messageRecieved);
    
    assert(messageRecieved.type == messageSent.type);
    
    stopEventStream(handle);
    
    freeGenerator(&genHandle);
    
    freeEventStream(&handle);
}

void TEST_can_yield_event_multiple() {
    EventStreamHandle handle = createEventStreamHandle();
    
    GeneratorHandle genHandle = eventStreamAsGenerator(handle);
    
    startEventStream(handle);
    
    EventMessage messageSent = { .type = 0, .data = NULL };
    EventMessage messageRecieved;
    
    int i;
    for(i=0;i<5;i++) {
        messageSent.type = i;
        emit(handle, messageSent);
        gen_next(genHandle, NULL, &messageRecieved);
        assert(messageRecieved.type == messageSent.type);
        assert(messageRecieved.type == i);
    }
    
    stopEventStream(handle);
    
    freeGenerator(&genHandle);
    
    freeEventStream(&handle);
}

void TEST_can_yield_event_with_sleep() {
    EventStreamHandle handle = createEventStreamHandle();
    
    GeneratorHandle genHandle = eventStreamAsGenerator(handle);
    
    startEventStream(handle);
    sleep(1);
    EventMessage messageSent = { .type = 4, .data = NULL };
    EventMessage messageRecieved;
    
    emit(handle, messageSent);
    
    gen_next(genHandle, NULL, &messageRecieved);
    
    assert(messageRecieved.type == messageSent.type);
    
    stopEventStream(handle);
    
    freeGenerator(&genHandle);
    freeEventStream(&handle);
}

int main(int argc, char *argv[]) 
{
    run(TEST_can_create_stream_handle);
    
    run(TEST_can_start_stream_handle);
   
    run(TEST_can_add_listener_aaa);
    
    run(TEST_emit_enqueues_message);

    run(TEST_emit_triggers_listener);
    
    run(TEST_emit_x2_triggers_listener_x3);
    
    run(TEST_listener_does_not_fire_when_stream_is_stopped);
    
    run(TEST_can_yield_event);
    
    run(TEST_can_yield_event_multiple);
    
    run(TEST_can_yield_event_with_sleep);
    
    printf("\ndone\n");
}