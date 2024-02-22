#ifndef input_hpp_
#define input_hpp_

#include "../events/events.hpp"
#include "../game/game.hpp"

namespace tetris::io {

    using tetris::game::Interval;
    using tetris::events::EventQueue;

    class IInputService {

        public:
            virtual ~IInputService() = default;
            virtual void start(const EventQueue& queue, Interval& interval) = 0;

    };

    std::unique_ptr<IInputService> createInputService();
}

#endif