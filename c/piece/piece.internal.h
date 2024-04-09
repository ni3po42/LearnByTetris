#ifndef __piece_internal_h_
#define __piece_internal_h_

#include "../constants.h"
#include "piece.h"

// encapsulated Piece data
struct Piece {

    // pointer to RawPieceData's field values
    int* data;

    // width of field data for piece
    int scanWidth;

    // height of field data for piece
    int scanHeight;
    
    // color code for piece
    color_t color;
    
    // piece id
    int id;
    
    // number of rotations piece can do
    int _ordinals;

    // flag for if piece is active or static
    int _active;
    
    // current rotation piece is in
    int _direction;

    // current row location
    int _row;

    // current col location
    int _col;
    
    // last saved direction
    int _prevDirection;

    // last saved row
    int _prevRow;

    // last saved col
    int _prevCol;

};

// defines a piece's definition
typedef struct RawPieceData {

    // array of 0/1, with 1 representing a cell with color. piece may not use all 16, but can at most have 16 cells
    int field[16];

    // width of 'box' for piece data
    int width;

    // height of 'box' for piece data
    int height;

    // number of rotations the piece has
    int ordinals;

    // unique id for piece
    int id;
} RawPieceData;

#endif