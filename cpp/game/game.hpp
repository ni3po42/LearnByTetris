#ifndef game_hpp_
#define game_hpp_

#include <memory>
#include <tuple>
#include "../piece/piece.hpp"
#include "../generator/generator.hpp"
#include "../events/events.hpp"
#include "../board/board.hpp"

namespace tetris::game {

    using tetris::piece::Piece;
    using tetris::gameboard::Board;
    using tetris::generator::Generator;
    using tetris::events::EventQueue;
    using tetris::gameboard::CollapsedRows;

    typedef int Interval;
    typedef int Level;
    typedef int Score;

    struct Status {
        Level level;
        bool isGameOver;
        Score score;
        CollapsedRows collapsedRows;
        std::tuple<Piece, char> pieceInfo;
    };

    Interval getInterval(Level level);

    class Game {

        private:
            struct GameImpl;
            std::shared_ptr<GameImpl> impl;

        public:
            Game(Board& board);
            Board& getBoard();
            void xorPiece(const Piece& piece);
            bool hasCollision(const Piece& piece);

            Generator<Status> getStatusGenerator(const EventQueue& eventQueue, Level startLevel);

    };

}

#endif