#ifndef __board_h_
#define __board_h_

#include <stdbool.h>

#include "../constants.h"
#include "../generator/generator.h"

#define BOARD_SIZE (BOARD_COLS*BOARD_ROWS)
#define WALL_COLOR 0x0F

typedef int Board[BOARD_SIZE];

typedef uint64_t collapsed_rows_mask_t;

typedef struct BoardScanArguments {
    int row;
    int height;
    int col;
    int width;
} BoardScanArguments;

typedef struct BoardScanData {
    int row;
    int col;
    int data;
    bool collision;
    
} BoardScanData;

void constructBoard(int stopIndex);
void collapseBoard(collapsed_rows_mask_t rows);
int getBoardCell(int row, int col);
void setBoardCell(int row, int col, int data);
void xorBoardCell(int row, int col, int data);

GeneratorHandle scanBoard(int row, int col, int height, int width);

int addCollapseRowMask(collapsed_rows_mask_t rowMask, int row);



#endif