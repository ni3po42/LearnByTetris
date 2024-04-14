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

// arguments for starting a game loop 
typedef struct GameLoopArguments {
    gamelevel_t startLevel;
    GeneratorHandle* eventStream;
} GameLoopArguments;


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
 * Map a game level to an interval
 * @param level level to determine an interval for
 * @return the interval for a given level
*/
interval_t getInterval(gamelevel_t level);

/**
 * Starts coroutine for running a game
 * @param args arguments for starting game loop
 * @return a generator handle that yields a GameStatus
*/
GeneratorHandle* doGameLoop(GameLoopArguments* args);

#endif