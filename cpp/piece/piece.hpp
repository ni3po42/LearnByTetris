#ifndef piece_hpp_
#define piece_hpp_

#include <memory>
#include <tuple>
#include "../board/board.hpp"
#include "../generator/generator.hpp"

namespace tetris::piece {

    using tetris::gameboard::CellData;
    using tetris::generator::Generator;

    typedef int Color;

    enum class Direction {
        LEFT, RIGHT
    };

    struct ScanData {
        int row;
        int col;
        CellData data;
    };

    class Piece {

        private:
            struct PieceImpl;
            std::shared_ptr<PieceImpl> impl;

        public:
            Piece(char id);
            Piece() = delete;
            //Piece();

            Piece& play();
            void revert();
            void rotate(Direction direction);
            void drop();
            void move(Direction direction);
            void commit();

            int getWidth() const;
            int getHeight() const;
            int getRow() const;
            int getCol() const;
            char getId() const;

            Generator<ScanData> scan(bool useNeturalPosition) const;            
    };

    Generator<std::tuple<Piece, char>> pieces();
}

#endif