use crate::{board::BoardRef, events::EventSender, game::{Interval, Status}};

extern crate ncurses;

pub trait Input {
    fn start(&self, sender: EventSender, interval: Interval); //add interval...somehow
}

pub trait Output {
    fn clear(&self);
    fn render(&self, status: &Status, board_ref: &BoardRef);    
}

mod curses {
    use std::thread;
    use ncurses::*;

    use crate::board::{self, BoardExt, BoardRef, COLS, ROWS};
    use crate::common::{self, CellDataExt, CellInfo, EMPTY_CELL_DATA};
    use crate::events::{self, EventMessage};
    use crate::game::{self, Interval, IntervalExt};
    use crate::piece::{self, PieceId};
    use self::offsets::OffsetExt;
    use super::{Input, Output};

    mod offsets {
        pub type Offset = (i32, i32);

        pub trait OffsetExt {
            fn with_offset(&self, row: i32, col: i32) -> Offset;
        }

        impl OffsetExt for Offset {
            fn with_offset(&self, row: i32, col: i32) -> Offset {
                (self.0 + row, self.1 + col)
            }
        }

        pub const SCORE: Offset = (7,4);
        pub const LEVEL: Offset = (10, 4);
        pub const GAMEOVER: Offset = (10, 22);
        pub const BOARD: Offset = (1, 7);
        pub const NEXT_PIECE: Offset = (3,2);
    }
        
    pub struct CursesInput;

    pub struct CursesOutput;

    const Z: i32 = 'z' as i32;
    const C: i32 = 'c' as i32;

    impl Input for CursesInput {
        fn start(&self, sender: events::EventSender, interval: Interval) {            
            let sender_drop = sender.clone();
            thread::spawn(move||{
                raw();                
                keypad(stdscr(), true);

                loop {
                    let message: Option<EventMessage>;
                    let key = wgetch(stdscr()); //wget_wch(stdscr());

                    message = match key {
                        KEY_DOWN => 
                            Some(EventMessage { message_type: events::EventType::Drop}),
                        KEY_LEFT => 
                            Some(EventMessage { message_type: events::EventType::Slide(common::Direction::Left)}),
                        KEY_RIGHT => 
                            Some(EventMessage { message_type: events::EventType::Slide(common::Direction::Right)}),
                        Z => 
                            Some(EventMessage { message_type: events::EventType::Turn(common::Direction::Left)}),
                        C => 
                            Some(EventMessage { message_type: events::EventType::Turn(common::Direction::Right)}),
                        27 => 
                            Some(EventMessage { message_type: events::EventType::ExitGame}),
                        _ => None,
                    };
 
                    if message.is_some() {
                        if let Err(_) = sender.send(message) {
                            break;
                        }
                    }

                }
            });

            thread::spawn(move||{                
                loop {
                    thread::sleep(interval.to_duration());
                    let message = Some(EventMessage{
                        message_type: events::EventType::Drop
                    });

                    if let Err(_) = sender_drop.send(message) {                        
                        break
                    }
                }                
            });

        }
    }

    fn draw_text(message: &str, offset: offsets::Offset) {
        mvprintw(offset.0, offset.1, message);
    }

    fn draw_integer(val: u64, offset: offsets::Offset) {
        draw_text(val.to_string().as_str(), offset);
    }

    fn draw_cell(cell: impl common::CellInfo, offset: offsets::Offset) {

        let (row, col) = cell.position();
        let data = cell.data();
        let color = data.get_color();        
        let (row_offset, col_offset) = offset;

        if color == 0 {
            attr_on(COLOR_PAIR(8));
        } else {
            attr_on(COLOR_PAIR(color as i16));
            attr_on(A_REVERSE());
        }

        mvprintw(row as i32 + row_offset, 2*(col as i32 + col_offset), "  ");

        if color == 0 {
            attr_off(COLOR_PAIR(8));
        } else {
            attr_off(A_REVERSE());
            attr_off(COLOR_PAIR(color as i16));            
        }

    }

    fn draw_game_board(board_ref: &BoardRef) {
        if let Ok(board) = board_ref.lock() {
            for cell in (*board).scan(0, 0, ROWS, COLS) {
                draw_cell(cell, offsets::BOARD);
            }

            ncurses::mv(0, 27);
        }
    }

    impl CursesOutput {
     
        pub fn init(&self) {
            initscr();

            start_color();
            init_pair(1, COLOR_RED, COLOR_BLACK);
            init_pair(2, COLOR_GREEN, COLOR_BLACK);
            init_pair(3, COLOR_YELLOW, COLOR_BLACK);
            init_pair(4, COLOR_BLUE, COLOR_BLACK);
            init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
            init_pair(6, COLOR_CYAN, COLOR_BLACK);
            init_pair(7, COLOR_WHITE, COLOR_BLACK);
            init_pair(8, COLOR_BLACK, COLOR_BLACK);

            self.clear();
            draw_text("SCORE", offsets::SCORE);
            draw_text("LEVEL", offsets::LEVEL);
            refresh();            
        }
    
        fn render_next_piece(&self, old_next_piece_id: Option<PieceId>, new_next_piece_id: PieceId) {
            
            if let Some(old_id) = old_next_piece_id {
                let piece = piece::new_piece(old_id);
                for cell in piece.scan(true) {
                    draw_cell(cell.with_data(EMPTY_CELL_DATA), offsets::NEXT_PIECE);
                }
            }

            let piece = piece::new_piece(new_next_piece_id);
            for cell in piece.scan(true) {
                draw_cell(cell, offsets::NEXT_PIECE);
            }
        }

    
    }

    impl Output for CursesOutput {
        fn clear(&self) {
            mv(10, 20);
            ncurses::clear();
            refresh();
        }
    
        fn render(&self, status: &game::Status, board_ref: &board::BoardRef) {
            
            match status {
                game::Status::ScoreUpdate(level, score, _) => {
                    draw_integer(*level, offsets::LEVEL.with_offset(1, 1));
                    draw_integer(*score, offsets::SCORE.with_offset(1, 1)); 
                },
                game::Status::NextPiece(old_next_piece_id, new_next_piece_id) => self.render_next_piece(*old_next_piece_id,*new_next_piece_id),
                game::Status::GameEnd(true) => draw_text("GAME OVER", offsets::GAMEOVER),
                game::Status::GameEnd(false) => {},
                game::Status::BoardUpdate => draw_game_board(&board_ref),
            }
            ncurses::refresh();
        }
    }

    impl Drop for CursesOutput {
        fn drop(&mut self) {
            ncurses::endwin();
        }
    }
}

pub fn create_io() -> (impl Input, impl Output) {
    let input = curses::CursesInput{};
    
    let output = curses::CursesOutput{};
    output.init();

    (input, output)
}