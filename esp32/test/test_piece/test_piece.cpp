#include "piece.internal.h"
#include "core.h"
#include <stdbool.h>
#include <unity.h>

void setUp(void) {

}

void tearDown(void) {

}

void test_can_get_next_piece(void) {
    
    initPieceSequence(0);
    
    promoteNextPiece(1);

    TEST_ASSERT_EQUAL(PIECE_T, getCurrentPiece()->id);
    TEST_ASSERT_EQUAL(PIECE_O, getNextPiece()->id);

    promoteNextPiece(2);

    TEST_ASSERT_EQUAL(PIECE_O, getCurrentPiece()->id);
    TEST_ASSERT_EQUAL(PIECE_S, getNextPiece()->id);

    promoteNextPiece(3);
    
    TEST_ASSERT_EQUAL(PIECE_S, getCurrentPiece()->id);
    TEST_ASSERT_EQUAL(PIECE_Z, getNextPiece()->id);

    promoteNextPiece(4);
    
    TEST_ASSERT_EQUAL(PIECE_Z, getCurrentPiece()->id);
    TEST_ASSERT_EQUAL(PIECE_J, getNextPiece()->id);

    promoteNextPiece(5);

    TEST_ASSERT_EQUAL(PIECE_J, getCurrentPiece()->id);
    TEST_ASSERT_EQUAL(PIECE_L, getNextPiece()->id);
        
    promoteNextPiece(6);
    
    TEST_ASSERT_EQUAL(PIECE_L, getCurrentPiece()->id);
    TEST_ASSERT_EQUAL(PIECE_I, getNextPiece()->id);

    promoteNextPiece(7);
    
    TEST_ASSERT_EQUAL(PIECE_I, getCurrentPiece()->id);
    TEST_ASSERT_EQUAL(PIECE_T, getNextPiece()->id);

}


void PieceScanFunc_T_test(int row, int col, CellData data) {
    
    int index = row * 3 + col;
    
    switch(index) {
        case 1:
        case 3:
        case 4:
        case 5:
            break;
        default:
            TEST_ABORT();
    }
    
    TEST_ASSERT_EQUAL(ACTIVE_MASK, data & ACTIVE_MASK);
}

void test_can_scan_piece_netural() {    
    
    initPieceSequence(0);
    promoteNextPiece(1);
    
    TEST_ASSERT_EQUAL(PIECE_T, getCurrentPiece()->id);
    
    pieceScan(getCurrentPiece(), true, PieceScanFunc_T_test);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_can_get_next_piece);        
    RUN_TEST(test_can_scan_piece_netural);
    UNITY_END();
}

void loop() {
    // do nothing
}


