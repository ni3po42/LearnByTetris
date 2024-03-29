#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include "../events/events.h"
#include "../board/board.h"
#include "../piece/piece.h"
#include "../generator/generator.h"
#include "../render/render.h"
#include "game.h"

static int scoring[5] = {
     0,
     40,
    100,
    300,
    1200
};

static int minInt(int a, int b) {
    return a < b ? a : b;
}

static int maxInt(int a, int b) {
    return a < b ? b : a;
}

void xorGameBoard(Piece* piece) {
    pieceScan(*piece, false, xorBoardCell);
}

bool gameHasCollision(Piece* piece) {
    
    GeneratorHandle handle = scanBoard(getPieceRow(piece), getPieceCol(piece), piece->scanHeight, piece->scanWidth);
    BoardScanData scanData;
    
    while(gen_next(handle, NULL, &scanData)) {
        if (scanData.collision) {
            freeGenerator(&handle);
            return true;
        }
    }
    
    freeGenerator(&handle);
    
    return false;
}

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

static collapsed_rows_mask_t getClearRows(Piece* piece) {
    //pieces at most can span 4 rows;
    int rowCounts[4] = {0,0,0,0};
    int row = getPieceRow(piece);
    GeneratorHandle handle = scanBoard(row, 0, piece->scanHeight, BOARD_COLS);
    BoardScanData scanData;
    
    collapsed_rows_mask_t clearedRows = 0;
    
    int index;
    int val;
    
    while(gen_next(handle, NULL, &scanData)) {
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

int countClearRows(collapsed_rows_mask_t clearedRows) {
    int count = 0;
    while(clearedRows) {
        if ((1 & clearedRows) == 1) {
            count++;
        }
        clearedRows >>= 1;
    }
    return count;
}

static void doGameLoop_genFunc(GeneratorHandle gameLoop, void* argument) {
    
    GameLoopArguments* loopArgs = (GameLoopArguments*)argument;
    
    GameStatus status;
    EventMessage eventData;
    
    status.score = 0;
    status.gameover = false;
    gamelevel_t startLevel = loopArgs->startLevel;
    status.level = loopArgs->startLevel;
    status.clearRows = 0;
    GeneratorHandle stream = loopArgs->eventStream;
    
    Piece* currentPiece = NULL;
    
    free(argument);
    
    int linesToClear = minInt(status.level * 10 + 10, maxInt(100, status.level * 10 - 50));
    int linesCleared = 0;
    bool wasDropped = false;
    int clearRowCount = 0;
    bool isDone = false;

    GeneratorHandle piecesHandle = pieces();
    
    while(gen_next(piecesHandle, NULL, &status.nextPieceData)){
       
       
       
        currentPiece = &status.nextPieceData.currentPiece;
        
        //draw
        xorGameBoard(currentPiece);
        
        status.gameover = gameHasCollision(currentPiece);

        if(!gen_yield(gameLoop, &status, NULL)) {
            freeGenerator(&piecesHandle);
            return;
        }
   
        while(gen_next(stream, NULL, &eventData)) {
            
            //clear
            xorGameBoard(currentPiece);
            
            wasDropped = false;
            switch(eventData.type) {
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
                case EXIT_GAME: 
                     
                    if(!gen_return(gameLoop, &status, NULL)) {
                        freeGenerator(&piecesHandle);
                        return;
                    }
                    
                    break;
            }


            //draw new
            xorGameBoard(currentPiece);

            if (gameHasCollision(currentPiece)) {
                
                //clear
                xorGameBoard(currentPiece);
                
                revertPiece(currentPiece);
                
                if (wasDropped) {
                    commitPiece(currentPiece);
                }
                
                //draw
                xorGameBoard(currentPiece);

                if (wasDropped) {
                    break;
                }
            }
            
            if(!gen_yield(gameLoop, &status, NULL)) {
                return;
            }
        }
        
        status.clearRows = getClearRows(currentPiece);

        clearRowCount = countClearRows(status.clearRows);

        status.score += scoring[clearRowCount] * (status.level + 1);

        linesCleared += clearRowCount;

        int bound = linesToClear + ((status.level - startLevel) * 10);
        
        if (linesCleared >= bound) {
            status.level++;
            linesCleared -= bound;
        }
        
        if (!gen_yield(gameLoop, &status, NULL)) {
            freeGenerator(&piecesHandle);
            return;
            
        }
        
        collapseBoard(status.clearRows);
    }
    
    freeGenerator(&piecesHandle);
    
}

GeneratorHandle doGameLoop(GeneratorHandle eventStreamHandle, gamelevel_t startLevel) {
    
    GameLoopArguments* argument = (GameLoopArguments*)malloc(sizeof(GameLoopArguments));
    
    argument->startLevel = startLevel;
    argument->eventStream = eventStreamHandle;
    
    return gen_func(doGameLoop_genFunc, void, GameStatus, argument);
}