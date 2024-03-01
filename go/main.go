package main

import (
	"time"

	"github.com/ni3po42/LearnByTetris/go/board"
	"github.com/ni3po42/LearnByTetris/go/events"
	"github.com/ni3po42/LearnByTetris/go/game"
	"github.com/ni3po42/LearnByTetris/go/io"
)

func main() {

	input, output := io.CreateIOServices()
	defer output.Close()

	var level game.Level = 10
	interval := game.GetInterval(level)

	var board board.Board
	board.Clear()

	events := events.NewEventQueue()
	defer events.Close()

	game := game.NewGame(&board)

	statusGen := game.StartLoop(events, level)

	input.Start(events, &interval)

	for {

		status := <-statusGen

		if level != status.Level {
			level = status.Level
		}

		output.Render(status, &board)

		if status.IsGameOver {
			time.Sleep(3 * time.Second)
			break
		}

		if status.ExitGame {
			break
		}
	}
}
