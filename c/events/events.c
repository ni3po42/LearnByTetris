#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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

// adds message to queue. no mutex locking
static EventNode* enqueue(EventQueue* Q, EventMessage message) {
    
    EventNode* newNode = (EventNode*) malloc(sizeof(EventNode));
    newNode->next = NULL;
    
    newNode->message.type = message.type;
    newNode->refCount = Q->refCount;
    
    if (Q->front == NULL) {
        Q->front = newNode;
    } else {
        Q->back->next = newNode;
    }    
    Q->back = newNode;
    
    return newNode;
}


// coroutine implementation for listening and then yielding event messages
static void wrap_generator(GeneratorHandle handle, void* argument) {
    EventQueue* Q = (EventQueue*) argument;   

    EventNode* finger = NULL;
    
    while(!Q->isClosed) {
  
        if (finger == NULL) {
            // wait for an update from the queue
            Q_lock(Q); 
            pthread_cond_wait(&Q->update, &Q->lock);
            finger = Q->front;
            Q_unlock(Q);

        } else {
            EventMessage message = finger->message;          
            
            if (!gen_yield(handle, &message, NULL)) {
                // generator is aborting
                break;
            }
            
            if (Q->isClosed) {
                break;
            }

            Q_lock(Q);
            
            // generator is done with node, decrement count
            finger->refCount--;

            EventNode* temp = finger->next;

            // if no generators are accessing node, perform clean up
            if (finger->refCount == 0) {
                
                if (finger == Q->front && finger != Q->back) {
                    Q->front = temp;
                } else if (finger == Q->front && finger == Q->back) {
                    Q->front = temp;
                    Q->back = temp;
                }

                free(finger);
            }

            finger = temp;
            Q_unlock(Q);
            
        }
    }

    // generator is terminating, decrement count in queue
    Q_lock(Q);
    Q->refCount--;
    Q_unlock(Q);
}

/**
 * public API
 */

EventStreamHandle createEventStreamHandle() {
    EventQueue* Q = (EventQueue*)malloc(sizeof(EventQueue));
    
    Q->front = NULL;
    Q->back = NULL;
    Q->isClosed = false;
    Q->refCount = 0;
    
    if (pthread_mutex_init(&Q->lock, NULL) != 0) { 
        fprintf(stderr, "\n mutex init has failed\n");
        free(Q);
        return (EventStreamHandle)NULL;
    }

    if (pthread_cond_init(&Q->update, NULL) != 0) {
        fprintf(stderr, "\n cond var update has failed\n");
        free(Q);
        return (EventStreamHandle)NULL;
    } 
    
    return (EventStreamHandle)Q;
}

void freeEventStream(EventStreamHandle* handle) {    
    EventQueue* Q = eventStreamHandleToPtr(*handle);
    
    Q_lock(Q);
    
    if (!Q->isClosed) {
        Q->isClosed = true;
    }

    if(Q->refCount > 0) {
        fprintf(stderr, "WARNING: not all generators freed before event queue free. Generator functionality is undefined.\n");
    }
    
    // free all remaining nodes
    while(Q->front != NULL) {
        EventNode* temp = Q->front;
        Q->front = Q->front->next;
        free(temp);
    }
    Q->back = NULL;
       
    Q_unlock(Q);

    pthread_cond_destroy(&Q->update);
    pthread_mutex_destroy(&Q->lock); 
    
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

    if (Q->isClosed) {
        Q_unlock(Q);
        return;
    }

    enqueue(Q, message); 

    Q_unlock(Q);

    pthread_cond_broadcast(&Q->update);
}

GeneratorHandle eventStreamAsGenerator(EventStreamHandle handle) {
    
    EventQueue* Q = eventStreamHandleToPtr(handle);
    
    Q_lock(Q);
  
    // increment ref counter for generator
    Q->refCount++;
    
    // with new generator, we'll need to increment all node refCounts to
    // account for additional generator
    EventNode* temp = Q->front;
    while(temp != NULL) {
        temp->refCount++;
        temp = temp->next;
    }    

    GeneratorHandle genHandle = gen_func(wrap_generator, NULL, EventMessage, Q);
    
    Q_unlock(Q);

    return genHandle;
}