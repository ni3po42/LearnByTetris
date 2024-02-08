#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

#include "generator.h"
#include "generator.internal.h"

#include "../testing/utils.h"


void gen_function1(GeneratorHandle handle, void* argument) {
}
void TEST_can_create_generator_function() {
    int start = 1;
    GeneratorHandle handle = gen_func(gen_function1, void, void, &start);
    
    GeneratorContext* context = (GeneratorContext*)handle;
    assert(context != NULL);
    
    freeGenerator(&handle);
    
    assert((GeneratorContext*)handle == NULL);
}

void gen_function2(GeneratorHandle handle, void* argument) {
    int i = *((int*)argument);
    i+=i;
    gen_yield(handle, &i, NULL);
}
void TEST_can_yield() {
    
    int start = 1;
    GeneratorHandle handle = gen_func(gen_function2, int, int, &start);
    int inVal = 0;
    int val;
    gen_next(handle, &inVal, &val);
    
    assert(val == 2);
    
    freeGenerator(&handle);
}


void gen_function3(GeneratorHandle handle, void* argument) {
    int i = *((int*)argument);
    
    i += i;
    gen_yield(handle, &i, NULL);
    i += i;
    gen_yield(handle, &i, NULL);
    i += i;
    gen_yield(handle, &i, NULL);
}
void TEST_can_yield_x3() {
    
    int start = 1;
    GeneratorHandle handle = gen_func(gen_function3, int, int, &start);
    
    gen_next(handle, NULL, NULL);
    gen_next(handle, NULL, NULL);
    int val;
    gen_next(handle, NULL, &val);
    
    assert(val == 8);
    
    freeGenerator(&handle);
}

void gen_function4(GeneratorHandle handle, void* argument) {
    int i = *((int*)argument);
    int j;//11
    
    gen_yield(handle, &i, &j);//j = yield i
    
    j++;
    gen_yield(handle, &j, NULL);//yield j
}
void TEST_can_use_yield() {
    
    int start = 1;
    
    int v1 = 10;
    int v2;//1
    int v3;//11
    
    GeneratorHandle handle = gen_func(gen_function4, int, int, &start);
    
    gen_next(handle, &v1, &v2);
    gen_next(handle, NULL, &v3);
    
    assert(v1 == 10);
    assert(v2 == 1);
    assert(v3 == 11);
    
    freeGenerator(&handle);
}

void gen_function5(GeneratorHandle handle, void* argument) {
    int i = *((int*)argument);
    
    int val = 3141;
    gen_yield(handle, &val, NULL);
}
void TEST_return_value() {
    
    int start = 1;
    GeneratorHandle handle = gen_func(gen_function5, void, int, &start);
    
    int val1;
    gen_next(handle, NULL, &val1);
    
    assert(val1 == 3141);
    
    freeGenerator(&handle);
}

void gen_function6(GeneratorHandle handle, void* argument) {
    int i = *((int*)argument);
    
    int val = 3141;
    gen_return(handle, &val, NULL);
}
void TEST_is_done_immediate() {
    int start = 1;
    GeneratorHandle handle = gen_func(gen_function6, void, int, &start);
    
    bool isDone = !gen_next(handle, NULL, NULL);
    
    assert(isDone);
    
    freeGenerator(&handle);
}

void gen_function7(GeneratorHandle handle, void* argument) {
   
    int i = 0;
    while(true) {
        if (!gen_yield(handle, &i, NULL)) {
            break;
        }
        i++;
    }
}
void TEST_can_abort_infinite_loop() {
    
    GeneratorHandle handle = gen_func(gen_function7, void, int, NULL);
    
    int output;
    
    gen_next(handle, NULL, &output);
    assert(output == 0);
    gen_next(handle, NULL, &output);
    assert(output == 1);
    gen_next(handle, NULL, &output);
    assert(output == 2);
    gen_next(handle, NULL, &output);
    assert(output == 3);
    
    freeGenerator(&handle);
}

