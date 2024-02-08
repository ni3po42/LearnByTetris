#include <stdbool.h>
#include <pthread.h>

#include "generator.h"

typedef struct GeneratorContext {
    pthread_t thread_id;
    
    pthread_mutex_t lock;
    
    pthread_cond_t nextCond;
    pthread_cond_t yieldCond;
    
    GeneratorFunction* generatorFunc;
    
    void* bufferIn;
    size_t bufferInSize;
    void* bufferOut;
    size_t bufferOutSize;
    
    void* argument;
    
    bool isYielding;
    bool isDone;
    
} GeneratorContext;
