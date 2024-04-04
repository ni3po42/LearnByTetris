#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <stdbool.h>

#include "events.h"
#include "events.internal.h"

/**
 * Private utilities
 */
 
// locks changes to queue
static void Q_lock(EventQueue* Q) {
    pthread_mutex_lock(&(Q->lock)); 
}

// unlocks changes to queue
static void Q_unlock(EventQueue* Q) {
    pthread_mutex_unlock(&(Q->lock)); 
}

// resolves opaque pointer to concrete pointer
static EventQueue* eventStreamHandleToPtr(EventStreamHandle handle) {
    return (EventQueue*)handle;
}

// add new file descriptor to list. no mutex locking
static void trackEventFileDescriptor(EventQueue* Q, event_fd_t efd) {
    EventFileDescriptorNode* newFDNode = (EventFileDescriptorNode*)malloc(sizeof(EventFileDescriptorNode));
    
    newFDNode->efd = efd;
    newFDNode->next = NULL;
    
    if (Q->efds == NULL) {
        Q->efds = newFDNode;
    } else {
        newFDNode->next = Q->efds;
        Q->efds = newFDNode;
    }
    
    Q->efdsCount += 1;
}

// adds new thread to track for queue. no mutex locking
static void trackThread(EventQueue* Q,  pthread_t thread_id) {
    ThreadIdNode* newThreadNode = (ThreadIdNode*)malloc(sizeof(ThreadIdNode));
    
    newThreadNode->thread_id = thread_id;
    newThreadNode->next = NULL;
    
    if (Q->threadIds == NULL) {
        Q->threadIds = newThreadNode;
    } else {
        newThreadNode->next = Q->threadIds;
        Q->threadIds = newThreadNode;
    }
}

// attempts to move a ref to a event node to the next in queue. no mutex locking
static void moveToNext(EventQueue* Q, EventNode** finger) {
    
    if (*finger == NULL) {
        return;
    }
    
    // if any are waiting...
    if ((*finger)->waitReadCount > 0) {
        // decrement count and point ref to next in list
        (*finger)->waitReadCount -= 1;
        (*finger) = (*finger)->next;
    }
    
    //trim tail if no others are waiting for tail anymore
    EventNode* temp = NULL;
    while(Q->tail != NULL && Q->tail->waitReadCount == 0) {
        temp = Q->tail;
        Q->tail = Q->tail->next;
        if (temp == Q->head) {
            Q->head = NULL;
        }
        free(temp);
    }
    
}

// adds message to queue. no mutex locking
static EventNode* enqueue(EventQueue* Q, EventMessage message) {
    
    EventNode* newNode = (EventNode*) malloc(sizeof(EventNode));
    newNode->next = NULL;
    
    newNode->message.type = message.type;
    newNode->waitReadCount = Q->efdsCount;
    newNode->message.data = message.data;
    
    if (Q->head == NULL) {
        Q->tail = newNode;
    } else {
        Q->head->next = newNode;
    }
    
    Q->head = newNode;
    
    return newNode;
}

// signals all open event file descriptors with given message data. no mutex locking
static void signalEfds(EventQueue* Q, uint64_t sigMessage) {
    
    EventFileDescriptorNode* efdNode = Q->efds;
    
    int size;
    
    while(efdNode != NULL) {
        size = write(efdNode->efd, &sigMessage, sizeof(uint64_t));
        
        if (size != sizeof(uint64_t)) {
            fprintf(stderr, "\nfailed to siginal a listener\n");
            return;
        }
        
        efdNode = efdNode->next;
    }
}

// handles calling event listener when an event is signaled from event fd
static void* wrap_listener(void* input) {
    listener_input_t* actualInput = (listener_input_t*) input;
    
    EventListener* listener = actualInput->listener;
    event_fd_t efd = actualInput->efd;
    EventQueue* Q = actualInput->queue;
    free(input);
    
    EventNode* finger = NULL;
    
    int size;
    uint64_t messageCount;
    EventMessage eventMessage;
    
    while(true) {
        
        size = read(efd, &messageCount, sizeof(uint64_t));
    
        // normally the number of events are written to the efd, so no passing of information.
        // I get around this by passing 2 times the real count. IF the count is odd, that means
        // a kill signal is issued and threads should all exit cleanly
        if (size != sizeof(uint64_t) || messageCount % 2 == 1) {
            return NULL;
        }
        
        // since message count is 2 times the real, shift 1 to get actual count
        messageCount >>= 1;
        
        Q_lock(Q);
        
        if (finger == NULL) {
            finger = Q->tail;
        }
        
        for(;messageCount > 0;messageCount--) {
            if (finger != NULL) {
                listener(&(finger->message));
                moveToNext(Q, &finger);
            }
        }
        
        Q_unlock(Q);
    }
}