void gen_function8(GeneratorHandle handle, void* argument) {
    int i = 42;
    
    gen_yield(handle, &i, NULL);
}
void TEST_no_start() {
    
    GeneratorHandle handle = gen_func(gen_function8, void, int, NULL);
    
    int output;
    
    gen_next(handle, NULL, &output);
    assert(output == 42);
    
    freeGenerator(&handle);
}


void gen_function_2deep2(GeneratorHandle handle, void* argument) {
    int output = 40;
    gen_yield(handle, &output, NULL);
    
}
void gen_function_2deep1(GeneratorHandle handle, void* argument) {
    GeneratorHandle handle2 = gen_func(gen_function_2deep2, int, int, NULL);
    
    int output;
    int input = 20;
    
    gen_next(handle2, &input, &output);
    
    assert(output == 40);
    
    freeGenerator(&handle2);
    
    gen_yield(handle, &output, NULL);
}
void TEST_deep_generator() {
    
    GeneratorHandle handle = gen_func(gen_function_2deep1, int, int, NULL);
    
    int output;
    int input = 20;
    
    gen_next(handle, &input, &output);
    
    assert(output == 40);
    
    freeGenerator(&handle);
}

typedef struct InnerData {
    int* d1;
    int d2;
    int d3;
    int d4;
    int d5;
    int d6;
    int d7;
    int d8;
    int d9;
    int d10;
    int d11;
    int d12;
    int d13;
} InnerData;

typedef struct OuterData {
    int d1;
    bool d2;
    int d3;
    uint64_t d4;
    InnerData innerData;
} OuterData;


void gen_function9(GeneratorHandle handle, void* argument) {
    OuterData outer;
   
    outer.d1 = 1;
    outer.d2 = true;
    outer.d3 = 3;
    outer.d4 = 4;
    
    outer.innerData.d1 = NULL;
    outer.innerData.d2 = 2;
    outer.innerData.d3 = 3;
    outer.innerData.d4 = 4;
    outer.innerData.d5 = 5;
    outer.innerData.d6 = 6;
    outer.innerData.d7 = 7;
    outer.innerData.d8 = 8;
    outer.innerData.d9 = 9;
    outer.innerData.d10 = 10;
    outer.innerData.d11 = 11;
    outer.innerData.d12 = 12;
    outer.innerData.d13 = 13;
    
    gen_yield(handle, &outer, NULL);
}
void TEST_can_yield_struct() {
    
    GeneratorHandle handle = gen_func(gen_function9, void, OuterData, NULL);
    
    OuterData outer;
    
    gen_next(handle, NULL, &outer);
    
    
    assert(outer.d1 == 1);
    assert(outer.d2 == true);
    assert(outer.d3 == 3);
    assert(outer.d4 == 4);
    
    assert(outer.innerData.d1 == NULL);
    assert(outer.innerData.d2 == 2);
    assert(outer.innerData.d3 == 3);
    assert(outer.innerData.d4 == 4);
    assert(outer.innerData.d5 == 5);
    assert(outer.innerData.d6 == 6);
    assert(outer.innerData.d7 == 7);
    assert(outer.innerData.d8 == 8);
    assert(outer.innerData.d9 == 9);
    assert(outer.innerData.d10 == 10);
    assert(outer.innerData.d11 == 11);
    assert(outer.innerData.d12 == 12);
    assert(outer.innerData.d13 == 13);
    
    
    freeGenerator(&handle);
}


int main(int argc, char *argv[]) 
{
    // run(TEST_can_create_generator_function);
    // run(TEST_can_yield);
    // run(TEST_can_yield_x3);
    // run(TEST_can_use_yield);
    // run(TEST_return_value);
    // run(TEST_is_done_immediate);
    // run(TEST_can_abort_infinite_loop);
    // run(TEST_no_start);
    
    // run(TEST_deep_generator);
    run(TEST_can_yield_struct);
    
    
    printf("\ndone\n");
}