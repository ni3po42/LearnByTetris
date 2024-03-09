use std::cmp::{max, min};
use std::collections::HashMap;
use std::sync::mpsc::sync_channel;
use std::thread::{self, JoinHandle};
use std::time::Duration;

use crate::common::{CellDataExt, CellInfo, CollapsedRows, CollapsedRowsExt};
use crate::events::{self, EventReceiver};
use crate::piece::{self, Piece, PieceId};
use crate::common::Rectangle;
use crate::board::{Board, BoardExt, BoardRef, COLS};
use crate::game;

pub type Interval = u64;
pub type Level = u64;
pub type Score = u64;

pub enum Status {
    ScoreUpdate(Level, Score, CollapsedRows),
    NextPiece(Option<PieceId>, PieceId), // old next, new next
    GameEnd(bool),
    BoardUpdate
}

impl std::fmt::Display for Status {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Status::ScoreUpdate(level, score, rows) => write!(f, "ScoreUpdate: level {} score {} rows {}", level, score, rows),
            Status::NextPiece(current_id, next_id) => write!(f, "OldNext: id {:?}, NewNext: id {}", current_id, next_id),
            Status::GameEnd(is_game_over) => write!(f, "GameEnd: gameover {}", is_game_over),
            Status::BoardUpdate => write!(f, "BoardUpdate"),
        }        
    }
}

pub type StatusReceiver = std::sync::mpsc::Receiver<Status>;
pub type StatusSender = std::sync::mpsc::SyncSender<Status>;

fn create_status_channel() -> (StatusSender, StatusReceiver) {    
    sync_channel(0)
}

pub trait IntervalExt {
    fn to_duration(&self) -> Duration;
}

impl IntervalExt for Interval {
    fn to_duration(&self) -> Duration {
        Duration::from_millis(*self as u64)
    }
}

const SCORING: [Score; 5] = [0, 40, 100, 300, 1200];

pub fn get_interval(level: Level) -> Interval {

    let raw_interval: Interval;
    match level {
        i if i <= 8 => raw_interval = 48 - 5 * i,
        i if i == 9 => raw_interval = 6,
        i if i <= 12 => raw_interval = 5,
        i if i <= 15 => raw_interval = 4,
        i if i <= 18 => raw_interval = 3,
        i if i <= 28 => raw_interval = 2,
        _ => raw_interval = 1
    }

    return (1000 * raw_interval) / 6;
}

fn xor_piece(board: &mut Board, piece: &Piece) {
    for cell in piece.scan(false) {
        board.xor(&cell);
    }
}

fn has_collision(board: &Board, piece: &Piece) -> bool {
    
    let (row, col) = piece.position();
    let (height, width) = piece.dimensions();

    board
        .scan(row, col, height, width)
        .any(|cell|{ cell.has_collision() })

}

fn get_collapsing_rows(board: &Board, piece: &Piece) -> CollapsedRows {

    let (row, _) = piece.position();
    let (height, _) = piece.dimensions();

    let mut hash_map: HashMap<i32, u8> = std::collections::HashMap::new();
    let mut collapsing_rows: CollapsedRows = 0;

    for cell in board.scan(row, 1, height, COLS - 2) {

        let (cell_row, _) = cell.position();
        let data = cell.data();

        if data.is_static() {
            if hash_map.contains_key(&cell_row) {                
                hash_map.entry(cell_row).and_modify(|v|{ *v += 1; });
            } else {
                hash_map.insert(cell_row, 1);
            }            
        }

    }
    
    for (row, val) in hash_map.iter() {
        
        if *val as usize == COLS - 2 {
            collapsing_rows.add_row(*row as usize);
        }
    }

    collapsing_rows

}

pub fn start_game_loop(board_ref: BoardRef, event_receiver: EventReceiver, start_level: Level) -> (StatusReceiver, JoinHandle<()>) {

    let (sender, receiver) = create_status_channel();

    let thread_handle = thread::spawn(move ||{
        game_loop(board_ref, event_receiver, sender, start_level);
    });

    (receiver, thread_handle)
}

