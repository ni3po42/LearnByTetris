package piece

import (
	"testing"

	"github.com/ni3po42/LearnByTetris/go/board"
	"github.com/ni3po42/LearnByTetris/go/common"
)

func TestCanScanPiece(t *testing.T) {

	piece := NewPiece('T')

	expect := [12]int{
		0, 1, 1,
		1, 0, 1,
		1, 1, 1,
		1, 2, 1,
	}

	gen := piece.Scan(true)
	index := 0

	for hasNext, actual := gen(); hasNext; hasNext, actual = gen() {

		expectedRow, expectedCol, expectedData := expect[index], expect[index+1], expect[index+2]

		actualRow, actualCol := actual.Position()
		var actualData common.CellData

		if actual.Data()&board.ACTIVE_MASK == board.ACTIVE_MASK {
			actualData = 1
		} else {
			actualData = 0
		}

		if expectedRow != actualRow {
			t.Errorf("expected row: %d, got %d", expectedRow, actualRow)
		}

		if expectedCol != actualCol {
			t.Errorf("expected col: %d, got %d", expectedCol, actualCol)
		}

		if expectedData != int(actualData) {
			t.Errorf("expected data: %d, got %d", expectedData, actualData)
		}

		index += 3
	}

}

func TestCanScanPieceCommit(t *testing.T) {

	piece := NewPiece('T')
	piece.Commit()

	expect := [12]int{
		0, 1, 1,
		1, 0, 1,
		1, 1, 1,
		1, 2, 1,
	}

	gen := piece.Scan(true)
	index := 0

	for hasNext, actual := gen(); hasNext; hasNext, actual = gen() {

		expectedRow, expectedCol, expectedData := expect[index], expect[index+1], expect[index+2]

		actualRow, actualCol := actual.Position()
		var actualData common.CellData

		if actual.Data()&board.STATIC_MASK == board.STATIC_MASK {
			actualData = 1
		} else {
			actualData = 0
		}

		if expectedRow != actualRow {
			t.Errorf("expected row: %d, got %d", expectedRow, actualRow)
		}

		if expectedCol != actualCol {
			t.Errorf("expected col: %d, got %d", expectedCol, actualCol)
		}

		if expectedData != int(actualData) {
			t.Errorf("expected data: %d, got %d", expectedData, actualData)
		}

		index += 3
	}

}
