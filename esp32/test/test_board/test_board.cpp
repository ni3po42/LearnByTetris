#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "board.h"
#include "core.h"
#include <unity.h>

void test_can_scan_board() {
    
    constructBoard(CONSTRUCT_WHOLE_BOARD);
    
    BoardScanArguments args = {
        .row = 0, .height = BOARD_ROWS, .col = 0, .width = BOARD_COLS, .start = true, ._current_col = 0, ._current_row = 0
    };
    
    BoardScanData scanData;
    int cellCount = 0;
    while(scanBoard(&args, &scanData)) {        
        cellCount++;
    }    

    TEST_ASSERT_EQUAL(BOARD_SIZE, cellCount);
}


void setup()
{    
    UNITY_BEGIN();
    RUN_TEST(test_can_scan_board);
    UNITY_END();
}

void loop() 
{
    
}