fn calc_lines_to_clear(level: Level) -> u64 {
    if level < 5 {
        min(level * 10 + 10, 100)
    } else {
        min(level * 10 + 10, max(100, level * 10 - 50))
    }    
}

fn game_loop(board_ref: BoardRef, event_receiver: EventReceiver, status_sender: StatusSender, start_level: Level) {

    let mut is_game_over = false;
    let mut level: Level = start_level;
    let mut score: Score = 0;
    
    let mut lines_to_clear = calc_lines_to_clear(level);
    let mut lines_cleared = 0;
    let mut was_dropped: bool;

    if let Err(_) = status_sender.send(Status::ScoreUpdate(level, score, 0)) {
        return;
    }

    let mut old_next_id: Option<PieceId> = None;

    for (mut current_piece, next_piece_id) in piece::piece_generator() {
        let mut collapsed_rows: CollapsedRows = 0;

        if let Err(_) = status_sender.send(Status::NextPiece(old_next_id, next_piece_id)) {
            return;
        }

        old_next_id = Some(next_piece_id);

        if let Ok(mut board) = board_ref.lock() {
            //draw
            game::xor_piece(&mut *board, &current_piece);
            is_game_over = game::has_collision(& *board, &current_piece);
        }
          
        if is_game_over {
            _ = status_sender.send(Status::GameEnd(true));
            break;
        }

        if let Err(_) = status_sender.send(Status::BoardUpdate) {
            return;
        }
        
        while let Ok(Some(message)) = event_receiver.recv() {
            
            if let Ok(mut board) = board_ref.lock() {

                //clear   
                game::xor_piece(&mut board, &current_piece);

                was_dropped = false;

                match message.message_type {
                    events::EventType::Drop => {
                        current_piece.drop();
                        was_dropped = true;
                    },
                    events::EventType::Slide(direction) => current_piece.slide(direction),
                    events::EventType::Turn(direction) => current_piece.rotate(direction),
                    events::EventType::ExitGame => {
                        _ = status_sender.send(Status::GameEnd(true));
                        return;
                    }
                }
    
                //draw
                game::xor_piece(&mut board, &current_piece);
    
                if game::has_collision(&board, &current_piece) {
                    //clear
                    game::xor_piece(&mut board, &current_piece);
                    current_piece.revert();
                    if was_dropped {
                        current_piece.commit();
                    }
                    //draw
                    game::xor_piece(&mut board, &current_piece);
                    if was_dropped {
                        collapsed_rows = game::get_collapsing_rows(&board, &current_piece);
                        board.collapse(collapsed_rows);
                        break;
                    }
                }

            }
            
            if let Err(_) = status_sender.send(Status::BoardUpdate) {
                return;
            }
        }

        let cleared_row_count = collapsed_rows.count_ones();

        score += SCORING[cleared_row_count as usize] + (level + 1);
        lines_cleared += cleared_row_count;
        let bound = (lines_to_clear + ((level - start_level) * 10)) as u32;

        if lines_cleared >= bound {
            level +=1;
            lines_cleared -= bound;
            lines_to_clear = calc_lines_to_clear(level);
        }

        if let Err(_) = status_sender.send(Status::ScoreUpdate(level, score, collapsed_rows)) {
            return;            
        }        
    }

}

//z, 6th left panics

#[cfg(test)]
mod tests {
    use crate::{board, piece};


    #[test]
    fn can_press_turn_left_then_left_6_times_with_i_piece() {

        let board_ref = board::new_board();
        let board_guard = board_ref.lock().unwrap();
        let mut board = *board_guard;

        let mut i_piece = piece::new_piece('I');
        i_piece.play();
        println!("start");
        
        i_piece.rotate(crate::common::Direction::Left);
        crate::game::xor_piece(&mut board, &i_piece);//draw
        _ = crate::game::has_collision(&board, &i_piece);

        for _ in 0..6 {
            crate::game::xor_piece(&mut board, &i_piece);//clear
            i_piece.slide(crate::common::Direction::Left);
            crate::game::xor_piece(&mut board, &i_piece);//draw
            let hc = crate::game::has_collision(&board, &i_piece);

            println!("has collision: {}", hc);
        }
        
               
    }
}

