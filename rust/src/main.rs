use io::{Input, Output};

mod events;
mod common;
mod board;
mod piece;
mod game;
mod io;

fn main() {

    let (input, output) = io::create_io();

    let mut level: game::Level = 10;
    let board_ref = board::new_board();
    let interval = game::get_interval(level);

    let (event_sender, event_receiver) = events::create_event_channel();
    
    let (status_receiver, join) = game::start_game_loop(
        board_ref.clone(), 
        event_receiver, 
        level);

    input.start(event_sender, interval);

    while let Ok(status) = status_receiver.recv() {

        if let game::Status::ScoreUpdate(status_level, _, _) = status {
            if level != status_level {
                level  = status_level;
            }
        }

        output.render(&status, &board_ref);

        if let game::Status::GameEnd(is_game_over) = status {
            if is_game_over {
                std::thread::sleep(std::time::Duration::from_secs(2));
            }
            break;
        }
    }

    join.join().unwrap();
}
