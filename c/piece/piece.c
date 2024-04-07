#include <stdbool.h>
#include <stdio.h>

#include "../generator/generator.h"
#include "../constants.h"

#include "../utilities.h"

#include "piece.h"
#include "piece.internal.h"


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
        .id = PIECE_T
    },
    {
        .field = {
            1,1,
            1,1
        },
        .height = 2,
        .width = 2,
        .ordinals = 1,
        .id = PIECE_O
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
        .id = PIECE_S
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
        .id = PIECE_Z
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
        .id = PIECE_J
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
        .id = PIECE_L
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
        .id = PIECE_I
    }
};

void initPiece(Piece* piece) {
    if (piece == NULL){
        return;
    }
    
    piece->_row = 0;
    piece->_col = (BOARD_COLS / 2) - 1;
    piece->_prevRow = 0;
    piece->_prevCol = (BOARD_COLS / 2) - 1;
    piece->_prevDirection = 0;
}

void constructPiece(Piece* piece, int* data, int scanHeight, int scanWidth, int ordinals, color_t color, int id) {
    if (piece == NULL){
        return;
    }
    
    piece->data = data;
    piece->scanWidth = scanWidth;
    piece->scanHeight = scanHeight;
    piece->id = id;
    piece->color = color;
    
    piece->_ordinals = ordinals;
    piece->_direction = 0;
    piece->_active = true;
    
    piece->_row = 0;
    piece->_col = 0;
    piece->_prevRow = 0;
    piece->_prevCol = 0;
    piece->_prevDirection = 0;
}



void revertPiece(Piece* piece) {
    if (piece == NULL){
        return;
    }
    piece->_row = piece->_prevRow;
    piece->_col = piece->_prevCol;
    piece->_direction = piece->_prevDirection;    
}

void savePieceState(Piece* piece) {
    if (piece == NULL){
        return;
    }
    piece->_prevRow = piece->_row;
    piece->_prevCol = piece->_col;
    piece->_prevDirection = piece->_direction;    
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

color_t getPieceColr(const Piece* piece) {
    if (piece == NULL) {
        return -1;
    }
    return piece->color;
}

int* getPieceData(const Piece* piece) {
    if (piece == NULL) {
        return NULL;
    }
    return piece->data;
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

static void copyPiece(Piece* source, Piece* destination) {
    destination->color = source->color;
    destination->data = source->data;
    destination->id = source->id;
    destination->scanHeight = source->scanHeight;
    destination->scanWidth = source->scanWidth;
    destination->_active = source->_active;
    destination->_col = source->_col;
    destination->_row = source->_row;
    destination->_direction = source->_direction;
    destination->_ordinals = source->_ordinals;
    destination->_prevCol = source->_prevCol;
    destination->_prevDirection = source->_prevDirection;
    destination->_prevRow = source->_prevRow;
}

static void nextPiece_genFunc(GeneratorHandle handle, void* argument) {
    
    bool currentPieceSet = false;
    NextPieceData nextPieceData;
    
    nextPieceData.currentPiece = (Piece*)malloc(sizeof(Piece));
    nextPieceData.nextPiece = (Piece*)malloc(sizeof(Piece));

    while(true) {
    
        int nextIndex = getRandomIntFromRange(0,6);
        
        RawPieceData* rawData = &RawPieces[nextIndex];
        color_t color = nextIndex + 1;
        
        constructPiece(nextPieceData.nextPiece, rawData->field, rawData->height, rawData->width, rawData->ordinals, color, rawData->id);
        
        if (currentPieceSet) {
            initPiece(nextPieceData.currentPiece);
            
            
            if(!gen_yield(handle, &nextPieceData, NULL)) {
                break;
            }
            
        } else {
            currentPieceSet = true;
        }
        
        copyPiece(nextPieceData.nextPiece, nextPieceData.currentPiece);
    }

    free(nextPieceData.currentPiece);
    free(nextPieceData.nextPiece);
    
}


GeneratorHandle pieces() {
    return gen_func(nextPiece_genFunc, void, NextPieceData, NULL);
}