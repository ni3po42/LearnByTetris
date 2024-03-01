package io

import (
	"github.com/ni3po42/LearnByTetris/go/board"
	"github.com/ni3po42/LearnByTetris/go/events"
	"github.com/ni3po42/LearnByTetris/go/game"
)

type IInputService interface {
	Start(*events.EventQueue, *game.Interval)
}

type IOutputService interface {
	Clear()
	Render(game.Status, *board.Board)
	Close()
}
