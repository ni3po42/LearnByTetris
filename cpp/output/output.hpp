#ifndef output_hpp_
#define output_hpp_

#include "../game/game.hpp"
#include "../board/board.hpp"

#include <string>
#include <memory>

namespace tetris::io {

    using tetris::game::Status;
    using tetris::gameboard::Board;

    class IOutputService {

        public:
            virtual ~IOutputService() = default;

            virtual void clear() = 0;
            virtual void render(Status status, const Board& board) = 0;
            virtual void debug(const std::string& message) = 0;
    };

    std::unique_ptr<IOutputService> createOutputService();
}

#endif