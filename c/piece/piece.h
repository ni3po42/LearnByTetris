#ifndef __piece_h_
#define __piece_h_

#include "../generator/generator.h"

typedef int color_t;

// opaque type to encapsulate data
typedef struct Piece Piece;

typedef struct PieceScanData {
    int row;
    int col;
    color_t color;
} PieceScanData;

typedef struct NextPieceData {
    Piece* nextPiece;
    Piece* currentPiece;
} NextPieceData;

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
int getPieceId(const Piece* piece);
color_t getPieceColr(const Piece* piece);
int* getPieceData(const Piece* piece);

typedef void PieceScanFunc(int row, int col, int data);
void pieceScan(const Piece* piece, bool posNeutral, PieceScanFunc func);

GeneratorHandle pieces();

#endif