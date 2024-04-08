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
    
    GeneratorHandle boardHandle = scanBoard(0,0,BOARD_ROWS, BOARD_COLS);
    BoardScanData scanData;
    
    while(gen_next(boardHandle, NULL, &scanData)) {
        
         if (scanData.col == 0) {
            fprintf(stderr, "\n");
        }
        
        //int bCell = boardBuffer[scanData.row * BOARD_COLS + scanData.col];
        if (scanData.data == 0) {
            fprintf(stderr, " ", scanData.data);    
        } else {
            fprintf(stderr, "X", scanData.data);    
        }
        
        
       
    }
    
    freeGenerator(&boardHandle);
}


int main(int argc, char *argv[]) 
{
    
    run(TEST_can_scan_board);
    
    printf("\ndone\n");
}