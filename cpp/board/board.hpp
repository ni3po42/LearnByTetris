#ifndef board_h_
#define board_h_

#include "../concepts.hpp"
#include <unistd.h>
#include <memory>
#include "../generator/generator.hpp"

namespace tetris::gameboard {

    using tetris::generator::Generator;

    typedef int CellData;

    constexpr int COLS = 12;
    constexpr int ROWS = 21;
    constexpr int SIZE = COLS * ROWS;

    constexpr CellData ACTIVE_MASK = 0x20;
    constexpr CellData STATIC_MASK = 0x40;
    constexpr CellData WALL_MASK = 0x80;
    constexpr CellData EMPTY_MASK = 0x00;
    constexpr CellData WALL_COLOR = 0x0F;
    constexpr CellData COLOR_MASK = 0x07;

    struct ScanData {
        int row;
        int col;
        CellData data;
        bool collision;    
    };

    typedef unsigned long CollapsedRows;

    CollapsedRows includeCollapsedRow(CollapsedRows existingCollapsedRows, int rowToCollapse);

    class Board {

        private:
            struct BoardImpl;
            std::shared_ptr<BoardImpl> impl;

        public:
            Board();
            
            void clear();
            void clear(int stopRow);
            void collapse(CollapsedRows rows);
            void setCell(int row, int col, CellData data);
            void xorCell(int row, int col, CellData data);

            Generator<ScanData> scan(int row, int col, int height, int width) const;
    };
}

#endif