#ifndef _game_h_
#define _game_h_

#include <stdbool.h>
#include "../events/events.h"
#include "../generator/generator.h"
#include "../board/board.h"
#include "../piece/piece.h"

// defines a game level
typedef int gamelevel_t;

// defines an interval
typedef int interval_t;

// structure to inform of game's state
typedef struct GameStatus {

    // player's current level
    gamelevel_t level;

    // flag if game is over
    bool gameover;

    // player's current score
    int score;
    
    // bit field of rows that collapsed
    collapsed_rows_t clearRows;
    
    // the next piece to drop
    NextPieceData nextPieceData;
    
} GameStatus;

/** 
 * XOR's a given piece's data with data in game board
 * @param piece pointer to Piece to XOR data
*/
void xorGameBoard(Piece* piece);

/**
 * Map a game level to an interval
 * @param level level to determine an interval for
 * @return the interval for a given level
*/
interval_t getInterval(gamelevel_t level);

/**
 * Starts coroutine for running a game
 * @param eventStreamHandle handle to the event stream to accquire game status
 * @param startLevel init level to start game
 * @return a genertor handle that yields a GameStatus
*/
GeneratorHandle doGameLoop(GeneratorHandle eventStreamHandle, gamelevel_t startLevel);

#endif