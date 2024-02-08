#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "generator.h"
#include "generator.internal.h"


static GeneratorContext* handleToPtr(GeneratorHandle handle) {
    return (GeneratorContext*)handle;
}

static void* generator_wrap(void* input) {
    
    GeneratorContext* context = (GeneratorContext*)input;
    GeneratorFunction* func = context->generatorFunc;
   
    func((GeneratorHandle)context, context->argument);
    
    pthread_mutex_lock(&(context->lock));
    
    context->isDone = true;
    pthread_cond_signal(&(context->yieldCond));
    
    pthread_mutex_unlock(&(context->lock));
    
}

GeneratorHandle createGenerator(GeneratorFunction* func, size_t bufferInSize, size_t bufferOutSize, void* argument) {
    
    GeneratorContext* context = (GeneratorContext*)malloc(sizeof(GeneratorContext));
    
    context->generatorFunc = func;
    context->isDone = false;
    context->argument = argument;
    context->bufferIn = malloc(bufferInSize);
    context->bufferInSize = bufferInSize;
    context->bufferOut = malloc(bufferOutSize);
    context->bufferOutSize = bufferOutSize;
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    if (pthread_mutex_init(&(context->lock), NULL) != 0) { 
        fprintf(stderr, "\n mutex init has failed\n");
        free(context);
        return (GeneratorHandle)NULL;
    } 
    
    if (pthread_cond_init(&(context->nextCond), NULL) != 0) {                                    
        fprintf(stderr, "\n cond init has failed\n");
        free(context);
        return (GeneratorHandle)NULL;                                                                  
    }
    
     if (pthread_cond_init(&(context->yieldCond), NULL) != 0) {                                    
        fprintf(stderr, "\n cond init has failed\n");
        free(context);
        return (GeneratorHandle)NULL;                                                                  
    } 
 
    pthread_create(&(context->thread_id), &attr, generator_wrap, context);
    
    return (GeneratorHandle)context;
}

bool GeneratorNext(GeneratorHandle handle, void* send, void* received) {
    GeneratorContext* context = handleToPtr(handle);
       
    pthread_mutex_lock(&(context->lock));
    
    if (context->isDone) {
        
        if (received != NULL && context->bufferOut != NULL) {
            memcpy(received, context->bufferOut, context->bufferOutSize);    
        } 
        
    } else {
        
        if (context->bufferIn != NULL && send != NULL) {
            memcpy(context->bufferIn, send, context->bufferInSize);    
        }
        
     //     printf("NEXT sig YIELD (%p)\n", context);
        pthread_cond_signal(&(context->nextCond));
        
   //   printf("NEXT sleeping (%p)\n", context);// (what, no signal?)
        pthread_cond_wait(&(context->yieldCond), &(context->lock));  
   //  printf("NEXT Wakeup (%p)\n", context);
     
        if (received != NULL && context->bufferOut != NULL) {
            memcpy(received, context->bufferOut, context->bufferOutSize);
        }
      
    }
    
    bool isDone = context->isDone;
    pthread_mutex_unlock(&(context->lock));
    
    return !isDone;
}

bool GeneratorYield(GeneratorHandle handle, void* sent, void* recieved, bool isDone) {
    GeneratorContext* context = handleToPtr(handle);
  
    pthread_mutex_lock(&(context->lock));

    if (context->isDone) {
     //   printf("YIELD sig NEXT (%p)\n", context);
        pthread_cond_signal(&(context->yieldCond));
        pthread_mutex_unlock(&(context->lock));
        return false;
    }
  
    if (context->bufferOut != NULL && sent != NULL) {
        memcpy(context->bufferOut, sent, context->bufferOutSize);    
    }
  
    context->isDone = isDone;
    
   // printf("YIELD sig NEXT (%p)\n", context);
    pthread_cond_signal(&(context->yieldCond));
    
   // printf("YIELD sleeping (%p)\n", context);
    pthread_cond_wait(&(context->nextCond), &(context->lock));    
   //printf("YIELD wakeup (%p)\n", context);
   
    if (recieved != NULL && context->bufferIn != NULL) {
        memcpy(recieved, context->bufferIn, context->bufferInSize);    
    }

    isDone = context->isDone;
    pthread_mutex_unlock(&(context->lock));

    return !isDone;
}

void freeGenerator(GeneratorHandle* handle) {
    if (handle == NULL) {
        return;
    }
    
    GeneratorContext* context = handleToPtr(*handle);
    
    pthread_mutex_unlock(&context->lock);
    context->isDone = true;
    pthread_cond_signal(&context->nextCond);
    pthread_mutex_unlock(&context->lock);
    
    pthread_join(context->thread_id, NULL);
    
    if (pthread_cond_destroy(&context->nextCond) != 0) {                                       
        perror("pthread_cond_destroy() error: nextCond");                                     
        exit(2);                                                                    
    } 
    
    if (pthread_cond_destroy(&context->yieldCond) != 0) {                                       
        perror("pthread_cond_destroy() error: nextCond");                                     
        exit(2);                                                                    
    }
    
    free(context->bufferIn);
    free(context->bufferOut);
    free(context);
    *handle = (GeneratorHandle)NULL;
}
