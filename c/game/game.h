#ifndef _game_h_
#define _game_h_

#include <stdbool.h>
#include "../events/events.h"
#include "../generator/generator.h"
#include "../board/board.h"
#include "../piece/piece.h"

typedef int gamelevel_t;
typedef int interval_t;

typedef struct GameStatus {
    gamelevel_t level;
    bool gameover;
    int score;
    
    collapsed_rows_mask_t clearRows;
    
    NextPieceData nextPieceData;
    
} GameStatus;

typedef struct GameLoopArguments {
    gamelevel_t startLevel;
    GeneratorHandle eventStream;
} GameLoopArguments;

void xorGameBoard(Piece* piece);
bool gameHasCollision(Piece* piece);
interval_t getInterval(gamelevel_t level);
GeneratorHandle doGameLoop(GeneratorHandle eventStreamHandle, gamelevel_t startLevel);

#endif