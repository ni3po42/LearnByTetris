#ifndef __constants_h_
#define __constants_h_




#define BOARD_ROWS (21)
#define BOARD_COLS (12)

#define ACTIVE_MASK (0x20)
#define STATIC_MASK (0x40)
#define WALL_MASK (0x80)

#define EMPTY_CELL_DATA (0X00)
#define COLOR_MASK (0X07)

// color of wall
#define WALL_COLOR (0x07)
// used as parameter for constructBoard
#define CONSTRUCT_WHOLE_BOARD -1


#define NONE 0
#define DROP 1
#define LEFT 2
#define RIGHT 3
#define TURN_LEFT 4
#define TURN_RIGHT 5
#define PAUSE 6
#define EXIT_GAME 7

#endif