package board

import (
	"github.com/ni3po42/LearnByTetris/go/common"
)

const (
	COLS int = 12
	ROWS int = 21
	SIZE     = COLS * ROWS
)

const (
	ACTIVE_MASK common.CellData = 0x20
	STATIC_MASK common.CellData = 0x40
	WALL_MASK   common.CellData = 0x80
	EMPTY_MASK  common.CellData = 0x00
	WALL_COLOR  common.CellData = 0x0F
	COLOR_MASK  common.CellData = 0x07
)

type ScanData struct {
	row, col int
	data     common.CellData
}

func NewScanData(row int, col int, data common.CellData) ScanData {
	return ScanData{row: row, col: col, data: data}
}

func (t ScanData) Position() (int, int) {
	return t.row, t.col
}

func (t ScanData) Data() common.CellData {
	return t.data
}

func (t ScanData) HasCollision() bool {
	return (t.data & (ACTIVE_MASK | STATIC_MASK)) == (ACTIVE_MASK | STATIC_MASK)
}

type CollaspedRows int

func (t *CollaspedRows) AddRow(row int) {
	*t = (1 << row) | *t
}

type Board struct {
	board [SIZE]common.CellData
}

func (b *Board) clear(startIndex int, stopIndex int) {

	for i := range b.board[startIndex:stopIndex] {
		c := int(i) % COLS
		r := int(i) / COLS
		if c == 0 || c == (COLS-1) || r == (ROWS-1) {
			b.board[i] = STATIC_MASK | WALL_MASK | WALL_COLOR
		} else {
			b.board[i] = EMPTY_MASK
		}
	}

}

func (b *Board) Clear() {
	b.clear(0, SIZE)
}

func (b *Board) Collapse(rows CollaspedRows) {

	const topRow CollaspedRows = 1
	const noRow CollaspedRows = 0

	if rows == 0 {
		return
	}

	finger := SIZE
	for i := SIZE - 1; i >= 0; i-- {
		rowNumber := i / COLS
		collaspseState := topRow << rowNumber

		if (collaspseState & rows) == noRow {
			finger -= 1
			b.board[finger] = b.board[i]
		}
	}

	b.clear(0, finger)
}

func (b *Board) Set(cell common.ICellInfo) {
	row, col := cell.Position()
	index := row*COLS + col
	b.board[index] = cell.Data()
}

func (b *Board) Xor(cell common.ICellInfo) {
	row, col := cell.Position()
	index := row*COLS + col
	b.board[index] = cell.Data() ^ b.board[index]
}

func (b *Board) Scan(rectangle common.IRectangle) func() (bool, common.ICellInfo) {
	row, col := rectangle.Position()
	height, width := rectangle.Dimensions()
	r, c := row, col

	return func() (bool, common.ICellInfo) {

		if width <= 0 || height <= 0 {
			return false, nil
		}

		index := r*COLS + c
		scanData := ScanData{row: r, col: c, data: b.board[index]}

		c++

		if c-col == width {
			c = col
			r++

			if r == ROWS {
				height = 0
			} else {
				height--
			}
		}

		return true, scanData
	}
}
