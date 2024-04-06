#include <stdbool.h>
#include <pthread.h>

#include "generator.h"

// concrete implementation of a Generator function's context
typedef struct GeneratorContext {

    // thread id of executing function
    pthread_t thread_id;
    
    // mutex for controlling access to context
    pthread_mutex_t lock;
    
    // condition variable to signal when a next call can stop blocking
    pthread_cond_t nextCond;

    // condition variable to signal when a yield call can stop blocking
    pthread_cond_t yieldCond;
    
    // function pointer to generator implementation
    GeneratorFunction* generatorFunc;
    
    // temp heap storage for data into gen func
    void* bufferIn;
    // size of bufferIn data
    size_t bufferInSize;

    // temp heap storage for data yielded from func
    void* bufferOut;
    // size of buggerOut data
    size_t bufferOutSize;
    
    // initial argument for gen func
    void* argument;
    
    // flag for if generator is currently yielding
    bool isYielding;

    // flag for if generator function is done
    bool isDone;
    
} GeneratorContext;
