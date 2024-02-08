
#include <stdbool.h>

#define EVENT_TERM ((uint64_t)1)
#define EVENT_UPDATE ((uint64_t)2)


struct EventNode {
    EventMessage message;
    int waitReadCount;
    struct EventNode* next;
};
typedef struct EventNode EventNode;


struct EventFileDescriptorNode {
    event_fd_t efd;
    struct EventFileDescriptorNode* next;
};
typedef struct EventFileDescriptorNode EventFileDescriptorNode;

struct ThreadIdNode {
    pthread_t thread_id;
    struct ThreadIdNode* next;
};
typedef struct ThreadIdNode ThreadIdNode;


typedef struct EventQueue {
    EventNode* head;
    EventNode* tail;
    pthread_mutex_t lock;
    bool hasStarted;
    EventFileDescriptorNode* efds;
    ThreadIdNode* threadIds;
    int efdsCount;
} EventQueue;

typedef struct listener_input_t {
    event_fd_t efd;
    EventListener* listener;
    EventQueue* queue;
} listener_input_t;
