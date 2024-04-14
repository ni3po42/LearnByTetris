#include <stdbool.h>
#include <stdio.h>

#include "../generator/generator.h"
#include "../constants.h"

#include "../utilities.h"

#include "piece.h"
#include "piece.internal.h"

/**
 * private apis
*/

// defines data for each piece
RawPieceData RawPieces[] = {
    {
        .field = {
            0,1,0,
            1,1,1,
            0,0,0
        },
        .height = 3,
        .width = 3,
        .ordinals = 4,
        .id = PIECE_T,
        .color = 1
    },
    {
        .field = {
            1,1,
            1,1
        },
        .height = 2,
        .width = 2,
        .ordinals = 1,
        .id = PIECE_O,
        .color = 2
    },
    {
        .field = {
            0,1,1,
            1,1,0,
            0,0,0
        },
        .height = 3,
        .width = 3,
        .ordinals = 2,
        .id = PIECE_S,
        .color = 3
    },
    {
        .field = {
            1,1,0,
            0,1,1,
            0,0,0
        },
        .height = 3,
        .width = 3,
        .ordinals = 2,
        .id = PIECE_Z,
        .color = 4
    },
    {
        .field = {
            0,0,0,
            1,1,1,
            0,0,1
        },
        .height = 3,
        .width = 3,
        .ordinals = 4,
        .id = PIECE_J,
        .color = 5
    },
    {
        .field = {
            0,0,0,
            1,1,1,
            1,0,0
        },
        .height = 3,
        .width = 3,
        .ordinals = 4,
        .id = PIECE_L,
        .color = 6
    },
    {
        .field = {
            0,0,0,0,
            1,1,1,1,
            0,0,0,0,
            0,0,0,0
        },
        .height = 4,
        .width = 4,
        .ordinals = 2,
        .id = PIECE_I,
        .color = 7
    }
};

// initializes a piece to be used on the game board
// static void initPiece(Piece* piece) {
   
// }

// builds piece based on raw piece data
static void constructPiece(Piece* piece, RawPieceData* data) {
    if (piece == NULL){
        return;
    }
    
    piece->data = data->field;
    piece->scanWidth = data->width;
    piece->scanHeight = data->height;
    piece->id = data->id;
    piece->color = data->color;
    
    piece->_ordinals = data->ordinals;
    piece->_direction = 0;
    piece->_active = true;
    
    piece->_row = 0;
    piece->_col = 0;
    piece->_prevRow = 0;
    piece->_prevCol = 0;
    piece->_prevDirection = 0;
}

// save state of piece
static void savePieceState(Piece* piece) {
    if (piece == NULL){
        return;
    }
    piece->_prevRow = piece->_row;
    piece->_prevCol = piece->_col;
    piece->_prevDirection = piece->_direction;    
}

// copies state of one piece to another
static void promotePiece(Piece* source, Piece* destination) {
    destination->color = source->color;
    destination->data = source->data;
    destination->id = source->id;
    destination->scanHeight = source->scanHeight;
    destination->scanWidth = source->scanWidth;
    destination->_active = source->_active;
    //destination->_col = source->_col;
    //destination->_row = source->_row;
    destination->_direction = source->_direction;
    destination->_ordinals = source->_ordinals;
    //destination->_prevCol = source->_prevCol;
    //destination->_prevDirection = source->_prevDirection;
    //destination->_prevRow = source->_prevRow;
    
    destination->_row = 0;
    destination->_col = (BOARD_COLS / 2) - 1;
    destination->_prevRow = 0;
    destination->_prevCol = (BOARD_COLS / 2) - 1;
    destination->_prevDirection = 0;
}

