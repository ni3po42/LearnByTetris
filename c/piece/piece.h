#ifndef __piece_h_
#define __piece_h_

#include "../generator/generator.h"
#include "../board/board.h"

typedef uint8_t color_t;

// opaque type to encapsulate data
typedef struct Piece Piece;

// data object from piece scanner
typedef struct PieceScanData {

    // row of piece cell
    int row;

    // col of piece cell
    int col;

    // color of cell for piece
    color_t color;
} PieceScanData;

// data object from pieces generator
typedef struct NextPieceData {
    // next piece to come from pieces generator
    Piece* nextPiece;

    // current piece to come from pieces generator
    Piece* currentPiece;
} NextPieceData;

/**
 * A function signature for handling piece scan data
 * @param row the row location of a cell that has color. May be absolute or relative depending on pieceScan invocation
 * @param col the col location of a cell that has color. May be absolute or relative depending on peiceScan invoaction
 * @param data CellData
 */
typedef void PieceScanFunc(int row, int col, CellData data);

/**
 * Reverts to piece's last saved state
 * @param piece piece state to revert
*/
void revertPiece(Piece* piece);

/** 
 * Rotates a piece counter clockwise
 * @param piece piece to rotate
*/
void rotatePieceLeft(Piece* piece);


/** 
 * Rotates a piece clockwise
 * @param piece piece to rotate
*/
void rotatePieceRight(Piece* piece);

/** 
 * Drop a piece 
 * @param piece piece to drop
*/
void dropPiece(Piece* piece);

/** 
 * Moves piece left
 * @param piece piece to move
*/
void leftPiece(Piece* piece);

/** 
 * Moves piece right
 * @param piece piece to move
*/
void rightPiece(Piece* piece);

/** 
 * commits piece data state. Controls if cell data is active or static
 * @param piece piece to commit
*/
void commitPiece(Piece* piece);

/** 
 * gets smallest width of piece. May vary on current rotation
 * @param piece pieces width to get
 * @return smallest width piece occupies completely in it's current rotation
*/
int getPieceWidth(const Piece* piece);

/** 
 * gets smallest height of piece. May vary on current rotation
 * @param piece pieces height to get
 * @return smallest height piece occupies completely in it's current rotation
*/
int getPieceHeight(const Piece* piece);

/** 
 * gets a row value for the piece. piece may not have a cell on this row, but guarentees no cell is on a row less then this value
 * @param piece pieces to get row value
 * @return a row value such that there is no cell on a row less than this value
*/
int getPieceRow(const Piece* piece);

/** 
 * gets a col value for the piece. piece may not have a cell on this col, but guarentees no cell is on a col less then this value
 * @param piece pieces to get col value
 * @return a col value such that there is no cell on a col less than this value
*/
int getPieceCol(const Piece* piece);

/**
 * unique identifier for the type of piece.
 * @param piece piece to get id for
 * @return one of the following constants: PIECE_T, PIECE_O, PIECE_S, PIECE_Z, PIECE_J, PIECE_L, PIECE_I
*/
int getPieceId(const Piece* piece);

/** 
 * returns a color code for the piece. value does not represent an rgb value, just a unique value a client can use to distiguish pieces
 * @param piece to get color code for
 * @return a 3 bit value for a color (8 colors in total)
*/
color_t getPieceColor(const Piece* piece);

/**
 * an action for handling piece scan data. will invoke func handler only when an actual cell has color is scanned
 * @param piece piece to scan
 * @param posNetural flag for specifing if row/col values are based on actual piece position (false) or use relative row/col values (true)
 * @param func function handler to be invoked when a cell is found in the piece that has color
*/
void pieceScan(const Piece* piece, bool posNeutral, PieceScanFunc func);

/**
 * handles generating a randome infinite sequence of pieces. 
 * @return a GeneratorHandle. When invoked with gen_next, a NextPieceData struct is expected to be yielded
*/
GeneratorHandle pieces();

#endif