#include "input.hpp"

#include <thread>
#include <curses.h>
#include <ctime>

namespace tetris::io::curses {

    using tetris::game::Interval;
    using tetris::events::EventMessage;
    using tetris::events::EventType;

    void inputLoop(EventQueue queue) {
        
        raw();				/* Line buffering disabled	*/
	    keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/

        EventMessage message = { .type = EventType::NONE };
        while(!queue.isClosed())
        {	
            int c = getch();
            switch(c)
            {
                case KEY_DOWN:
                    message.type = EventType::DROP;
                    break;
                case KEY_LEFT:
                    message.type = EventType::LEFT;
                    break;
                case KEY_RIGHT:
                    message.type = EventType::RIGHT;
                    break;
                case 'z':
                    message.type = EventType::TURN_LEFT;
                    break;
                case 'c':
                    message.type = EventType::TURN_RIGHT;
                    break;
                case 27:
                    message.type = EventType::EXIT_GAME;
                    break;
                default:
                    message.type = EventType::NONE;
            }
            
            if (message.type != EventType::NONE) {
                queue.emit(message);
            }
        }
    }

    void dropLoop(EventQueue queue, Interval& interval) {

        struct timespec ts;
        
        EventMessage message = { .type = EventType::DROP };
        
        while(!queue.isClosed()) {
        
            ts.tv_sec = interval / 1000;
            ts.tv_nsec = (interval % 1000) * 1000000;
        
            nanosleep(&ts, &ts);
            
            queue.emit(message);
        }
    }

    class CursesInputService : public IInputService {

        private:
            std::jthread inputThread;
            std::jthread dropThread;
        public:

            virtual ~CursesInputService() {
            }

            virtual void start(const EventQueue& queue, Interval& interval) override {
                inputThread = std::jthread(inputLoop, std::ref(queue));
                dropThread = std::jthread(dropLoop, std::ref(queue), std::ref(interval));
            }
    };
}
