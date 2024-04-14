#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include "../events/events.h"
#include "../board/board.h"
#include "../piece/piece.h"
#include "../generator/generator.h"
#include "../render/render.h"
#include "game.h"

/**
 * private apis
*/

// map for number of rows (index) to a score (value at index)
static int scoring[5] = {
     0,
     40,
    100,
    300,
    1200
};

// get min of two integers
static int minInt(int a, int b) {
    return a < b ? a : b;
}

// get max of two integers
static int maxInt(int a, int b) {
    return a < b ? b : a;
}

// xor's board data with piece's data
static void xorGameBoard(Piece* piece) {
    pieceScan(piece, false, xorBoardCell);
}

// determines of a given piece causes a collision on board in it's current state
static bool gameHasCollision(Piece* piece) {
    
    BoardScanArguments args = {
        .col = getPieceCol(piece), .row = getPieceRow(piece), .height = getPieceHeight(piece), .width = getPieceWidth(piece)
    };
    GeneratorHandle* handle = scanBoard(&args);
    BoardScanData scanData;
    
    while(gen_next(handle, &scanData)) {
        if (scanData.collision) {
            freeGenerator(&handle);
            return true;
        }
    }
    
    freeGenerator(&handle);
    
    return false;
}

// determines which rows, if any, are being collapsed
static collapsed_rows_t getClearRows(Piece* piece) {
    //pieces at most can span 4 rows;
    int rowCounts[4] = {0,0,0,0};
    int row = getPieceRow(piece);
    BoardScanArguments args = {
        .row = row, .col = 0, .height = getPieceHeight(piece), .width = BOARD_COLS
    };
    GeneratorHandle* handle = scanBoard(&args);
    BoardScanData scanData;
    
    collapsed_rows_t clearedRows = 0;
    
    int index;
    int val;
    
    while(gen_next(handle, &scanData)) {
        index = scanData.row - row;
        val = (scanData.data & WALL_MASK) ? 0 : (scanData.data & (ACTIVE_MASK | STATIC_MASK));
        rowCounts[index] += (val > 0) ? 1 : 0;
    }
    
    freeGenerator(&handle);
    
    for(index = 0; index < 4; index++) {
        
        if (rowCounts[index] == BOARD_COLS - 2) {
            clearedRows |= ( 1 << (index + row));
        }
        
    }
    
    return clearedRows;
}

// gets the number of collapsed rows in mask
static int countClearRows(collapsed_rows_t clearedRows) {
    int count = 0;
    while(clearedRows) {
        if ((1 & clearedRows) == 1) {
            count++;
        }
        clearedRows >>= 1;
    }
    return count;
}

// define the actual function loop for generator
static void doGameLoop_genFunc(GeneratorHandle* gameLoop) {
    
    gen_restore(gameLoop, gamelevel_t, startLevel);
    gen_restore(gameLoop, int, linesCleared);
    gen_restore(gameLoop, int, linesToClear);
    gen_restore(gameLoop, bool, wasDropped);
    gen_restore(gameLoop, int, clearRowCount);
    gen_restore(gameLoop, GeneratorHandle*, piecesHandle);
    gen_restore(gameLoop, Piece*, currentPiece);
    gen_restore(gameLoop, GameStatus, status);
    gen_frame_init(gameLoop, GameLoopArguments, loopArgs);
    
    *startLevel = loopArgs->startLevel;
    *linesCleared = 0;
    *linesToClear = 0;
    *wasDropped = false;
    *clearRowCount = 0;

    status->score = 0;
    status->gameover = false;
    status->level = loopArgs->startLevel;
    status->clearRows = 0;
    
    *piecesHandle = pieces();
    *currentPiece = NULL;
    
    // get next pieces
    while(gen_next(*piecesHandle, &status->nextPieceData)){
        
        *linesToClear = minInt(status->level * 10 + 10, maxInt(100, status->level * 10 - 50));
    
        *currentPiece = status->nextPieceData.currentPiece;
        
        //draw
        xorGameBoard(*currentPiece);
        
        status->gameover = gameHasCollision(*currentPiece);

        // yield game status.
        gen_yield(gameLoop, status);
   
        // get next event
        EventMessage eventData;
        while(gen_next(loopArgs->eventStream, &eventData)) {
            
            //clear
            xorGameBoard(*currentPiece);
            
            *wasDropped = false;
            switch(eventData.type) {
                case DROP: 
                    dropPiece(*currentPiece); 
                    *wasDropped = true; 
                    break;
                case TURN_LEFT: 
                    rotatePieceLeft(*currentPiece); 
                    break;
                case TURN_RIGHT: 
                    rotatePieceRight(*currentPiece); 
                    break;
                case LEFT: 
                    leftPiece(*currentPiece); 
                    break;
                case RIGHT: 
                    rightPiece(*currentPiece); 
                    break;
                case EXIT_GAME:   
                    gen_yield(gameLoop, status);
                    freeGenerator(piecesHandle);
                    return;
            }

            //draw new
            xorGameBoard(*currentPiece);

            if (gameHasCollision(*currentPiece)) {
                
                //clear
                xorGameBoard(*currentPiece);                
                revertPiece(*currentPiece);
                
                if (*wasDropped) {
                    commitPiece(*currentPiece);
                }
                
                //draw
                xorGameBoard(*currentPiece);

                if (*wasDropped) {
                    break;
                }
            }
            
            gen_yield(gameLoop, status);
        }
        
        status->clearRows = getClearRows(*currentPiece);
        *clearRowCount = countClearRows(status->clearRows);
        status->score += scoring[*clearRowCount] * (status->level + 1);
        *linesCleared += *clearRowCount;

        int bound = *linesToClear + ((status->level - *startLevel) * 10);
        
        if (*linesCleared >= bound) {
            (status->level)++;
            *linesCleared -= bound;
        }
        
        gen_yield(gameLoop, status);
        
        collapseBoard(status->clearRows);
    }
    
    freeGenerator(piecesHandle);    
}

/**
 * public apis
*/

interval_t getInterval(gamelevel_t level) {
    int frames;
    if( level <= 8) {
        frames = (48 - level * 5);
    } else if (level == 9) {
        frames = 6;
    } else if (level <= 12) {
        frames = 5;
    } else if (level <= 15) {
        frames = 4;
    } else if (level <= 18) {
        frames = 3;
    } else if (level <= 28) {
        frames = 2;
    } else {
        frames = 1;
    }

    return (1000 * frames) / 6;
}

GeneratorHandle* doGameLoop(GameLoopArguments* args) {
    return gen_func(doGameLoop_genFunc, GameStatus, args);
}