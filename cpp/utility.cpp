#include <cstdlib>
#include <ctime>

namespace tetris::utilities {

    void initRand() {
        srand(time(NULL));   // Initialization, should only be called once.
    }

    int getRandomIntFromRange(int min, int max) {
        return (rand() % (max - min + 1)) + min;
    }

}