// generator function for creating infinite sequence of pieces
static void nextPiece_genFunc(GeneratorHandle* handle) {
    gen_restore(handle, Piece, currentPiece);
    gen_restore(handle, Piece, nextPiece);       
    gen_frame_init(handle, void, argument);
    
    int nextIndex = getRandomIntFromRange(0,6);              
    constructPiece(nextPiece, &RawPieces[nextIndex]);

    while(true) {
        promotePiece(nextPiece, currentPiece);
        // initPiece(currentPiece);
        
        nextIndex = getRandomIntFromRange(0,6);              
        constructPiece(nextPiece, &RawPieces[nextIndex]);
        // to tomorrow tim, since we are copying nextPieceData literally,
        // it means the pointers are copied, we think that by the time we
        // get the value, the data the pointers are pointing to have already
        // changed!
        NextPieceData nextPieceData = {
            .currentPiece = currentPiece,
            .nextPiece = nextPiece
        };
            
        gen_yield(handle, &nextPieceData);
    }    
}

/**
 * public apis
*/

void revertPiece(Piece* piece) {
    if (piece == NULL){
        return;
    }
    piece->_row = piece->_prevRow;
    piece->_col = piece->_prevCol;
    piece->_direction = piece->_prevDirection;    
}

void rotatePieceLeft(Piece* piece) {
    if (piece == NULL){
        return;
    }
    savePieceState(piece);
    piece->_direction = (--(piece->_direction) + piece->_ordinals) % piece->_ordinals;    
}

void rotatePieceRight(Piece* piece) {
    if (piece == NULL){
        return;
    }
    savePieceState(piece);
    piece->_direction = (++(piece->_direction)) % piece->_ordinals;
}

void dropPiece(Piece* piece) {
    if (piece == NULL){
        return;
    }
    savePieceState(piece);
    piece->_row++;
}

void leftPiece(Piece* piece) {
    if (piece == NULL){
        return;
    }
    savePieceState(piece);
    piece->_col -= 1;    
}

void rightPiece(Piece* piece) {
    if (piece == NULL){
        return;
    }
    savePieceState(piece);
    piece->_col += 1;     
}

void commitPiece(Piece* piece) {
    if (piece == NULL){
        return;
    }
    piece->_active = false;
}

int getPieceWidth(const Piece* piece) {
    if (piece == NULL){
        return -1;
    }
    return piece->_direction % 2 ? piece->scanHeight : piece->scanWidth;    
}

int getPieceHeight(const Piece* piece) {
    if (piece == NULL){
        return -1;
    }
    return piece->_direction % 2 ? piece->scanWidth : piece->scanHeight;
}

int getPieceRow(const Piece* piece) {
    if (piece == NULL){
        return -1;
    }
    return piece->_row;
}

int getPieceCol(const Piece* piece) {
    if (piece == NULL){
        return -1;
    }
    return piece->_col;
}


int getPieceId(const Piece* piece) {
    if (piece == NULL) {
        return -1;
    }
    return piece->id;
}

color_t getPieceColor(const Piece* piece) {
    if (piece == NULL) {
        return -1;
    }
    return piece->color;
}

void pieceScan(const Piece* piece, bool posNeutral, PieceScanFunc func) {
    
    int bound = piece->scanWidth * piece->scanHeight;
    int scanWidth = piece->scanWidth;
    int index;
    int r, c;
    color_t color;
    
    for(index = 0; index < bound; index++) {
        color = (piece->data)[index] ? piece->color : 0x00;
        
        if (color == 0){
            continue;
        }

        color |= piece->_active ? ACTIVE_MASK : STATIC_MASK;

        // based on piece's direction, actual row/col values can be different
        // this switch figures out when row/col values are swapped and account
        // for size of bounding box
        switch(piece->_direction) {
            case 0:
                r = index / scanWidth;
                c = index % scanWidth;
                break;
            case 1:
                r = index % scanWidth;
                c = piece->scanHeight - (index / scanWidth) - 1;
                break;
            case 2:
                r = (bound - index - 1) / scanWidth;
                c = (bound - index - 1) % scanWidth;                    
                break;
            case 3:
                r = scanWidth - (index % scanWidth) - 1;
                c = index / scanWidth;                    
                break;
        }

        func(
            r + (posNeutral ? 0 :piece->_row),
            c + (posNeutral ? 0 :piece->_col),
            color
        );
    }
    
}

GeneratorHandle* pieces() {
    return gen_func(nextPiece_genFunc, NextPieceData, NULL);
}