#include "output.hpp"

#ifdef _TETRIS_IO_CURSES
    #include "curses.output.cpp"
#else
    #error "Must include output service at linking. -D_TETRIS_IO_CURSES"
#endif

namespace tetris::io {

    std::unique_ptr<IOutputService> createOutputService() {

        #ifdef _TETRIS_IO_CURSES
            using tetris::io::curses::CursesOutputService;
            return std::move(std::make_unique<CursesOutputService>());
        #endif
    }

}