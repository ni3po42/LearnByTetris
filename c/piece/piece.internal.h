#ifndef __piece_internal_h_
#define __piece_internal_h_


#include "piece.h"

#define PIECE_T 0
#define PIECE_O 1
#define PIECE_S 2
#define PIECE_Z 3
#define PIECE_J 4
#define PIECE_L 5
#define PIECE_I 6

struct Piece {
    int* data;
    int scanWidth;
    int scanHeight;
    
    color_t color;
    
    int id;
    
    int _ordinals;
    int _active;
    
    int _direction;
    int _row;
    int _col;
    
    int _prevDirection;
    int _prevRow;
    int _prevCol;

};

typedef struct PieceScanInput {
    Piece* piece;
    bool posNeutral;
} PieceScanInput;


typedef struct RawPieceData {
    int field[16];
    int width;
    int height;
    int ordinals;
    int id;
} RawPieceData;

#endif