/*
    Game board implementation
*/

#include "board.h"
#include <stdbool.h>
#include <stdio.h>

#include "../generator/generator.h"

static Board board;

void constructBoard(int stopIndex) {
    
    int i;
    int c;
    int r;
    
    for(i = 0; i < BOARD_SIZE; i++) {
        if (stopIndex == i) {
            break;
        }
        c = i % BOARD_COLS;
        r = i / BOARD_COLS;
        if (c == 0 || c == (BOARD_COLS - 1) || r == (BOARD_ROWS - 1)) {
            board[i] = (STATIC_MASK | WALL_MASK | WALL_COLOR);
        } else {
            board[i] = 0x00;
        }
    }
}

int addCollapseRowMask(collapsed_rows_mask_t rowMask, int row) {
    return (1 << row) | rowMask;
}

void collapseBoard(collapsed_rows_mask_t rowMask) {
    
    int i;
    int finger = BOARD_SIZE;
    int row;
    
    int rowBit;
    
    for(i=BOARD_SIZE-1;i>=0;i--) {
        row = i / BOARD_COLS;
        
        rowBit = (1 << row);
        
        if (!(rowBit & rowMask)) {
            board[--finger] = board[i];
        }
    }
    constructBoard(finger);
}

int getBoardCell(int row, int col) {
    return board[row * BOARD_COLS + col];
}

void setBoardCell(int row, int col, int data) {
    int index = row * BOARD_COLS + col;
    board[index] = data; 
}

void xorBoardCell(int row, int col, int data) {
    int index = row * BOARD_COLS + col;
    board[index] ^= data;
}

// internal callback to handle generator function
static void scanBoard_genFunc(GeneratorHandle handle, void* argument) {
    
    BoardScanArguments* scanArguments = (BoardScanArguments*)argument;
    int r;
    int c;
    int height = scanArguments->height;
    int width = scanArguments->width;
    int row = scanArguments->row;
    int col = scanArguments->col;
    
    free(scanArguments);
    
    BoardScanData data;
    int index;
    
    for(r = row; r < row + height; r++) {
        for(c = col; c < col + width; c++) {
            
            index = r * BOARD_COLS + c;
            
            data.data = board[index];
            data.collision = (data.data & (ACTIVE_MASK | STATIC_MASK)) == (ACTIVE_MASK | STATIC_MASK);
            data.row = r;
            data.col = c;
          
            if (!gen_yield(handle, &data, NULL)) {
                return;
            }
        }
    }
}

GeneratorHandle scanBoard(int row, int col, int height, int width) {
    BoardScanArguments* scanArguments = (BoardScanArguments*)malloc(sizeof(BoardScanArguments));
    
    scanArguments->height = height;
    scanArguments->width = width;
    scanArguments->row = row;
    scanArguments->col = col;
    
    return gen_func(scanBoard_genFunc, void, BoardScanData, scanArguments);
}
