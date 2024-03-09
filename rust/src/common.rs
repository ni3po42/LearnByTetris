#[derive(Clone, Debug, PartialEq)]
pub enum Direction {
    Left, Right
}

pub type CellData = u8;
pub type Color = CellData;

const ACTIVE_MASK: CellData = 0x20;
const STATIC_MASK: CellData = 0x40;
const WALL_MASK: CellData =   0x80;
const EMPTY_MASK: CellData =  0x00;
pub const WALL_COLOR: CellData =  0x0F;
const COLOR_MASK: CellData =  0x07;

pub const EMPTY_CELL_DATA: CellData = EMPTY_MASK;

pub trait CellDataExt {
    fn get_color(&self) -> Color;
    fn is_active(&self) -> bool;
    fn is_static(&self) -> bool;
    fn is_invalid(&self) -> bool;
    fn is_wall(&self) -> bool;
    fn is_empty(&self) -> bool;

    fn set_color(&self, color: Color) -> CellData;
    fn set_active(&self, b: bool) -> CellData;
    fn set_static(&self, b: bool) -> CellData;
}

impl CellDataExt for CellData {
    fn get_color(&self) -> Color {
        self & COLOR_MASK
    }

    fn is_active(&self) -> bool {
        self & ACTIVE_MASK == ACTIVE_MASK
    }

    fn is_static(&self) -> bool {
        self & STATIC_MASK == STATIC_MASK
    }

    fn is_invalid(&self) -> bool {
        (self & (STATIC_MASK | ACTIVE_MASK)) == (STATIC_MASK | ACTIVE_MASK)
    }

    fn is_wall(&self) -> bool {
        (self & (STATIC_MASK | WALL_MASK)) == (STATIC_MASK | WALL_MASK)
    }

    fn is_empty(&self) -> bool {
        *self == EMPTY_MASK
    }

    fn set_color(&self, color: Color) -> CellData {
        (*self & !COLOR_MASK) | (color & COLOR_MASK)
    }

    fn set_active(&self, b: bool) -> CellData{
        if b { *self | ACTIVE_MASK } else { *self & !ACTIVE_MASK}
    }

    fn set_static(&self, b: bool) -> CellData {
        if b { *self | STATIC_MASK } else { *self & !STATIC_MASK}
    }
}

pub type CollapsedRows = u32;
pub trait CollapsedRowsExt {
    fn add_row(&mut self, row: usize);

}

impl CollapsedRowsExt for CollapsedRows {
    fn add_row(&mut self, row: usize) {
        *self = (1 << row) | *self
    }    
}

pub trait CellInfo {
    fn position(&self) -> (i32, i32);
    fn data(&self) -> CellData;
    fn has_collision(&self) -> bool;
    fn with_data(&self, data: CellData) -> impl CellInfo;
}

pub trait Rectangle {
    fn position(&self) -> (i32, i32);
    fn dimensions(&self) -> (usize, usize);
}

pub struct CellRange{
    row: i32,
    col: i32,
    height: usize,
    width: usize
}

impl Rectangle for CellRange {
    fn position(&self) -> (i32, i32) {
        (self.row, self.col)
    }
    
    fn dimensions(&self) -> (usize, usize) {
        (self.height, self.width)
    }

}

#[cfg(test)]
mod tests {
    use crate::common::{CellData, CellDataExt, EMPTY_CELL_DATA, WALL_MASK};


    #[test]
    fn can_mark_cell_invalid() {

        let mut cell: CellData = EMPTY_CELL_DATA;

        cell = cell.set_active(true);
        cell = cell.set_static(true);

        //base
        assert!(cell.is_invalid());

        cell = cell.set_color(3);

        //color
        assert!(cell.is_invalid());

        //wall
        cell = cell | WALL_MASK;

        assert!(cell.is_invalid());

    }
}

