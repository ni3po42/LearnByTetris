/*
    Game board implementation
*/

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * private apis
*/

static Board board;

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

bool scanBoard(BoardScanArguments* args, BoardScanData* data) {
    
    if (args->start) {
        args->_current_col = args->col;
        args->_current_row = args->row;
        args->start = false;
    }

    if (args->_current_row >= args->row + args->height) {
        // out of range
        return false;
    }

    int index = args->_current_row * BOARD_COLS + args->_current_col;
            
    data->data = board[index];
    data->collision = (board[index] & (ACTIVE_MASK | STATIC_MASK)) == (ACTIVE_MASK | STATIC_MASK);
    data->row = args->_current_row;
    data->col = args->_current_col;

    args->_current_col++;

    if (args->_current_col >= args->col + args->width) {
        args->_current_col -= args->width;
        args->_current_row++;
    }

    return true;
}

#ifdef __cplusplus
}
#endif
