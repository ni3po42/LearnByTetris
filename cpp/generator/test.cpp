#include "generator.hpp"
#include "../test/utils.hpp"

using tetris::generator::Generator;

Generator<int> generator1() {

    for(int i=0;i<5;i++) {
        co_yield i;
    }

}

void TEST_can_create_generator() {

    auto gen = generator1();

    for(int i = 0; i < 5; i++) {
        
        Expect(i, gen().value_or(-1), "gen values do not match");

    }
    
}


int main() {

    run(TEST_can_create_generator);

    return 0;
}