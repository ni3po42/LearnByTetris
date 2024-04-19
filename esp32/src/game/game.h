#ifndef _game_h_
#define _game_h_

#include "board.h"
#include "piece.h"
#include "core.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void GameStatusHandler(GameStatus* status);

/**
 * Map a game level to an interval
 * @param level level to determine an interval for
 * @return the interval for a given level
*/
interval_t getInterval(gamelevel_t level);

/**
 * Starts coroutine for running a game
 * @param startLevel level to start game at
 * @param callback handles updates to game status
*/
void doGameLoop(gamelevel_t startLevel, GameStatusHandler* callback);

#ifdef __cplusplus
}
#endif


#endif