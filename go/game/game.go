package game

import (
	"math/bits"
	"time"

	"github.com/ni3po42/LearnByTetris/go/board"
	"github.com/ni3po42/LearnByTetris/go/common"
	"github.com/ni3po42/LearnByTetris/go/events"
	"github.com/ni3po42/LearnByTetris/go/piece"
)

type Interval int

func (i Interval) ToDuration() time.Duration {
	return time.Duration(int(i) * int(time.Millisecond))
}

type Level int
type Score int

type Status struct {
	Level         Level
	IsGameOver    bool
	ExitGame      bool
	Score         Score
	CollaspedRows board.CollaspedRows
	Piece         *piece.Piece
	NextPieceId   piece.PieceId
}

type Game struct {
	board *board.Board
}

var scoring = [5]Score{
	0, 40, 100, 300, 1200,
}

func NewGame(b *board.Board) *Game {
	game := Game{board: b}
	return &game
}

func GetInterval(level Level) Interval {

	var rawInterval Interval

	switch {
	case level <= 8:
		rawInterval = Interval(48 - level*5)
	case level == 9:
		rawInterval = 6
	case level <= 12:
		rawInterval = 5
	case level <= 15:
		rawInterval = 4
	case level <= 18:
		rawInterval = 3
	case level <= 28:
		rawInterval = 2
	default:
		rawInterval = 1
	}

	return (1000 * rawInterval) / 6
}

func countClearRows(collapsedRows board.CollaspedRows) int {
	return bits.OnesCount(uint(collapsedRows))
}

func (g *Game) XorPiece(piece *piece.Piece) {
	gen := piece.Scan(false)
	for hasNext, cell := gen(); hasNext; hasNext, cell = gen() {
		g.board.Xor(cell)
	}
}

func (g *Game) HasCollision(piece *piece.Piece) bool {
	gen := g.board.Scan(piece)

	for hasNext, cell := gen(); hasNext; hasNext, cell = gen() {
		if cell.HasCollision() {
			return true
		}
	}

	return false
}

func (g *Game) getCollapsingRows(piece *piece.Piece) board.CollaspedRows {

	row, _ := piece.Position()
	height, _ := piece.Dimensions()

	rowCounts := [4]int{0, 0, 0, 0}

	gen := g.board.Scan(common.CellRange{
		Row: row, Col: 0, Height: height, Width: board.COLS,
	})

	var collapsingRows board.CollaspedRows

	for hasNext, cell := gen(); hasNext; hasNext, cell = gen() {
		cellRow, _ := cell.Position()
		index := cellRow - row
		data := cell.Data()

		isWall := data&board.WALL_MASK != 0
		isBlock := data&board.STATIC_MASK != 0

		if isWall || !isBlock {
			continue
		}

		rowCounts[index]++
	}

	for i, v := range rowCounts {
		if v == board.COLS-2 {
			collapsingRows.AddRow(row + i)
		}
	}

	return collapsingRows
}

func (g *Game) StartLoop(eventQueue *events.EventQueue, startLevel Level) <-chan Status {

	statusChannel := make(chan Status)

	go func(out chan<- Status) {

		isGameOver := false
		level := startLevel
		var score Score = 0
		var collapsedRows board.CollaspedRows = 0

		event, _ := eventQueue.CreateChannel()

		linesToClear := min(level*10+10, max(100, level*10-50))
		linesCleared := 0
		wasDropped := false
		clearRowCount := 0

		pieceGen := piece.Pieces()

		for {

			currentPiece, nextPieceId := pieceGen()

			//draw
			g.XorPiece(currentPiece)
			isGameOver = g.HasCollision(currentPiece)

			out <- Status{
				Level:         level,
				IsGameOver:    isGameOver,
				Score:         score,
				CollaspedRows: collapsedRows,
				Piece:         currentPiece,
				NextPieceId:   nextPieceId,
			}

			for {
				message := <-event

				//clear
				g.XorPiece(currentPiece)

				wasDropped = false

				switch message.MessageType {
				case events.DROP:
					currentPiece.Drop()
					wasDropped = true
				case events.TURN_LEFT:
					currentPiece.Rotate(common.LEFT)
				case events.TURN_RIGHT:
					currentPiece.Rotate(common.RIGHT)
				case events.LEFT:
					currentPiece.Move(common.LEFT)
				case events.RIGHT:
					currentPiece.Move(common.RIGHT)
				case events.EXIT_GAME:
					out <- Status{
						Level:         level,
						IsGameOver:    isGameOver,
						ExitGame:      true,
						Score:         score,
						CollaspedRows: collapsedRows,
						Piece:         currentPiece,
						NextPieceId:   nextPieceId,
					}
					return
				}

				//draw new
				g.XorPiece(currentPiece)

				if g.HasCollision(currentPiece) {

					//clear
					g.XorPiece(currentPiece)

					currentPiece.Revert()

					if wasDropped {
						currentPiece.Commit()
					}

					//draw
					g.XorPiece(currentPiece)

					if wasDropped {
						break
					}
				}

				out <- Status{
					Level:         level,
					IsGameOver:    isGameOver,
					Score:         score,
					CollaspedRows: collapsedRows,
					Piece:         currentPiece,
					NextPieceId:   nextPieceId,
				}

			}

			collapsedRows = g.getCollapsingRows(currentPiece)
			clearRowCount = countClearRows(collapsedRows)

			score += scoring[clearRowCount] * Score(level+1)
			linesCleared += clearRowCount

			bound := linesToClear + ((level - startLevel) * 10)

			if linesCleared >= int(bound) {
				level++
				linesCleared -= int(bound)
			}

			out <- Status{
				Level:         level,
				IsGameOver:    isGameOver,
				Score:         score,
				CollaspedRows: collapsedRows,
				Piece:         currentPiece,
				NextPieceId:   nextPieceId,
			}

			g.board.Collapse(collapsedRows)
		}

	}(statusChannel)

	return statusChannel
}
