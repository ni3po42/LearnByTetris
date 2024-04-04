
#include <stdbool.h>

/**
 * The event queue uses event file descriptors to signal when events occur.
 * These structures track resources related to which threads have handled what messages
*/

#define EVENT_TERM ((uint64_t)1)
#define EVENT_UPDATE ((uint64_t)2)

// type for event file descriptor
typedef int event_fd_t;

// linked list defining the event queue of messages
struct EventNode {

    // payload
    EventMessage message;

    // tracks howmany threads are waiting to read the message
    int waitReadCount;

    // next node in the list
    struct EventNode* next;
};
typedef struct EventNode EventNode;

// linked list defining open event file descriptors
struct EventFileDescriptorNode {
    event_fd_t efd;
    struct EventFileDescriptorNode* next;
};
typedef struct EventFileDescriptorNode EventFileDescriptorNode;

// linked list defining running threads
struct ThreadIdNode {
    pthread_t thread_id;
    struct ThreadIdNode* next;
};
typedef struct ThreadIdNode ThreadIdNode;

// concreate event queue to the EventStreamHandle
typedef struct EventQueue {
    // front of queue
    EventNode* head;

    // back of queue
    EventNode* tail;

    // mutex for locking changes to queue
    pthread_mutex_t lock;

    // flag for if event queue is runnning
    bool hasStarted;

    // list of open event file descriptors
    EventFileDescriptorNode* efds;

    // list of running threads
    ThreadIdNode* threadIds;
    int efdsCount;
} EventQueue;

/**
 * structure for wrapping queue information into an isolated scope (this case, a pthread)
*/
typedef struct listener_input_t {
    // the open event file descriptor for this listener
    event_fd_t efd;

    // handler for when an event signal occurs
    EventListener* listener;

    // pointer to queue context
    EventQueue* queue;
} listener_input_t;
