use common::CellDataExt;
use crate::board::COLS;
use crate::common::{self, Rectangle};

pub type PieceId = char;

struct PieceData {
    field: u16,
    width: usize,
    height: usize,
    ordinal: u8,
    color: common::CellData
}

fn get_raw_piece(id: PieceId) -> Option<PieceData> {
    match id {
        'T' => Some(PieceData{
            field: 0x3A,
            width: 3, height: 3, ordinal: 4, color: 1
        }),
        'O' => Some(PieceData{
            field: 0x0F,
            width: 2, height: 2, ordinal: 1, color: 2
        }),
        'S' => Some(PieceData{
            field: 0x1E,
            width: 3, height: 3, ordinal: 2, color: 3
        }),
        'Z' => Some(PieceData{
            field: 0x33,
            width: 3, height: 3, ordinal: 2, color: 4
        }),
        'J' => Some(PieceData{
            field: 0x138,
            width: 3, height: 3, ordinal: 4, color: 5
        }),
        'L' => Some(PieceData{
            field: 0x78,
            width: 3, height: 3, ordinal: 4, color: 6
        }),
        'I' => Some(PieceData{
            field: 0xF0,
            width: 4, height: 4, ordinal: 2, color: 7
        }),
        _ => None
    }
}

fn id_lookup(index: usize) -> PieceId {
    let i = index % 7;
    match i {
        0 => 'T',
        1 => 'O',
        2 => 'S',
        3 => 'Z',
        4 => 'J',
        5 => 'L',
        6 => 'I',
        _ => unreachable!()        
    }
}

pub struct Piece {
    row: i32,
    col: i32,
    prev_row: i32,
    prev_col: i32,
    turns: usize,
    prev_turns: usize,
    active: bool,
    id: PieceId
}

impl std::fmt::Display for Piece {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        writeln!(f, "Piece: id {} row {} col {}", self.id, self.row, self.col)
    }
}

pub fn new_piece(id: PieceId) -> Piece {
    Piece {
        id,
        active: false,
        row: 0, col: 0, prev_row: 0, prev_col: 0, turns: 0, prev_turns: 0
    }    
}

fn get_random_piece_id() -> PieceId {
    id_lookup(rand::random::<usize>() % 7)
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

pub struct PieceScanIterator<'p> {
    index: usize,
    piece: &'p Piece,
    width: usize,
    height: usize,
    color: u8,
    field: u16,
    use_netural_position: bool
}

impl<'p> Iterator for PieceScanIterator<'p> {
    type Item = ScanData;

    fn next(&mut self) -> Option<Self::Item> {
        
        let r: i32;
        let c: i32;

        let bound = self.width * self.height;

		loop {

			if self.index == bound {
				return None;
			}

            if self.field & (1 << self.index) == 0 {
                self.index += 1;
            } else {
                break;
            }

		}

        let cell_data = common::EMPTY_CELL_DATA
            .set_color(self.color)
            .set_active(self.piece.active)
            .set_static(!self.piece.active);

        match self.piece.turns {
            0 => {
                r = (self.index / self.width) as i32;
                c = (self.index % self.width) as i32;
            },
            1 => {
                r = (self.index % self.width) as i32;
                c = (self.height - (self.index / self.width) - 1) as i32;
            },
            2 => {
                r = ((bound - self.index - 1) / self.width) as i32;
                c = ((bound - self.index - 1) % self.width) as i32;
            },
            3 => {
                r = (self.width - (self.index % self.width) - 1) as i32;
                c = (self.index / self.width) as i32;
            },
            _ => return None
        }

		self.index += 1;
		if self.use_netural_position {
            Some(ScanData{ row: r, col: c, data: cell_data })
		} else {
            Some(ScanData{ row: (r + self.piece.row) as i32, col: c + self.piece.col, data: cell_data })
		}

    }
}

pub struct NextPieceIterator {

    current_id: PieceId,
    next_id: PieceId
}

impl Iterator for NextPieceIterator {
    type Item = (Piece, PieceId);

    fn next(&mut self) -> Option<Self::Item> {
        
        self.current_id = self.next_id;
        self.next_id = get_random_piece_id();
        
        let mut current_piece = new_piece(self.current_id);
        current_piece.play();

		Some((current_piece, self.next_id))
    }
}

pub fn piece_generator() -> NextPieceIterator {
    NextPieceIterator {
        current_id: get_random_piece_id(),
        next_id: get_random_piece_id()
    }
}

impl Piece {

    fn save(&mut self) {
        self.prev_row = self.row;
        self.prev_col = self.col;
        self.prev_turns = self.turns;
    }

    pub fn revert(&mut self) {
        self.row = self.prev_row;
        self.col = self.prev_col;
        self.turns = self.prev_turns;
    }

    pub fn play(&mut self) {
        self.active = true;

        self.row = 0;
        self.col = ((COLS / 2) - 1) as i32;
        self.prev_row = 0;
        self.prev_col = self.col;
        self.turns = 0;

    }

    pub fn rotate(&mut self,  direction: common::Direction) {
        self.save();

        let raw = get_raw_piece(self.id).unwrap();
        let ordinal: usize = raw.ordinal as usize;
        match direction {
            common::Direction::Left => self.turns = (self.turns + ordinal - 1) % ordinal,
            common::Direction::Right => self.turns = (self.turns + 1) % ordinal,
        }
    }

    pub fn drop(&mut self) {
        self.save();
        self.row += 1;
    }

    pub fn slide(&mut self, direction: common::Direction) {
        self.save();

        match direction {
            common::Direction::Left => self.col -= 1,
            common::Direction::Right => self.col += 1
        }
    }

    pub fn commit(&mut self) {
        self.active = false
    }

    pub fn scan(&self, use_netural_position: bool) -> PieceScanIterator {

        let raw = get_raw_piece(self.id).unwrap();
            
        PieceScanIterator {
            index: 0,
            piece: self,
            width: raw.width,
            height: raw.height,
            field: raw.field,
            color: raw.color,
            use_netural_position
        }
    }


}

impl Rectangle for Piece {
    fn position(&self) -> (i32, i32) {
        (self.row, self.col)
    }

    fn dimensions(&self) -> (usize, usize) {
        let raw = get_raw_piece(self.id).unwrap();
        if self.turns % 2 == 0 {
            (raw.height, raw.width)
        } else {
            (raw.width, raw.height)
        }
    }
}

#[cfg(test)]
mod tests {
    use crate::common::{CellDataExt, CellInfo};
    use super::new_piece;


    #[test]
    fn can_scan_piece() {

        let mut piece = new_piece('T');
        piece.play();

        let expect: [(i32, i32); 4] = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, 2)
        ];

        let mut index = 0;

        for cell in piece.scan(true) {

            let (row, col) = expect[index];

            let (actual_row, actual_col) = cell.position();

            assert_eq!(row, actual_row);
            assert_eq!(col, actual_col);

            let data = cell.data();

            assert!(data.is_active());
            assert!(!data.is_static());

            index += 1;
        }

    }

    #[test]
    fn can_scan_piece_commit() {

        let mut piece = new_piece('T');
        piece.play();
        piece.commit();

        let expect: [(i32, i32); 4] = [
            (0, 1),
            (1, 0),
            (1, 1),
            (1, 2)
        ];

        let mut index = 0;

        for cell in piece.scan(true) {

            let (row, col) = expect[index];

            let (actual_row, actual_col) = cell.position();

            assert_eq!(row, actual_row);
            assert_eq!(col, actual_col);

            let data = cell.data();

            assert!(!data.is_active());
            assert!(data.is_static());

            index += 1;
        }

    }

}