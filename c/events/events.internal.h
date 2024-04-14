#include <stdbool.h>

// linked list defining the event queue of messages
struct EventNode {

    // tracks howmany references outside the event queue exist
    int refCount;

    // payload
    event_type type;

    // next node in the list
    struct EventNode* next;
};
typedef struct EventNode EventNode;

// concreate event queue to the EventStreamHandle
typedef struct EventQueue {
    // front of queue
    EventNode* front;

    // back of queue
    EventNode* back;

    // mutex for locking changes to queue
    pthread_mutex_t lock;

    // condition variable to signalling updates to queue
    pthread_cond_t update;

    // tracks how many generators are referencing the queue
    int refCount;

    // flag is queue is accepting events or not
    bool isClosed;
} EventQueue;
