#ifndef __constants_h_
#define __constants_h_




#define BOARD_ROWS (21)
#define BOARD_COLS (12)

#define ACTIVE_MASK (0x20)
#define STATIC_MASK (0x40)
#define WALL_MASK (0x80)

#define DELETE_MASK (0xf00)

#define NONE 0
#define DROP 1
#define LEFT 2
#define RIGHT 3
#define TURN_LEFT 4
#define TURN_RIGHT 5
#define PAUSE 6
#define EXIT_GAME 7

#endif