static void wrap_generator(GeneratorHandle handle, void* argument) {
    listener_input_t* actualInput = (listener_input_t*) argument;
    
    event_fd_t efd = actualInput->efd;
    EventQueue* Q = actualInput->queue;
    EventNode* finger = Q->tail;
    
    int size;
    uint64_t messageCount;
    EventMessage eventMessage;
    
    while(true) {
        
        size = read(efd, &messageCount, sizeof(uint64_t));
    
        if (size != sizeof(uint64_t) || messageCount % 2 == 1) {
            free(argument);
            return;
        }
        
        messageCount >>= 1;
        
        if (finger == NULL) {
            finger = Q->tail;
        }
        
        for(;messageCount > 0;messageCount--) {
            if (finger == NULL) {
                break;
            }
                
            if (!gen_yield(handle, &finger->message, NULL)) {
                free(argument);
                return;
            }
            
            moveToNext(Q, &finger);
        }
    }
}


/**
 * public API
 */

void startEventStream(EventStreamHandle handle) {
    EventQueue* Q = eventStreamHandleToPtr(handle);
    
    Q_lock(Q); 
    Q->hasStarted = true;
    Q_unlock(Q);
}

EventStreamHandle createEventStreamHandle() {
    EventQueue* Q = (EventQueue*)malloc(sizeof(EventQueue));
    
    Q->head = NULL;
    Q->tail = NULL;
    Q->efds = NULL;
    Q->threadIds = NULL;
    Q->efdsCount = 0;
    Q->hasStarted = false;
    
    if (pthread_mutex_init(&(Q->lock), NULL) != 0) { 
        fprintf(stderr, "\n mutex init has failed\n");
        free(Q);
        return (EventStreamHandle)NULL;
    } 
    
    return (EventStreamHandle)Q;
}

void stopEventStream(EventStreamHandle handle) {
    EventQueue* Q = eventStreamHandleToPtr(handle);
    
    Q_lock(Q); 
    Q->hasStarted = false;
    Q_unlock(Q); 
}

void freeEventStream(EventStreamHandle* handle) {
    
    EventQueue* Q = eventStreamHandleToPtr(*handle);
    
    Q_lock(Q);
    
    signalEfds(Q, EVENT_TERM);
    
    EventNode* currentEventNode = Q->tail;
    while(currentEventNode != NULL) {
        EventNode* temp = currentEventNode;
        currentEventNode = temp->next;
        free(temp);
    }
    Q->tail = NULL;
    Q->head = NULL;
    
    EventFileDescriptorNode* efdNode = Q->efds;
    while(efdNode != NULL) {
        EventFileDescriptorNode* temp = efdNode;
        efdNode = temp->next;
        close(temp->efd);
        free(temp);
    }
    Q->efds = NULL;
    
    ThreadIdNode* threadNode = Q->threadIds;
    while(threadNode != NULL) {
        ThreadIdNode* temp = threadNode;
        threadNode = temp->next;
        pthread_join(temp->thread_id, NULL);  
        free(temp);
    }
    
    Q->threadIds = NULL;
    Q->efdsCount = 0;
    
    Q_unlock(Q);

    pthread_mutex_destroy(&(Q->lock)); 
    
    free(Q);
    
    *handle = (EventStreamHandle)NULL;
}

void emit(EventStreamHandle handle, EventMessage message) {
    EventQueue* Q = eventStreamHandleToPtr(handle);
    
    if (Q == NULL) {
        fprintf(stderr, "BAD HANDLE");
        return;
    }
    
    Q_lock(Q);
    
    if (!(Q->hasStarted)) {
        Q_unlock(Q);
        return;
    }
    
    enqueue(Q, message);
    
    signalEfds(Q, EVENT_UPDATE);
    
    Q_unlock(Q);
}

void addEventListener(EventStreamHandle handle, EventListener* listener) {
    EventQueue* Q = eventStreamHandleToPtr(handle);
    
    Q_lock(Q);
    
    if (Q->hasStarted) {
        Q_unlock(Q);
        return;
    }
    
    pthread_t thread_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    listener_input_t* input = (listener_input_t*)malloc(sizeof(listener_input_t));
   
    event_fd_t efd = eventfd(0,0);
    
    input->listener = listener;
    input->queue = Q;
    input->efd = efd;
    
    trackEventFileDescriptor(Q, efd);
    pthread_create(&thread_id, &attr, wrap_listener, input);
    trackThread(Q, thread_id);
    
    Q_unlock(Q);
}

GeneratorHandle eventStreamAsGenerator(EventStreamHandle handle) {
    
    EventQueue* Q = eventStreamHandleToPtr(handle);
    
    Q_lock(Q);
    
    if (Q->hasStarted) {
        Q_unlock(Q);
        return (GeneratorHandle)NULL;
    }
    
    listener_input_t* input = (listener_input_t*)malloc(sizeof(listener_input_t));
   
    event_fd_t efd = eventfd(0,0);
    
    input->listener = NULL;
    input->queue = Q;
    input->efd = efd;
    
    trackEventFileDescriptor(Q, efd);
    
    GeneratorHandle genHandle = gen_func(wrap_generator, NULL, EventMessage, input);
    
    Q_unlock(Q);
    
    return genHandle;
}