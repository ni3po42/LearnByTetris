/* 
    Defines interface for interacting with game board
*/

#ifndef __board_h_
#define __board_h_

#include <stdbool.h>

#include "../constants.h"
#include "../generator/generator.h"

// defines number of cells on board
#define BOARD_SIZE (BOARD_COLS*BOARD_ROWS)

/** 
 * Cell data for piece or board information. CellData is a bit field
 * 0wsa0ccc
 * w - wall bit. specifies if cell is a wall
 * s - static. specifies if cell is static (no longer moving)
 * a - active. specifies if cell is active (still moving). 
 * ccc - color code for cell. client defined color mapping
 * 0 - unused bits
 * if a and c are 1, than a collision has occurred
*/
typedef uint8_t CellData;

// defines board to be array of integers
typedef CellData Board[BOARD_SIZE];

// used as bit-field to track which rows are to be collapsed
typedef uint32_t collapsed_rows_t;

// arguments to pass board's scan function
typedef struct BoardScanArguments {
    int row;
    int height;
    int col;
    int width;
} BoardScanArguments;

// structure defining results of board's scan function
typedef struct BoardScanData {
    int row;
    int col;
    CellData data;
    bool collision;
    
} BoardScanData;

/*
    initializes the game board to be empty with walls/bottom
    @param stopIndex - boards clears from top to bottom, left to right. specifies cell index to stop clearing. Constant CONSTRUCT_WHOLE_BOARD clears all
*/
void constructBoard(int stopIndex);

/*
    clears specified rows from board and collapses remaining rows
    @param rows - bit field representing which rows to collapse
*/
void collapseBoard(collapsed_rows_t rows);

/*
    get cell data from game board
    @param row - integer between 0 and ROWS
    @param col - integer between 0 and COLS
    @return - masked data containing active/static states, is wall, and cell color
*/
CellData getBoardCell(int row, int col);

/*
    sets cell data for game board
    @param row - integer between 0 and ROWS
    @param col - ingeger between 0 and COLS
    @param data - masked data containing active/static states, is wall, and cell color
*/
void setBoardCell(int row, int col, CellData data);

/*
    applies an XOR operation with the board data at a row,col and data. new value saved to cell row,col
    @param row - integer between 0 and ROWS
    @param col - ingeger between 0 and COLS
    @param data - masked data containing active/static states, is wall, and cell color to be XORed
*/
void xorBoardCell(int row, int col, CellData data);

/*
    generating function that yields data within given bounds of board
    @param args - pointer to args to restrict scan
    @return GeneratorHandle - to be used with gen_next to obtain next BoardScanData. caller must free handle when done
*/
GeneratorHandle* scanBoard(BoardScanArguments* args);

/*
    include new row in given collapsed row mask
    @param collapsedRows - current mask to add row to
    @param row - row to add
    @return new collapsed_row_t with row added
*/
int addCollapsedRow(collapsed_rows_t collapsedRows, int row);

#endif