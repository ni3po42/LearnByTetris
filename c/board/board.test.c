#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

#include "board.h"
#include "../generator/generator.h"
#include "../constants.h"
#include "../testing/utils.h"



void TEST_can_scan_board() {
    
    constructBoard(CONSTRUCT_WHOLE_BOARD);
    
    BoardScanArguments args = {
        .col = 0, .row = 0, .width = BOARD_COLS, .height = BOARD_ROWS
    };
    GeneratorHandle* boardHandle = scanBoard(&args);
    BoardScanData scanData;
    
    while(gen_next(boardHandle, &scanData)) {
        
        if (scanData.col == 0) {
            fprintf(stderr, "\n");
        }
        
        //int bCell = boardBuffer[scanData.row * BOARD_COLS + scanData.col];
        if (scanData.data == 0) {
            fprintf(stderr, " ");    
        } else {
            fprintf(stderr, "X");    
        }
        
    }
    
    freeGenerator(&boardHandle);
}


int main(int argc, char *argv[]) 
{
    
    run(TEST_can_scan_board);
    
    printf("\ndone\n");
}