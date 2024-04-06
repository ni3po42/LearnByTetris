#ifndef __generator_h_
#define __generator_h_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// opaque pointer for generator handle
typedef struct GeneratorContext* GeneratorHandle;

// function type a generator function
typedef void GeneratorFunction(GeneratorHandle handle, void* argument);

/** 
 * Creates a generator function with isolated scope. Only use directly if macros are not sufficient
 * @param func function representing an isolated scope to handle yielding data to a caller
 * @param bufferInSize size of data going into gen function
 * @param bufferOutSize size of data coming out of gen function
 * @param argument arbitrary data to pass to init call of gen function
 * @return GeneratorHandle: an opaque pointer to the generator function
*/
GeneratorHandle createGenerator(GeneratorFunction* func, size_t bufferInSize, size_t bufferOutSize, void* argument);

/** 
 * Gets the next value from a generator function
 * @param handle opaque pointer for a generator function
 * @param sent data to send to generator function. Can be set to NULL if no data sent
 * @param received data yielded to caller. Can be set to NULL if no data to be received
 * @return boolean: true gen function has not been closed, false if aborted
*/
bool GeneratorNext(GeneratorHandle handle, void* sent, void* received);

/** 
 * Yields a value from a generator function
 * @param handle opaque pointer for a generator function
 * @param sent data to yielded from generator function. Can be set to NULL if no data yielded
 * @param received data received from caller. Can be set to NULL if no data to be received
 * @param isDone signals the generator function if generator should be closed after yielding
 * @return boolean: true gen function has not been closed, false if aborted
*/
bool GeneratorYield(GeneratorHandle handle, void* sent, void* received, bool isDone);

/** 
 * Frees generator from heap
 * @param handle address of open GeneratorHandle. Will free and set handle to NULL
*/
void freeGenerator(GeneratorHandle* handle);

/** 
 * Macro for creating a generator function
 * @param func function representing an isolated scope to handle yielding data to a caller
 * @param TSEND the type of the data being sent into the generator function
 * @param TRECEIVE the type of data being yielded from generator function
 * @param input arbitrary data sent initially into the generator function
 * @return GeneratorHandle: an opaque pointer to the generator function
*/
#define gen_func(func, TSEND, TRECEIVE, input) createGenerator(func, sizeof(TSEND), sizeof(TRECEIVE), (void*)input)

/** 
 * Macro for getting the next value from a generator function
 * @param handle opaque pointer for a generator function
 * @param sent data to send to generator function. Can be set to NULL if no data sent
 * @param received data yielded to caller. Can be set to NULL if no data to be received
 * @return boolean: true gen function has not been closed, false if aborted
*/
#define gen_next(handle, sent, received) GeneratorNext(handle, (void*)sent, (void*)received)

/** 
 * Macro for yielding a value from a generator function
 * @param handle opaque pointer for a generator function
 * @param sent data to yielded from generator function. Can be set to NULL if no data yielded
 * @param received data received from caller. Can be set to NULL if no data to be received
 * @return boolean: true gen function has not been closed, false if aborted
*/
#define gen_yield(handle, sent, received) GeneratorYield(handle, (void*)sent, (void*)received, false)

/** 
 * Macro for yielding a final value from a generator function
 * @param handle opaque pointer for a generator function
 * @param sent data to yielded from generator function. Can be set to NULL if no data yielded
 * @param received data received from caller. Can be set to NULL if no data to be received
 * @return boolean: true gen function has not been closed, false if aborted
*/
#define gen_return(handle, sent, received) GeneratorYield(handle, (void*)sent, (void*)received, true)

#endif