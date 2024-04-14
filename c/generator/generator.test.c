#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

#include "generator.h"
#include "generator.internal.h"

#include "../testing/utils.h"


void gen_function1(GeneratorHandle* handle) {
}
void TEST_can_create_generator_function() {
    int start = 1;
    GeneratorHandle* handle = gen_func(gen_function1, void, &start);
    
    GeneratorContext* context = (GeneratorContext*)handle;
    assert(context != NULL);
    
    freeGenerator(&handle);
    
    assert((GeneratorContext*)handle == NULL);
}

void gen_function2(GeneratorHandle* handle) {
    gen_restore(handle, int, i);  
    gen_frame_init(handle, int, argument);
    *i = *argument;  
    *i+=*i;     
    gen_yield(handle, i);
}
void TEST_can_yield() {
    
    int inVal = 1;
    GeneratorHandle* handle = gen_func(gen_function2, int, &inVal);
   
    int val;
    gen_next(handle, &val);
    
    assert(val == 2);
    
    freeGenerator(&handle);
}

void gen_function3(GeneratorHandle* handle) {
    gen_restore(handle, int, i);   
    gen_frame_init(handle, int, argument);  
    *i = *argument;
    *i += *i;
    gen_yield(handle, i);
    *i += *i;
    gen_yield(handle, i);
    *i += *i;
    gen_yield(handle, i);
}
void TEST_can_yield_x3() {
    
    int start = 1;
    GeneratorHandle* handle = gen_func(gen_function3, int, &start);
    GeneratorContext* c = handle;
    gen_next(handle, NULL);
    gen_next(handle, NULL);
    int val;
    gen_next(handle, &val);
    assert(val == 8);
    
    freeGenerator(&handle);
}

void gen_function4(GeneratorHandle* handle) {
    gen_restore(handle, int, i);
    gen_frame_init(handle, int, j);
    *i = *j;
    
    // i = 1
    gen_yield(handle, i);
    // j= 10
    
    (*j)++;
    // j= 11
    gen_yield(handle, j);
}
void TEST_can_use_yield() {
    
    int input = 1;
    
    int v1 = 10;
    int v2;//1
    int v3;//11
    
    GeneratorHandle* handle = gen_func(gen_function4, int, &input);    
    //i = 1, j= 1

    gen_next(handle, &v2);
    //v2 = 1, j = 1

    input = 10;
    //j = 10
    
    gen_next(handle, &v3);
    //v3 = 11, j = 11
    
    gen_next(handle, &v3);
    //v3 = 11 (unchanged)

    assert(v1 == 10);
    assert(v2 == 1);
    assert(v3 == 11);
    
    freeGenerator(&handle);
}

void gen_function5(GeneratorHandle* handle) {
    gen_restore(handle, int, i);
    gen_restore(handle, int, val);    
    gen_frame_init(handle, int, argument);
    *i = *argument;

    *val = 3141;
    gen_yield(handle, val);
}
void TEST_return_value() {
    
    int start = 1;
    GeneratorHandle* handle = gen_func(gen_function5, int, &start);
    
    int val1= 42;
    gen_next(handle, &val1);
    assert(val1 == 3141);
    
    freeGenerator(&handle);
}

void gen_function6(GeneratorHandle* handle) {
    gen_restore(handle, int, i);
    gen_restore(handle, int, val);    
    gen_frame_init(handle, int, argument);
    *i = *argument;

    *val = 3141;
    gen_yield(handle, val);
}
void TEST_is_done_immediate() {
    int start = 1;
    GeneratorHandle* handle = gen_func(gen_function6, void, &start);
    
    gen_next(handle, NULL);
    bool isDone = !gen_next(handle, NULL);

    assert(isDone);
    
    freeGenerator(&handle);
}

void gen_function_2deep2(GeneratorHandle* handle) {    
    gen_frame_init(handle, void, argument);

    int output = 40;
    gen_yield(handle, &output);
}
void gen_function_2deep1(GeneratorHandle* handle) {
    gen_restore(handle, int, output);
    gen_restore(handle, int, input);
    gen_restore(handle, GeneratorHandle, handle2);
    gen_frame_init(handle, void, argument);

    handle2 = gen_func(gen_function_2deep2, int, input);
    
    *input = 20;
    
    gen_next(handle2, output);
    
    assert(*output == 40);
    
    freeGenerator(&handle2);
    
    gen_yield(handle, output);
}
void TEST_deep_generator() {
    
    int input = 20;
    GeneratorHandle* handle = gen_func(gen_function_2deep1, int, &input);
    
    int output;
    
    gen_next(handle, &output);
    
    assert(output == 40);
    
    freeGenerator(&handle);
}

int main(int argc, char *argv[]) 
{
    run(TEST_can_create_generator_function);
    run(TEST_can_yield);
    run(TEST_can_yield_x3);
    run(TEST_can_use_yield);
    run(TEST_return_value);
    run(TEST_is_done_immediate);
    
    run(TEST_deep_generator);
    
    printf("\ndone\n");
}

