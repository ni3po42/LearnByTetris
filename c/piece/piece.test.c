#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

#include "piece.internal.h"
#include "../generator/generator.h"
#include "../constants.h"
#include "../testing/utils.h"

//mocks
static int startingValue = 0;
int getRandomIntFromRange(int l, int u) {
    
    startingValue = startingValue % (u - l + 1);
    
    int returnVal = startingValue + l;
    startingValue++;
    
    return returnVal;
}

void TEST_can_get_next_piece() {
    
    startingValue = 0;
    
    GeneratorHandle handle = pieces();
    
    NextPieceData pieceData;
    
    gen_next(handle, NULL, &pieceData);
    
    assert(pieceData.currentPiece->id == PIECE_T);
    assert(pieceData.nextPiece->id == PIECE_O);
    
    gen_next(handle, NULL, &pieceData);
    
    assert(pieceData.currentPiece->id == PIECE_O);
    assert(pieceData.nextPiece->id == PIECE_S);
    
    gen_next(handle, NULL, &pieceData);
    
    assert(pieceData.currentPiece->id == PIECE_S);
    assert(pieceData.nextPiece->id == PIECE_Z);
    
    gen_next(handle, NULL, &pieceData);
    
    assert(pieceData.currentPiece->id == PIECE_Z);
    assert(pieceData.nextPiece->id == PIECE_J);
    
    gen_next(handle, NULL, &pieceData);
    
    assert(pieceData.currentPiece->id == PIECE_J);
    assert(pieceData.nextPiece->id == PIECE_L);
    
    gen_next(handle, NULL, &pieceData);
    
    assert(pieceData.currentPiece->id == PIECE_L);
    assert(pieceData.nextPiece->id == PIECE_I);
    
    gen_next(handle, NULL, &pieceData);
    
    assert(pieceData.currentPiece->id == PIECE_I);
    assert(pieceData.nextPiece->id == PIECE_T);
    
    freeGenerator(&handle);
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
            assert(("Unexpected col/row encountered", false));
    }
    
    assert(data & ACTIVE_MASK == ACTIVE_MASK);
    
}
void TEST_can_scan_piece_netural() {
    startingValue = 0;
    
    GeneratorHandle handle = pieces();
    
    NextPieceData pieceData;
    
    gen_next(handle, NULL, &pieceData);
    
    assert(pieceData.currentPiece->id == PIECE_T);
    
    pieceScan(pieceData.currentPiece, true, PieceScanFunc_T_test);
    
    freeGenerator(&handle);
}

int main(int argc, char *argv[]) 
{
    
    run(TEST_can_get_next_piece);
    
    run(TEST_can_scan_piece_netural);
    
    printf("\ndone\n");
}