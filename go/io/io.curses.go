package io

import (
	"fmt"
	"log"
	"time"

	"github.com/ni3po42/LearnByTetris/go/board"
	"github.com/ni3po42/LearnByTetris/go/common"
	"github.com/ni3po42/LearnByTetris/go/events"
	"github.com/ni3po42/LearnByTetris/go/game"
	"github.com/ni3po42/LearnByTetris/go/piece"

	"github.com/gbin/goncurses"
)

const (
	_SCORE_OFFSET_ROW     = 7
	_SCORE_OFFSET_COL     = 4
	_LEVEL_OFFSET_ROW     = 10
	_LEVEL_OFFSET_COL     = 4
	_GAMEOVER_OFFSET_ROW  = 10
	_GAMEOVER_OFFSET_COL  = 22
	_BOARD_OFFSET_ROW     = 1
	_BOARD_OFFSET_COL     = 7
	_NEXTPIECE_OFFSET_ROW = 3
	_NEXTPIECE_OFFSET_COL = 2
	_DEBUG_OFFSET_ROW     = 13
	_DEBUG_OFFSET_COL     = 4
)

type cursesInputService struct {
}

type cursesOutputService struct {
	nextPieceBuffer *piece.Piece
}

func CreateIOServices() (IInputService, IOutputService) {

	inputService := cursesInputService{}
	outputService := cursesOutputService{}

	outputService.init()

	return inputService, outputService
}

// Clear implements IOutputService.
func (c cursesOutputService) Clear() {
	goncurses.StdScr().Move(10, 20)
	goncurses.StdScr().Clear()
	goncurses.StdScr().Refresh()
}

// Render implements IOutputService.
func (c cursesOutputService) Render(status game.Status, board *board.Board) {
	c.renderNextPiece(status.NextPieceId, status.Piece)
	drawGameBoard(board)
	drawInteger(int(status.Score), _SCORE_OFFSET_ROW+1, _SCORE_OFFSET_COL+1)
	drawInteger(int(status.Level), _LEVEL_OFFSET_ROW+1, _LEVEL_OFFSET_COL+1)

	if status.IsGameOver {
		drawText("GAME OVER", _GAMEOVER_OFFSET_ROW, _GAMEOVER_OFFSET_COL)
	}

	//drawDebug("%b", status.CollaspedRows)

	goncurses.StdScr().Refresh()
}

func (c cursesOutputService) init() {
	_, err := goncurses.Init()

	if err != nil {
		log.Fatal("init: ", err)
		return
	}

	goncurses.StartColor()

	goncurses.InitPair(1, goncurses.C_RED, goncurses.C_BLACK)
	goncurses.InitPair(2, goncurses.C_GREEN, goncurses.C_BLACK)
	goncurses.InitPair(3, goncurses.C_YELLOW, goncurses.C_BLACK)
	goncurses.InitPair(4, goncurses.C_BLUE, goncurses.C_BLACK)
	goncurses.InitPair(5, goncurses.C_MAGENTA, goncurses.C_BLACK)
	goncurses.InitPair(6, goncurses.C_CYAN, goncurses.C_BLACK)
	goncurses.InitPair(7, goncurses.C_WHITE, goncurses.C_BLACK)
	goncurses.InitPair(8, goncurses.C_BLACK, goncurses.C_BLACK)

	c.Clear()
	drawText("SCORE", _SCORE_OFFSET_ROW, _SCORE_OFFSET_COL)
	drawText("LEVEL", _LEVEL_OFFSET_ROW, _LEVEL_OFFSET_COL)
	goncurses.StdScr().Refresh()
}

func drawDebug(format string, args ...any) {
	goncurses.StdScr().MovePrintf(_DEBUG_OFFSET_ROW, _DEBUG_OFFSET_COL, format, args...)
}

func drawText(message string, row int, col int) {
	goncurses.StdScr().MovePrintf(row, col, "%s", message)
}

func drawInteger(value int, row int, col int) {
	drawText(fmt.Sprintf("%d", value), row, col)
}

func drawCell(cell common.ICellInfo, rowOffset int, colOffset int) {
	row, col := cell.Position()
	data := cell.Data()

	rawColor := int(data & board.COLOR_MASK)

	if rawColor != 0 {
		goncurses.StdScr().AttrOn(goncurses.ColorPair(int16(rawColor)))
		goncurses.StdScr().AttrOn(goncurses.A_REVERSE)
	} else {
		goncurses.StdScr().AttrOn(goncurses.ColorPair(8))
	}

	goncurses.StdScr().MovePrintf(row+rowOffset, 2*(col+colOffset), "  ")

	if rawColor != 0 {
		goncurses.StdScr().AttrOff(goncurses.A_REVERSE)
		goncurses.StdScr().AttrOff(goncurses.ColorPair(int16(rawColor)))
	} else {
		goncurses.StdScr().AttrOff(goncurses.ColorPair(8))
	}
}

func drawGameBoard(b *board.Board) {
	gen := b.Scan(common.CellRange{
		Row: 0, Col: 0, Height: board.ROWS, Width: board.COLS,
	})

	for hasNext, cell := gen(); hasNext; hasNext, cell = gen() {
		drawCell(cell, _BOARD_OFFSET_ROW, _BOARD_OFFSET_COL)
	}

	goncurses.StdScr().Move(0, 27)
}

func (c cursesOutputService) renderNextPiece(nextPieceId piece.PieceId, currentPiece *piece.Piece) {

	if c.nextPieceBuffer == nil || c.nextPieceBuffer.GetID() != nextPieceId {
		c.nextPieceBuffer = piece.NewPiece(nextPieceId)

		gen := currentPiece.Scan(true)

		for hasNext, cell := gen(); hasNext; hasNext, cell = gen() {
			row, col := cell.Position()
			drawCell(board.NewScanData(row, col, board.EMPTY_MASK), _NEXTPIECE_OFFSET_ROW, _NEXTPIECE_OFFSET_COL)
		}

		gen = c.nextPieceBuffer.Scan(true)

		for hasNext, cell := gen(); hasNext; hasNext, cell = gen() {
			drawCell(cell, _NEXTPIECE_OFFSET_ROW, _NEXTPIECE_OFFSET_COL)
		}

	}
}

func (c cursesOutputService) Close() {
	goncurses.End()
}

// Start implements IInputService.
func (c cursesInputService) Start(eventQueue *events.EventQueue, interval *game.Interval) {

	//input loop
	go func() {
		goncurses.Raw(true)
		goncurses.StdScr().Keypad(true)

		message := events.EventMessage{MessageType: events.NONE}

		for !eventQueue.IsClosed() {

			c := goncurses.StdScr().GetChar()

			switch c {
			case goncurses.KEY_DOWN:
				message.MessageType = events.DROP
			case goncurses.KEY_LEFT:
				message.MessageType = events.LEFT
			case goncurses.KEY_RIGHT:
				message.MessageType = events.RIGHT
			case 'z':
				message.MessageType = events.TURN_LEFT
			case 'c':
				message.MessageType = events.TURN_RIGHT
			case 27:
				message.MessageType = events.EXIT_GAME
			default:
				message.MessageType = events.NONE
			}

			if message.MessageType != events.NONE {
				eventQueue.Emit(message)
			}
		}
	}()

	//drop loop
	go func() {

		message := events.EventMessage{MessageType: events.DROP}

		for !eventQueue.IsClosed() {
			time.Sleep(interval.ToDuration())
			eventQueue.Emit(message)
		}
	}()

}
