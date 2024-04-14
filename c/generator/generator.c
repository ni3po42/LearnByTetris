#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "generator.h"
#include "generator.internal.h"

/**
 * Generator function isolated scope is implemented with a pthread.
 * Condition variables and mutex handle blocking and signaling
*/

GeneratorHandle* CreateGenerator(GeneratorFunction* func, size_t yieldDataSize, void* argument) {
    
    GeneratorHandle* context = (GeneratorHandle*)malloc(sizeof(GeneratorContext));
    
    context->generatorFunc = func;
    context->isDone = false; 
    context->argument = argument;    
    context->yieldDataSize = yieldDataSize;

    context->returnAddress = NULL;
    context->stackFrame = NULL;
    context->framePtr = NULL;
    context->restoreMode = false;

    return context;
}

static void DumpFrame(GeneratorHandle* context, const char* str) {

    fprintf(stderr, "%s (root)", str);
    FrameNode* temp2 = context->stackFrame;
    while(temp2 != NULL) {
        fprintf(stderr, " -> %p [%p:%ld] (%p)", temp2, temp2->data, temp2->size, temp2->next);
        temp2 = temp2->next;
    }
    fprintf(stderr, "\n");
}

void* GeneratorNextFramePtr(GeneratorHandle* context, size_t size) {

    if (context->restoreMode) {
        
        if (context->framePtr == NULL && context->stackFrame == NULL) {   
            return NULL;            
        } else {

            if (context->framePtr == NULL) {
                context->framePtr = context->stackFrame;
            }

            void* temp = context->framePtr->data;
            context->framePtr = context->framePtr->next;
            return temp;
        }
    } else {
        FrameNode* newPtr = (FrameNode*)malloc(sizeof(FrameNode));

        if (context->framePtr == NULL && context->stackFrame == NULL) {
            context->stackFrame = newPtr;
        } else if(context->framePtr != NULL) {
            context->framePtr->next = newPtr;  
        } else {            
            context->stackFrame->next = newPtr;
        }

        context->framePtr = newPtr;

        newPtr->data = malloc(size);
        newPtr->size = size;
        newPtr->next = NULL;

        return newPtr->data;
    }
    
}

void* GeneratorTryGoto(GeneratorHandle* handle, void* elseAddress, void** argument) {
    
    *argument = handle->argument;
    handle->restoreMode = true;
    handle->framePtr = handle->stackFrame;
   
    if (handle->returnAddress != NULL) {
        return handle->returnAddress;
    } else {
        return elseAddress;
    }
}

bool GeneratorNext(GeneratorHandle* context, void* received) {

    if (context->isDone) {
        return false;
    }
    
    context->yieldPtr = received;
    context->generatorFunc(context);
    context->yieldPtr = NULL;
        
    return !context->isDone;
}

bool GeneratorYield_First(GeneratorHandle* context, void* sent, void* returnAddress) {

    context->isDone = false;

    context->returnAddress = returnAddress;

    // place yielded value into buffer
    if (context->yieldPtr != NULL && sent != NULL) {
        memcpy(context->yieldPtr, sent, context->yieldDataSize);    
    }

    return returnAddress != NULL;
}

void GeneratorYield_Second(GeneratorHandle* context) {   
    context->returnAddress = NULL;
    context->isDone = true;
}

void freeGenerator(GeneratorHandle** handlePtr) {

    GeneratorContext* context = *handlePtr;

    if (context == NULL) {
        return;
    }
    
    context->isDone = true;
   
    if (context->stackFrame != NULL) {
        free(context->stackFrame);
        context->stackFrame = NULL;
    }

    while (context->stackFrame != NULL) {
        FrameNode* temp = context->stackFrame->next;        
        free(context->stackFrame->data);
        context->stackFrame->data = NULL;
        free(context->stackFrame);
        context->stackFrame = temp;
    }

    context->framePtr = NULL;

    free(context);
    *handlePtr = (GeneratorHandle*)NULL;
}
