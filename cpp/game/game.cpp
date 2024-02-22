#include "game.hpp"

#include <memory>

namespace tetris::game {

    using tetris::gameboard::COLS;
    using tetris::gameboard::ACTIVE_MASK;
    using tetris::gameboard::STATIC_MASK;
    using tetris::gameboard::WALL_MASK;
    using tetris::gameboard::ScanData;
    using tetris::piece::pieces;
    using tetris::piece::Direction;

    using tetris::events::EventType;

    int scoring[5] = {
        0,
        40,
        100,
        300,
        1200
    };

    static int minInt(int a, int b) {
        return a < b ? a : b;
    }

    static int maxInt(int a, int b) {
        return a < b ? b : a;
    }

    Interval getInterval(Level level) {
        int frames;
        if( level <= 8) {
            frames = (48 - level * 5);
        } else if (level == 9) {
            frames = 6;
        } else if (level <= 12) {
            frames = 5;
        } else if (level <= 15) {
            frames = 4;
        } else if (level <= 18) {
            frames = 3;
        } else if (level <= 28) {
            frames = 2;
        } else {
            frames = 1;
        }

        return (1000 * frames) / 6;
    }

    int countClearRows(CollapsedRows collapsedRows) {
        int count = 0;
        while(collapsedRows) {
            if ((1 & collapsedRows) == 1) {
                count++;
            }
            collapsedRows >>= 1;
        }
        return count;
    }

    struct Game::GameImpl {
        Board& board;

        void xorPiece(const Piece& piece) {
            auto scanner = piece.scan(false);
            while(auto scanData = scanner()) {
                auto [row, col, data] = scanData.value();
                board.xorCell(row, col, data);
            }
        }

        bool hasCollision(const Piece& piece) {
            
            auto gen = board.scan(piece.getRow(), piece.getCol(), piece.getHeight(), piece.getWidth());

            while(auto scanData = gen()) {
                if (scanData->collision) {
                    return true;
                }
            }
            
            return false;
        }

        CollapsedRows getClearRows(Piece& piece) {
            //pieces at most can span 4 rows;
            int rowCounts[4] = {0,0,0,0};
            int row = piece.getRow();

            auto gen = board.scan(row, 0, piece.getHeight(), COLS);
                        
            CollapsedRows clearedRows = 0;
            
            while(auto scanData = gen()) {
                int index = scanData->row - row;
                int val = (scanData->data & WALL_MASK) ? 0 : (scanData->data & STATIC_MASK);
                rowCounts[index] += (val > 0) ? 1 : 0;
            }

            for(int index = 0; index < 4; index++) {
                
                if (rowCounts[index] == COLS - 2) {
                    clearedRows |= ( 1 << (index + row));
                }                
            }
            
            return clearedRows;
        }

        Generator<Status> getStatusGenerator(const EventQueue& eventQueue, Level startLevel) {

            bool isGameOver = false;
            Level level = startLevel;
            Score score = 0;
            CollapsedRows collapsedRows = 0;
            
            auto event = eventQueue.createGenerator();

            int linesToClear = minInt(level * 10 + 10, maxInt(100, level * 10 - 50));
            int linesCleared = 0;
            bool wasDropped = false;
            int clearRowCount = 0;
            bool isDone = false;

            auto nextPieceGen = pieces();

            while(auto nextPieceOption = nextPieceGen()){
                
                auto nextPiece = nextPieceOption.value();
            
                //may need to implement a move constructor!!!
                auto currentPiece = std::get<0>(nextPiece);
                auto nextPieceId = std::get<1>(nextPiece);
                
                //draw
                xorPiece(currentPiece);                
                isGameOver = hasCollision(currentPiece);
                
                co_yield Status{ level, isGameOver, score, collapsedRows, nextPiece };

                while(auto eventDataOption = event()) {
                    
                    //clear
                    xorPiece(currentPiece); 

                    wasDropped = false;
                    switch(eventDataOption->type) {
                        case EventType::DROP:
                            currentPiece.drop();
                            wasDropped = true;
                            break;
                        case EventType::TURN_LEFT:
                            currentPiece.rotate(Direction::LEFT);
                            break;
                        case EventType::TURN_RIGHT:
                            currentPiece.rotate(Direction::RIGHT);
                            break;
                        case EventType::LEFT:
                            currentPiece.move(Direction::LEFT);
                            break;
                        case EventType::RIGHT:
                            currentPiece.move(Direction::RIGHT);
                            break;
                        case EventType::EXIT_GAME:
                            co_return Status{ level, isGameOver, score, collapsedRows, nextPiece };
                            break;
                    }

                    //draw new
                    xorPiece(currentPiece);

                    if (hasCollision(currentPiece)) {
                        //clear
                        xorPiece(currentPiece);

                        currentPiece.revert();

                        if (wasDropped) {
                            currentPiece.commit();
                        }

                        //draw
                        xorPiece(currentPiece);

                        if (wasDropped) {
                            break;
                        }
                    }

                    co_yield Status{ level, isGameOver, score, collapsedRows, nextPiece }; 
                }
                        
                collapsedRows = getClearRows(currentPiece);
                clearRowCount = countClearRows(collapsedRows);

                score += scoring[clearRowCount] * (level + 1);
                linesCleared += clearRowCount;

                int bound = linesToClear + ((level - startLevel) * 10);
                
                if (linesCleared >= bound) {
                    level++;
                    linesCleared -= bound;
                }
                
                co_yield Status{ level, isGameOver, score, collapsedRows, nextPiece }; 
                
                board.collapse(collapsedRows);
            }
        }

    };

    Game::Game(Board& board) : impl{std::make_shared<GameImpl>(board)} {        
    }

    Board& Game::getBoard() { return impl->board; }
    void Game::xorPiece(const Piece& piece) { impl->xorPiece(piece); }
    bool Game::hasCollision(const Piece& piece) { return impl->hasCollision(piece); }

    Generator<Status> Game::getStatusGenerator(const EventQueue& eventQueue, Level startLevel) {
        return impl->getStatusGenerator(eventQueue, startLevel);
    }

}