#include "events.hpp"
#include <thread>
#include <condition_variable>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <future>

namespace tetris::events {

    class EventQueueException : public std::logic_error {
        public:
            EventQueueException(const char* message) : std::logic_error{message} {
            }
    };

    struct EventNode;
    struct EventNode {
        std::shared_ptr<EventNode> next;
        EventType type;
    };

    struct EventQueue::EventQueueImpl {

        std::mutex m_mutex;
        std::condition_variable m_condition;

        std::shared_ptr<EventNode> front;
        std::shared_ptr<EventNode> back;

        bool isDone = false;

        EventQueueImpl() { }

        void cleanUp() {
            while (front && front.unique()) {
                front = front->next;
            }
            
            //so either front holds null, or multiple things point to front's pointer
            if (!front) {
                back.reset();
            } else if (front == back && front.use_count() == 2) {
                //nothing else is pointing to this pointer except front and back,
                //so clear both
                back.reset();
                front.reset();
            }
        }

        void emit(EventMessage message) {
            
            if (isDone) {
                return;
            }

            //lock scope
            {
                std::scoped_lock<std::mutex> lock(m_mutex);
                std::shared_ptr<EventNode> newNode = std::make_shared<EventNode>();
                newNode->type = message.type;
                
                //enqueue with signals!
                if (!front) {
                    front = newNode;
                } else {
                    back->next = newNode;
                }

                back = newNode;
            }
            
            m_condition.notify_all();
        }

        static Generator<EventMessage> createGenerator(std::shared_ptr<EventQueue::EventQueueImpl> impl) { 

            std::shared_ptr<EventNode> finger;

            while(!impl->isDone) {

                for(finger = impl->front; finger; finger = finger->next) {
                    EventMessage message { finger->type };

                    if (impl->isDone) {
                        co_return std::move(message);
                        break;
                    } else {
                        co_yield std::move(message);
                    }
                }

                {
                    std::unique_lock<std::mutex> lock(impl->m_mutex);

                    if (impl->isDone) {
                        break;
                    }

                    impl->cleanUp();
                    impl->m_condition.wait(lock);  
                }

            }
        
        }
    
    };

    EventQueue::EventQueue() : impl{std::make_shared<EventQueue::EventQueueImpl>()}, root(true) {        
    }

    EventQueue::EventQueue(const EventQueue& queue) : impl{queue.impl}, root(false) {        
    }

    EventQueue::~EventQueue() {    
        if (root) {
            impl->isDone = true;
        }
    }

    bool EventQueue::isClosed() const {
        return impl->isDone;
    }

    void EventQueue::emit(EventMessage message) { impl->emit(message); }

    Generator<EventMessage> EventQueue::createGenerator() const {
        return EventQueue::EventQueueImpl::createGenerator(impl);
    }
}
