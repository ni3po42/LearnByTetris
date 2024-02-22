#ifndef events_h_
#define events_h_

#include <concepts>
#include <memory>
#include "../generator/generator.hpp"

namespace tetris::events {

    using tetris::generator::Generator;

    enum class EventType {
        NONE, DROP, LEFT, RIGHT, TURN_LEFT, TURN_RIGHT, PAUSE, EXIT_GAME
    };

    struct EventMessage {
        EventType type;
    };

    class EventQueue {
        
        private:
            struct EventQueueImpl;            
            std::shared_ptr<EventQueueImpl> impl;
            bool root;

        public:

            EventQueue();
            ~EventQueue();
            EventQueue(const EventQueue& eventQueue);
            EventQueue(EventQueue&& eventQueue) = delete;
            EventQueue& operator = (EventQueue&& eventQueue) = delete;

            void emit(EventMessage message);

            bool isClosed() const;
            Generator<EventMessage> createGenerator() const;

    };

};

#endif