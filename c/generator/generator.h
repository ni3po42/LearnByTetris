#ifndef __generator_h_
#define __generator_h_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct GeneratorContext* GeneratorHandle;

typedef void GeneratorFunction(GeneratorHandle handle, void* argument);

GeneratorHandle createGenerator(GeneratorFunction* func, size_t bufferInSize, size_t bufferOutSize, void* argument);
bool GeneratorNext(GeneratorHandle handle, void* sent, void* received);
bool GeneratorYield(GeneratorHandle handle, void* sent, void* received, bool isDone);
void freeGenerator(GeneratorHandle* handle);

#define gen_func(func, TSEND, TRECEIVE, input) createGenerator(func, sizeof(TSEND), sizeof(TRECEIVE), (void*)input)
#define gen_next(handle, sent, received) GeneratorNext(handle, (void*)sent, (void*)received)
#define gen_yield(handle, sent, received) GeneratorYield(handle, (void*)sent, (void*)received, false)
#define gen_return(handle, sent, received) GeneratorYield(handle, (void*)sent, (void*)received, true)

#endif