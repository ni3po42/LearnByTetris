#include "board.hpp"
#include <iostream>

namespace tetris::gameboard {

    CollapsedRows includeCollapsedRow(CollapsedRows existingCollapsedRows, int rowToCollapse) {
        return (1 << rowToCollapse) | existingCollapsedRows;
    }

    struct Board::BoardImpl {

        CellData board[SIZE];

        void clear(int stopIndex) {
            
            for(int i = 0; i < SIZE && stopIndex != i; i++) {
                int c = i % COLS;
                int r = i / COLS;
                if (c == 0 || c == (COLS - 1) || r == (ROWS - 1)) {
                    board[i] = (STATIC_MASK | WALL_MASK | WALL_COLOR);
                } else {
                    board[i] = EMPTY_MASK;
                }
            }
        }

        void collapse(CollapsedRows rows) {

            int finger = SIZE;
            for(int i = SIZE-1; i>=0; i--) {
                int row = i / COLS;                
                int rowBit = (1 << row);
                
                if (!(rowBit & rows)) {
                    board[--finger] = board[i];
                }
            }
            clear(finger);            
        }

        void setCell(int row, int col, CellData data) {
            int index = row * COLS + col;
            board[index] = data; 
        }

        void xorCell(int row, int col, CellData data) {
            int index = row * COLS + col;
            board[index] ^= data;
        }

        Generator<ScanData> scan(int row, int col, int height, int width) const {
            
            for(int r = row; r < row + height; r++) {
                for(int c = col; c < col + width; c++) {
                    
                    int index = r * COLS + c;
                    
                    bool collision = (board[index] & (ACTIVE_MASK | STATIC_MASK)) == (ACTIVE_MASK | STATIC_MASK);
                    
                    co_yield ScanData { r, c, board[index], collision };
                }
            }
        }
    };

    Board::Board() : impl{std::make_shared<Board::BoardImpl>()} {}

    void Board::clear() { impl->clear(SIZE); }
    void Board::clear(int stopRow) { impl->clear(stopRow * COLS); }
    void Board::collapse(CollapsedRows rows) { impl->collapse(rows); }
    void Board::setCell(int row, int col, CellData data) { impl->setCell(row, col, data); }
    void Board::xorCell(int row, int col, CellData data) { impl->xorCell(row, col, data); }

    Generator<ScanData> Board::scan(int row, int col, int height, int width) const {
        return impl->scan(row, col, height, width);
    }
}