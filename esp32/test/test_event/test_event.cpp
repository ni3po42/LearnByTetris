#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <unity.h>
#include "events.internal.h"


void setUp() {
    initEventQueue();
}

void test_queue_can_wrap_around() {
    EventMessage message = 0;
    
    for(int i=1;i<eventListSize;i++) {
        bool emitResult = emit(i);     
        TEST_ASSERT_EQUAL(true, emitResult);
    }

    bool result = emit(eventListSize);
    TEST_ASSERT_EQUAL(false, result);    

    for(int i=1;i<eventListSize;i++) {                
        getNextMessage(&message);
        TEST_ASSERT_EQUAL(i, message);
    }

    getNextMessage(&message);

    TEST_ASSERT_EQUAL(NO_EVENT, message);

}

void test_can_queue_multiple_messages() {
   
    emit(42);
    emit(3141);
    emit(112358);

    EventMessage messageRecieved;

    getNextMessage(&messageRecieved);
    TEST_ASSERT_EQUAL(42, messageRecieved);
    getNextMessage(&messageRecieved);
    TEST_ASSERT_EQUAL(3141, messageRecieved);
    getNextMessage(&messageRecieved);
    TEST_ASSERT_EQUAL(112358, messageRecieved);
    getNextMessage(&messageRecieved);
    TEST_ASSERT_EQUAL(NO_EVENT, messageRecieved);
}

void test_empty_queue_gives_no_event_message() {
    EventMessage message = 42;

    getNextMessage(&message);

    TEST_ASSERT_EQUAL(NO_EVENT, message);
}

void setup()
{
    UNITY_BEGIN();    
    RUN_TEST(test_queue_can_wrap_around);
    
    RUN_TEST(test_can_queue_multiple_messages);
    
    RUN_TEST(test_empty_queue_gives_no_event_message);
    UNITY_END();
}

void loop() {

}