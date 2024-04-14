#include <stdbool.h>
#include <pthread.h>

#include "generator.h"

struct FrameNode {
    struct FrameNode* next;
    void* data;
    size_t size;
};
typedef struct FrameNode FrameNode;

// concrete implementation of a Generator function's context
typedef struct GeneratorContext {
 
    // address to return to on the next call to next
    void* returnAddress;
        
    // function pointer to generator implementation
    GeneratorFunction* generatorFunc;
    
    // pointer to input data
    void* argument;

    // pointer to write yielded data to
    void* yieldPtr;
    // size of buggerOut data
    size_t yieldDataSize;
    
    // represents data from stack from that must be saved between calls to the generator function
    FrameNode* stackFrame;

    FrameNode* framePtr;

    bool restoreMode;
    
    // flag for if generator function is done
    bool isDone;
    
} GeneratorContext;
