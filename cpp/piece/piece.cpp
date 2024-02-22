#include "piece.hpp"
#include <map>

#include "../board/board.hpp"
#include "../utility.hpp"
#include <string>
#include <stdexcept>

namespace tetris::piece {

    using tetris::gameboard::COLS;
    using tetris::gameboard::STATIC_MASK;
    using tetris::gameboard::ACTIVE_MASK;

    using tetris::utilities::getRandomIntFromRange;

    struct PieceData {
        short field[16];
        int width;
        int height;
        int ordinals;
        Color color;
    };

    std::map<char, PieceData> rawPieces = {
        {'T', {
            {
                0,1,0,
                1,1,1,
                0,0,0
            }, 3, 3, 4, 1
        }},
        {'O', {
            {
                1,1,
                1,1
            }, 2, 2, 1, 2
        }},
        {'S', {
            {
                0,1,1,
                1,1,0,
                0,0,0
            }, 3, 3, 2, 3
        }},
        {'Z', {
            {
                1,1,0,
                0,1,1,
                0,0,0
            }, 3, 3, 2, 4
        }},
        {'J', {
            {
                0,0,0,
                1,1,1,
                0,0,1
            }, 3, 3, 4, 5
        }},
        {'L', {
            {
                0,0,0,
                1,1,1,
                1,0,0
            }, 3, 3, 4, 6
        }},
        {'I', {
            {
                0,0,0,0,
                1,1,1,1,
                0,0,0,0,
                0,0,0,0
            }, 4, 4, 2, 7
        }}
    };

    std::string idLookUp("TOSZJLI");

    struct Piece::PieceImpl {

        PieceImpl(char id) : 
            _id(id), turns(0), active(true), row(0), col(0), 
            prevCol(0), prevRow(0), prevTurns(0) {
                width = rawPieces[id].width;
                height = rawPieces[id].height;
                ordinals = rawPieces[id].ordinals;
                color = rawPieces[id].color;
        }

        PieceImpl() : 
            _id(' '), turns(0), active(false), row(0), col(0), 
            prevCol(0), prevRow(0), prevTurns(0), width(0), height(0),
            ordinals(1), color(0) {
        }
        
        void revert() {
            row = prevRow;
            col = prevCol;
            turns = prevTurns;
        }

        void init() {
            row = 0;
            col = (COLS / 2)  - 1;
            prevRow = 0;
            prevCol = col;
            turns = 0;
        }

        void rotate(Direction direction) {
            save();
            switch(direction) {
                case Direction::LEFT: 
                turns = (--(turns) + ordinals) % ordinals;
                break;
                
                case Direction::RIGHT:                
                turns = (++(turns)) % ordinals;
                break;
            }
        }

        void drop() {
            save();
            row++;
        }

        void move(Direction direction) {
            save();
            switch(direction) {
                case Direction::LEFT: col--; break;
                case Direction::RIGHT: col++; break;
            }
        }

        void commit() {
            active = false;
        }

        int getWidth() {
            return turns % 2 ? height : width;    
        }

        int getHeight() {
            return turns % 2 ? width : height;
        }

        int getRow() {
            return row;
        }

        int getCol() {
            return col;
        }

        int getId() {
            return _id;
        }

        Generator<ScanData> scan(bool useNeturalPosition) {
            int bound = width * height;
            int r, c;
            
            PieceData pieceData = rawPieces.at(_id);

            for(int index = 0; index < bound; index++) {
                
                if (pieceData.field[index] == 0) {
                    continue;
                }

                CellData cellData = color | (active ? ACTIVE_MASK : STATIC_MASK);

                switch(turns) {
                    case 0:
                        r = index / width;
                        c = index % width;
                        break;
                    case 1:
                        r = index % width;
                        c = height - (index / width) - 1;
                        break;
                    case 2:
                        r = (bound - index - 1) / width;
                        c = (bound - index - 1) % width;                    
                        break;
                    case 3:
                        r = width - (index % width) - 1;
                        c = index / width;                    
                        break;
                }

                co_yield ScanData {  
                    r + (useNeturalPosition ? 0 : row),
                    c + (useNeturalPosition ? 0 : col),
                    cellData
                };
            }
        } 

        private:
           
            int _id;
            bool active;
            
            int width;
            int height;
            int ordinals;
            Color color;

            int turns;
            int row;
            int col;
            
            int prevTurns;
            int prevRow;
            int prevCol;

            void save() {
                prevRow = row;
                prevCol = col;
                prevTurns = turns;
            }
            
    };

    Piece::Piece(char id) : impl{std::make_shared<Piece::PieceImpl>(id)} {}
    
    void Piece::revert() { impl->revert(); }
    void Piece::rotate(Direction direction) { impl->rotate(direction); }
    void Piece::drop() { impl->drop(); }
    void Piece::move(Direction direction) { impl->move(direction); }
    void Piece::commit() { impl->commit(); }
    int Piece::getWidth() const { return impl->getWidth(); }
    int Piece::getHeight() const { return impl->getHeight(); }
    int Piece::getRow() const { return impl->getRow(); }
    int Piece::getCol() const { return impl->getCol(); }
    char Piece::getId() const { return impl->getId(); }
    Piece& Piece::play() { impl->init(); return *this; }


    Generator<ScanData> Piece::scan(bool useNeturalPosition) const {
        return impl->scan(useNeturalPosition);
    }

    char getRandomPieceId() {
        int index = getRandomIntFromRange(0,6);
        return idLookUp.at(index);
    }

    Generator<std::tuple<Piece, char>> pieces() {
        
        char currentId = getRandomPieceId();

        while(true) {        
            char nextId = getRandomPieceId();
            co_yield std::tuple<Piece, char> { Piece(currentId).play(), nextId };            
            currentId = nextId;
        }
    }
}