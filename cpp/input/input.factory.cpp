#include "input.hpp"

#ifdef _TETRIS_IO_CURSES
    #include "curses.input.cpp"
#else
    #error "Must include input service at linking. -D_TETRIS_IO_CURSES"
#endif

namespace tetris::io {

    std::unique_ptr<IInputService> createInputService() {

        #ifdef _TETRIS_IO_CURSES
            using tetris::io::curses::CursesInputService;
            return std::move(std::make_unique<CursesInputService>());
        #endif
    }

}