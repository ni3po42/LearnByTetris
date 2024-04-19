#include <esp_random.h>

#ifdef __cplusplus
extern "C" {
#endif


void initRand() {   
}

int getRandomIntFromRange(int min, int max) {
    return (esp_random() % (max - min + 1)) + min;
}

#ifdef __cplusplus
}
#endif


