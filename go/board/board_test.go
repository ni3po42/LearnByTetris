package board

import (
	"testing"

	"github.com/ni3po42/LearnByTetris/go/common"
)

func TestCanScanBoard(t *testing.T) {

	var b Board

	b.clear(0, SIZE)

	gen := b.Scan(common.CellRange{Row: 0, Col: 0, Height: ROWS, Width: COLS})

	var index int = 0

	for hasNext, cellInfo := gen(); hasNext; hasNext, cellInfo = gen() {

		row, col := cellInfo.Position()
		data := cellInfo.Data()

		if col == 0 || col+1 == COLS {

			if data != STATIC_MASK|WALL_MASK|WALL_COLOR {
				t.Errorf("Data %x did not match", data)
			}

		} else if row+1 == ROWS {

			if data != STATIC_MASK|WALL_MASK|WALL_COLOR {
				t.Errorf("Data %x did not match", data)
			}

		} else {

			if index%COLS != col {
				t.Errorf("expected %d for col, got %d", index%COLS, col)
			}

			if index/COLS != row {
				t.Errorf("expected %d for row, got %d", index/COLS, row)
			}

			if EMPTY_MASK != data {
				t.Errorf("expected %d for data, got %d", EMPTY_MASK, data)
			}

		}

		index += 1
	}

	if SIZE != index {
		t.Errorf("expected %d for index, got %d", SIZE, index)
	}

}
