/*
    Game board implementation
*/

#include "board.h"
#include <stdbool.h>
#include <stdio.h>

#include "../generator/generator.h"

/**
 * private apis
*/

static Board board;

// internal callback to handle generator function
static void scanBoard_genFunc(GeneratorHandle* handle) {
    gen_restore(handle, int, r);
    gen_restore(handle, int, c);
    gen_restore(handle, int, height);
    gen_restore(handle, int, width);
    gen_restore(handle, int, row);
    gen_restore(handle, int, col);
    gen_frame_init(handle, BoardScanArguments, scanArguments);

    *height = scanArguments->height;
    *width = scanArguments->width;
    *row = scanArguments->row;
    *col = scanArguments->col;
        
    BoardScanData data;
    int index;
    
    for(*r = *row; *r < *row + *height; (*r)++) {
        for(*c = *col; *c < *col + *width; (*c)++) {

            index = *r * BOARD_COLS + *c;
            
            data.data = board[index];
            data.collision = (data.data & (ACTIVE_MASK | STATIC_MASK)) == (ACTIVE_MASK | STATIC_MASK);
            data.row = *r;
            data.col = *c;
          
            gen_yield(handle, &data);
        }
    }
}

/**
 * public apis
*/

void constructBoard(int stopIndex) {
    
    int i;
    int c;
    int r;
    
    // starting top left of board and scaning right then down
    for(i = 0; i < BOARD_SIZE; i++) {

        // when index is the stop index, stop so everything right and below
        // remains unchanged
        if (stopIndex == i) {
            break;
        }
        c = i % BOARD_COLS;
        r = i / BOARD_COLS;

        // set cell as wall for left,right,bottom bounds
        if (c == 0 || c == (BOARD_COLS - 1) || r == (BOARD_ROWS - 1)) {
            board[i] = (STATIC_MASK | WALL_MASK | WALL_COLOR);
        } else {
            board[i] = 0x00;
        }
    }
}

int addCollapsedRow(collapsed_rows_t rowMask, int row) {
    return (1 << row) | rowMask;
}

void collapseBoard(collapsed_rows_t rowMask) {
    
    int i;
    // point finger to last cell index plus one
    int finger = BOARD_SIZE;
    int row;
    
    collapsed_rows_t currentRow;
    
    // start bottom right and work left and up
    for(i=BOARD_SIZE-1;i>=0;i--) {
        // get the row being processed
        row = i / BOARD_COLS;
        
        currentRow = (1 << row);
        
        // if the current row is not a collapsed row
        if (!(currentRow & rowMask)) {
            //move finger to previous index
            //set data in board at finger index to the value 
            board[--finger] = board[i];
        }
        // when current row is a collapsed row, i will continue to decrement,
        // but finger stays the same, this effectively skips the row from being copied
        // until a non collapsed row is encountered and resume copying from where finger
        // left off
    }
    constructBoard(finger);
}

CellData getBoardCell(int row, int col) {
    return board[row * BOARD_COLS + col];
}

void setBoardCell(int row, int col, CellData data) {
    int index = row * BOARD_COLS + col;
    board[index] = data; 
}

void xorBoardCell(int row, int col, CellData data) {
    int index = row * BOARD_COLS + col;
    board[index] ^= data;
}

GeneratorHandle* scanBoard(BoardScanArguments* args) {    
    return gen_func(scanBoard_genFunc, BoardScanData, args);
}
