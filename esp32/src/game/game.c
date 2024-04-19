#include "core.h"
#include "game.h"
#include <stdbool.h>
#include <stdio.h>
#include "events.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif


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
        .col = getPieceCol(piece), 
        .row = getPieceRow(piece), 
        .height = getPieceHeight(piece), 
        .width = getPieceWidth(piece),
        .start = true      
    };

    BoardScanData scanData;

    while (scanBoard(&args, &scanData)) {
        if (scanData.collision) {
            return true;
        }
    }

    return false;
}

// determines which rows, if any, are being collapsed
static collapsed_rows_t getClearRows(Piece* piece) {
    //pieces at most can span 4 rows;
    int rowCounts[4] = {0,0,0,0};
    int row = getPieceRow(piece);
    BoardScanArguments args = {
        .row = row, 
        .col = 0, 
        .height = getPieceHeight(piece), 
        .width = BOARD_COLS,
        .start = true
    };

    collapsed_rows_t clearedRows = 0;
    
    int index;
    int val;

    BoardScanData scanData;
    
    while(scanBoard(&args, &scanData)) {
        index = scanData.row - row;
        val = (scanData.data & WALL_MASK) ? 0 : (scanData.data & (ACTIVE_MASK | STATIC_MASK));
        rowCounts[index] += (val > 0) ? 1 : 0;
    }
        
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

void doGameLoop(gamelevel_t startLevel, GameStatusHandler* callback) {
     
    int linesCleared = 0;
    int linesToClear = 0;
    bool wasDropped = false;
    int clearRowCount = 0;
    GameStatus status = {
        .score = 0,
        .gameover = false,
        .level = startLevel,
        .clearRows = 0,
        .leveledUp = false,
        .id = 0
    };
    
    Piece* currentPiece = getCurrentPiece();

    initPieceSequence(getRandomIntFromRange(0,6));

    // get next pieces
    for(;;){
        promoteNextPiece(getRandomIntFromRange(0,6));

        linesToClear = minInt(status.level * 10 + 10, maxInt(100, status.level * 10 - 50));
    
        //draw
        xorGameBoard(currentPiece);
        
        status.gameover = gameHasCollision(currentPiece);

        // yield game status.
        status.id = 1;
        callback(&status);
   
        // get next event
        EventMessage eventData;
        for(;;) {
            getNextMessage(&eventData);

            if (eventData == NO_EVENT) {                
                callback(NULL);
                continue;
            }            
            
            //clear
            xorGameBoard(currentPiece); 

            wasDropped = false;
            switch(eventData) {
                case DROP: 
                    dropPiece(currentPiece); 
                    wasDropped = true; 
                    break;
                case TURN_LEFT: 
                    rotatePieceLeft(currentPiece); 
                    break;
                case TURN_RIGHT: 
                    rotatePieceRight(currentPiece); 
                    break;
                case LEFT: 
                    leftPiece(currentPiece); 
                    break;
                case RIGHT: 
                    rightPiece(currentPiece); 
                    break;
                case NO_EVENT:
                    //shouldn't get here, but let's handle this case
                    //just in case
                    //draw
                    xorGameBoard(currentPiece);
                    status.id = 0;
                    callback(NULL);
                    continue;
                case EXIT_GAME:
                    status.gameover = true;
                    status.id = 0;
                    callback(&status);
                    return;
            }

            //draw new            
            xorGameBoard(currentPiece);
            status.id = 1;

            if (gameHasCollision(currentPiece)) {
                
                //clear
                xorGameBoard(currentPiece);
                status.id = 0;                
                revertPiece(currentPiece);
                
                if (wasDropped) {
                    commitPiece(currentPiece);
                }
                
                //draw
                xorGameBoard(currentPiece);
                status.id = 1;

                if (wasDropped) {
                    break;
                }
            }
            
            //status.id = 4;
            callback(&status);
        }
        
        status.clearRows = getClearRows(currentPiece);
        clearRowCount = countClearRows(status.clearRows);
        status.score += scoring[clearRowCount] * (status.level + 1);
        linesCleared += clearRowCount;

        int bound = linesToClear + ((status.level - startLevel) * 10);
        
        if (linesCleared >= bound) {
            (status.level)++;
            linesCleared -= bound;
        }
        
        //status.id = 5;
        callback(&status);
        
        collapseBoard(status.clearRows);
    }
     
}

#ifdef __cplusplus
}
#endif
