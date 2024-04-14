#ifndef __generator_h_
#define __generator_h_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// opaque pointer for generator handle
typedef struct GeneratorContext GeneratorHandle;

// function type a generator function
typedef void GeneratorFunction(GeneratorHandle* handle);


/** 
 * Creates a generator function with isolated scope. Only use directly if macros are not sufficient
 * @param func function representing an isolated scope to handle yielding data to a caller
 * @param yieldDataSize size of data going into gen function
 * @param argument arbitrary data to pass to init call of gen function
 * @return GeneratorHandle: an opaque pointer to the generator function
*/
GeneratorHandle* CreateGenerator(GeneratorFunction* func, size_t yieldDataSize, void* argument);

//void GeneratorTrySetReturn(GeneratorHandle* handle, void* returnAddress, bool isDone);
void* GeneratorTryGoto(GeneratorHandle* handle, void* elseAddress, void** argument);
void* GeneratorNextFramePtr(GeneratorHandle* context, size_t size);

/** 
 * Gets the next value from a generator function
 * @param handle opaque pointer for a generator function 
 * @param received data yielded to caller. Can be set to NULL if no data to be received
 * @return boolean: true gen function has not been closed, false if aborted
*/
bool GeneratorNext(GeneratorHandle* handle, void* received);

/** 
 * Yields a value from a generator function
 * @param handle opaque pointer for a generator function
 * @param sent data to yielded from generator function. Can be set to NULL if no data yielded
 * @param received data received from caller. Can be set to NULL if no data to be received
 * @param isDone signals the generator function if generator should be closed after yielding
 * @return boolean: true gen function has not been closed, false if aborted
*/
//bool GeneratorYield(GeneratorHandle handle, void* sent, void* received, bool isDone);
bool GeneratorYield_First(GeneratorHandle* context, void* sent, void* returnAddress);
void GeneratorYield_Second(GeneratorHandle* context);
  

/** 
 * Frees generator from heap
 * @param handle address of open GeneratorHandle. Will free and set handle to NULL
*/
void freeGenerator(GeneratorHandle** handle);

/** 
 * Macro for creating a generator function
 * @param func function representing an isolated scope to handle yielding data to a caller
 * @param TSEND the type of the data being sent into the generator function
 * @param input arbitrary data sent initially into the generator function
 * @return GeneratorHandle: an opaque pointer to the generator function
*/
#define gen_func(func, TSEND, input) CreateGenerator(func, sizeof(TSEND), (void*)input)



/** 
 * Macro for getting the next value from a generator function
 * @param handle opaque pointer for a generator function
 * @param received data yielded to caller. Can be set to NULL if no data to be received
 * @return boolean: true gen function has not been closed, false if aborted
*/
#define gen_next(handle, received) GeneratorNext(handle, (void*)received)

// support macro for concatenating tokens
#define __generator_build_label(x) GENLABEL ## x

// support macro for generator yield
#define __generator_build_yield(counter, context, sent) do{\
    if(GeneratorYield_First(context, (void*)sent, &&__generator_build_label(counter))){\
    return;\
    }\
    __generator_build_label(counter): GeneratorYield_Second(context);\
    }while(0)

/** 
 * Macro for yielding a value from a generator function
 * @param handle opaque pointer for a generator function
 * @param sent data to yielded from generator function. Can be set to NULL if no data yielded
 * @return boolean: true gen function has not been closed, false if aborted
*/
#define gen_yield(context, sent) __generator_build_yield(__COUNTER__, context, sent)

#define gen_restore(context, Tfield, name) Tfield* name = (Tfield*)GeneratorNextFramePtr(context, sizeof(Tfield))

// support macro for building jmp
#define __generator_frame_initialization(counter, context, Tfield, name) Tfield* name;\
do{\
goto *(GeneratorTryGoto(context, &&__generator_build_label(counter), (void**)&name));\
__generator_build_label(counter):\
}while(0)

#define gen_frame_init(context, Tfield, name) __generator_frame_initialization(__COUNTER__, context, Tfield, name)


#endif