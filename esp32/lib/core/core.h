#ifndef __constants_h_
#define __constants_h_

#include <stdint.h>
#include <stdbool.h>

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

#define DROP 1
#define LEFT 2
#define RIGHT 3
#define TURN_LEFT 4
#define TURN_RIGHT 5
#define PAUSE 6
#define EXIT_GAME 7

// defines number of cells on board
#define BOARD_SIZE (BOARD_COLS*BOARD_ROWS)

#ifdef __cplusplus
extern "C" {
#endif


/** 
 * Cell data for piece or board information. CellData is a bit field
 * 0wsa0ccc
 * w - wall bit. specifies if cell is a wall
 * s - static. specifies if cell is static (no longer moving)
 * a - active. specifies if cell is active (still moving). 
 * ccc - color code for cell. client defined color mapping
 * 0 - unused bits
 * if a and c are 1, than a collision has occurred
*/
typedef uint8_t CellData;


// defines a game level
typedef int gamelevel_t;

// defines an interval
typedef int interval_t;

typedef uint8_t color_t;

// defines board to be array of integers
typedef CellData Board[BOARD_SIZE];

// used as bit-field to track which rows are to be collapsed
typedef uint32_t collapsed_rows_t;

// structure to inform of game's state
typedef struct GameStatus {

    // player's current level
    gamelevel_t level;

    // flag true if level increased
    bool leveledUp;

    // flag if game is over
    bool gameover;

    // player's current score
    int score;
    
    // bit field of rows that collapsed
    collapsed_rows_t clearRows;

    int id;    
    
} GameStatus;

void initRand();
int getRandomIntFromRange(int min, int max);

#ifdef __cplusplus
}
#endif


#endif