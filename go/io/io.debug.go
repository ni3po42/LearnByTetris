//go:build TETRIS_IO_DEBUG

package io

import (
	"fmt"
	"time"

	"github.com/ni3po42/LearnByTetris/go/board"
	"github.com/ni3po42/LearnByTetris/go/events"
	"github.com/ni3po42/LearnByTetris/go/game"
)

type debugInputService struct {
}

type debugOutputService struct {
}

func CreateIOServices() (IInputService, IOutputService) {

	inputService := debugInputService{}
	outputService := debugOutputService{}

	return inputService, outputService
}

// Clear implements IOutputService.
func (c debugOutputService) Clear() {

}

// Render implements IOutputService.
func (c debugOutputService) Render(status game.Status, board *board.Board) {

	fmt.Printf("render\n")

	if status.IsGameOver {
		fmt.Printf("Game over\n")
	}
}

func (c debugOutputService) Close() {

}

// Start implements IInputService.
func (c debugInputService) Start(eventQueue *events.EventQueue, interval *game.Interval) {

	//drop loop
	go func() {

		message := events.EventMessage{MessageType: events.DROP}

		for !eventQueue.IsClosed() {
			time.Sleep(interval.ToDuration())
			eventQueue.Emit(message)
		}
	}()

}
