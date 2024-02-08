#ifndef __piece_h_
#define __piece_h_

#include "../generator/generator.h"

#define PIECE_T 0
#define PIECE_O 1
#define PIECE_S 2
#define PIECE_Z 3
#define PIECE_J 4
#define PIECE_L 5
#define PIECE_I 6

typedef int color_t;

typedef struct Piece {
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

} Piece;

typedef struct PieceScanInput {
    Piece* piece;
    bool posNeutral;
} PieceScanInput;

typedef struct PieceScanData {
    int row;
    int col;
    color_t color;
} PieceScanData;

typedef struct NextPieceData {
    Piece nextPiece;
    Piece currentPiece;
} NextPieceData;

typedef struct RawPieceData {
    int field[16];
    int width;
    int height;
    int ordinals;
    int id;
} RawPieceData;

void constructPiece(Piece* piece, int* data, int scanHeight, int scanWidth, int ordinals, color_t color, int id);
void initPiece(Piece* piece);
void revertPiece(Piece* piece);
void savePieceState(Piece* piece);
void rotatePieceLeft(Piece* piece);
void rotatePieceRight(Piece* piece);
void dropPiece(Piece* piece);
void leftPiece(Piece* piece);
void rightPiece(Piece* piece);
void commitPiece(Piece* piece);

int getPieceWidth(const Piece* piece);
int getPieceHeight(const Piece* piece);
int getPieceRow(const Piece* piece);
int getPieceCol(const Piece* piece);

typedef void PieceScanFunc(int row, int col, int data);
void pieceScan(Piece piece, bool posNeutral, PieceScanFunc func);


GeneratorHandle pieces();

#endif