#ifndef __testing_utils_h_
#define __testing_utils_h_

#define run(func) ({ \
printf("running: " #func "\n"); \
func(); \
})

#endif
