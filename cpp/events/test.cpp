#include "events.hpp"
#include "../test/utils.hpp"

using tetris::events::EventQueue;
using tetris::events::EventMessage;
using tetris::events::EventType;
using tetris::generator::Generator;


void TEST_can_create_event_queue() {

    EventQueue queue;

}

void TEST_can_create_generator() {

    EventQueue queue;

    auto gen = queue.createGenerator();

    queue.emit( EventMessage { EventType::RIGHT } );
    queue.emit( EventMessage { EventType::LEFT } );
    queue.emit( EventMessage { EventType::PAUSE } );

    Expect(int(EventType::RIGHT), int(gen()->type), "EventType isn't Right");
    Expect(int(EventType::LEFT), int(gen()->type), "EventType isn't left");
    Expect(int(EventType::PAUSE), int(gen()->type), "EventType isn't pause");

}

void TEST_can_destory_generator_on_drop() {

    EventQueue *queue = new EventQueue;

    auto gen = queue->createGenerator();

    queue->emit( EventMessage { EventType::RIGHT } );

    bool typesEqual = EventType::RIGHT == gen()->type;

    Expect(true, typesEqual, "EventType isn't Right");

    delete queue;
    
    auto t = gen();
    Expect(false, bool(t), "should have no value");
    Expect(true, t == std::nullopt, "should be null option");  

}

int main() {

    run(TEST_can_create_event_queue);
    run(TEST_can_create_generator);
    run(TEST_can_destory_generator_on_drop);


    return 0;
}