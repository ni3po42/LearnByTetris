use std::sync::Mutex;

use crate::common::{self, CellDataExt};

pub const COLS: usize = 12;
pub const ROWS: usize = 21;
pub const SIZE: usize = COLS * ROWS;

pub type Board = [common::CellData; SIZE];

pub type BoardRef = std::sync::Arc<std::sync::Mutex<Board>>;

pub fn new_board() -> BoardRef {
    let mut b: Board = [0;SIZE];
    b.clear();
    std::sync::Arc::new(Mutex::new(b))
}

pub struct BoardScanIterator<'b> {
    col: i32,
    height: usize,
    width: usize,
    it_r: i32,
    it_c: i32,
    board: &'b Board
}

pub struct ScanData {
    row: i32,
    col: i32,
    data: common::CellData
}

impl common::CellInfo for ScanData {
    fn position(&self) -> (i32, i32) {
        (self.row, self.col)
    }

    fn data(&self) -> common::CellData {
        self.data
    }

    fn has_collision(&self) -> bool {
        self.data.is_invalid()
    }

    fn with_data(&self, data: common::CellData) -> impl common::CellInfo {
        ScanData { row: self.row, col: self.col, data }
    }
}

impl<'b> Iterator for BoardScanIterator<'b> {
    type Item = ScanData;

    fn next(&mut self) -> Option<Self::Item> {
        
        if self.width <= 0 || self.height <= 0 {
			return None
		}

		let index = (self.it_r*(COLS as i32) + self.it_c) as usize;

        if index >= SIZE {
            return None;
        }

		let scan_data = ScanData{
            row: self.it_r, 
            col: self.it_c, 
            data: self.board[index]
        };

		self.it_c += 1;

		if self.it_c - self.col == self.width as i32 {
			self.it_c = self.col;
			self.it_r += 1;

			if self.it_r == ROWS as i32 {
				self.height = 0;
			} else {
				self.height -= 1;
			}
		}

		return Some(scan_data);

    }
}

pub trait BoardExt {
    fn clear(&mut self);
    fn collapse(&mut self, rows: common::CollapsedRows);
    fn set(&mut self, cell: &impl common::CellInfo);    
    fn xor(&mut self, cell: &impl common::CellInfo);
    fn scan(&self, row: i32, col: i32, height: usize, width: usize) -> BoardScanIterator;
}

fn clear_range(board: &mut Board, start_index: usize, stop_index: usize) {
        
    for i in start_index..stop_index {
        
        let c = i % COLS;
        let r = i / COLS;

        if c == 0 || c == (COLS-1) || r == (ROWS-1) {
            board[i] = common::EMPTY_CELL_DATA
                        .set_active(false)
                        .set_static(true)
                        .set_color(common::WALL_COLOR);
        } else {
            board[i] = common::EMPTY_CELL_DATA;
        }

    }
}

impl BoardExt for Board {

    fn clear(&mut self) {
        clear_range(self, 0, SIZE);
    }

    fn collapse(&mut self, rows: common::CollapsedRows) {
        let top_row: common::CollapsedRows = 1;
        let no_row: common::CollapsedRows = 0;
    
        if rows == 0 {
            return
        }
    
        let mut finger = SIZE - COLS;
        let mut i = SIZE - COLS - 1;
        loop {

            let row_number = i / COLS;
            let collaspse_state = top_row << row_number;
    
            if (collaspse_state & rows) == no_row {
                finger -= 1;
                self[finger] = self[i];
            }

            if i == 0 {
                break;
            }

            i-=1;            
        }
           
        clear_range(self, 0, finger);
    }


    fn set(&mut self, cell: &impl common::CellInfo) {
        let (row, col) = cell.position();
        let index = (row*(COLS as i32) + col) as usize;
        self[index] = cell.data()
    }
    
    fn xor(&mut self, cell: &impl common::CellInfo) {
        let (row, col) = cell.position();
        let index = (row*(COLS as i32) + col) as usize;
        self[index] = cell.data() ^ self[index]
    }

    fn scan(&self, row: i32, col: i32, height: usize, width: usize) -> BoardScanIterator {
        BoardScanIterator {
            col,
            height,
            width,
            it_c: col,
            it_r : row,
            board: self
        }
    }

}

#[cfg(test)]
mod test {
    use crate::{board::{self, BoardExt, ScanData}, common::{self, CellDataExt, CellInfo, EMPTY_CELL_DATA}};

    use super::{Board, SIZE, ROWS, COLS};

    #[test]
    fn can_scan_board() {

        let mut board: Board = [0; SIZE];
        board.clear();

        let mut index = 0;

        for cell_info in board.scan(0, 0, ROWS, COLS) {

            let (row, col ) = cell_info.position();
            let data: common::CellData = cell_info.data();

            if col == 0 || col+1 == COLS as i32 {

                if !data.is_static() {
                    assert!(false, "{}, {} is not wall", col, row);
                }

            } else if row+1 == ROWS as i32{

                if !data.is_static() {
                    assert!(false, "{}, {} is not wall", col, row);
                }

            } else {

                assert_eq!((index % COLS) as i32, col);
                assert_eq!((index / COLS) as i32, row);
                assert!(data.is_empty());

            }

            index += 1
        }

        assert_eq!(SIZE, index);

    }

    #[test]
    fn can_scan_board_clip_out() {

        let mut board: Board = [0; SIZE];
        board.clear();

        let mut v: Vec<board::ScanData> = vec![];

        for cell_info in board.scan(19, 5, 2, 2) {
            v.push(cell_info);
        }

        assert_eq!(4, v.len());
        assert_eq!(19, v.get(0).unwrap().row);
        assert_eq!(5, v.get(0).unwrap().col);
        assert_eq!(EMPTY_CELL_DATA, v.get(0).unwrap().data);

        assert_eq!(20, v.get(3).unwrap().row);
        assert_eq!(6, v.get(3).unwrap().col);
        assert!(v.get(3).unwrap().data().is_static());
        assert!(!v.get(3).unwrap().data().is_active());

        let s = ScanData {
            row: 20, col: 6, data: EMPTY_CELL_DATA.set_active(true)
        };

        //draw active over static
        board.xor(&s);

        v.clear();

        for cell_info in board.scan(19, 5, 2, 2) {
            v.push(cell_info);
        }
        
        assert_eq!(20, v.get(3).unwrap().row);
        assert_eq!(6, v.get(3).unwrap().col);
        assert!(v.get(3).unwrap().data().is_static());
        assert!(v.get(3).unwrap().data().is_active());
        assert!(v.get(3).unwrap().data().is_invalid());


    }

}

