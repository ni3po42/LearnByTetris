#include "events.internal.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * public API
 */
void initEventQueue() {
    for(int i=0;i<eventListSize;i++) {
        circularEventQueue[i] = NO_EVENT;
    }
    frontIndex = 0;
    endIndex = 0;    
}

bool emit(EventMessage message) {

    int nextIndex = (frontIndex + 1) % eventListSize;
    if (nextIndex == endIndex) {
        return false;
    }

    circularEventQueue[frontIndex] = message;
    frontIndex = nextIndex;
    
    return true;  
}

void getNextMessage(EventMessage* message) {
    if (frontIndex == endIndex) {
        *message = NO_EVENT;
        return;
    }

    *message = circularEventQueue[endIndex];
    endIndex = (endIndex + 1) % eventListSize;    
}

#ifdef __cplusplus
}
#endif

