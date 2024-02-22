#include "utility.hpp"
#include "input/input.hpp"
#include "output/output.hpp"
#include "game/game.hpp"

int main() {

    using tetris::game::Status;
    using tetris::io::IInputService;
    using tetris::io::IOutputService;

    tetris::utilities::initRand();

    auto outputService = tetris::io::createOutputService();
    auto inputService = tetris::io::createInputService();
	
    //scoped to ensure event goes out of scope before the input service
    {
        tetris::game::Level level = 10;
        auto interval = tetris::game::getInterval(level);
        
        tetris::gameboard::Board board;
        board.clear();
        
        tetris::events::EventQueue events;
        tetris::game::Game game(board);

        auto statusGenerator = game.getStatusGenerator(events, level);

        inputService->start(events, interval);

        while(
            statusGenerator()
                .and_then([&](Status status) -> std::optional<bool> {

                    if(level != status.level) {
                        level = status.level;
                        //interval = getInterval(level);
                        //resetDropInterval(interval);
                    }
                    
                    outputService->render(status, board);
                    
                    if (status.isGameOver) {                        
                        return std::nullopt;
                    } else {                        
                        return true;
                    }                
                })
        ){}
    }
    
    return 0;
}