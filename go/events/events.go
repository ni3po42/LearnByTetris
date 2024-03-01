package events

import (
	"sync"
)

type EventType byte

const (
	NONE EventType = iota
	DROP
	LEFT
	RIGHT
	TURN_LEFT
	TURN_RIGHT
	PAUSE
	EXIT_GAME
)

type EventQueueError struct {
	s string
}

func (e *EventQueueError) Error() string {
	return e.s
}

type EventMessage struct {
	MessageType EventType
}

type EventQueue struct {
	sync.Mutex
	isDone      bool
	channels    []chan<- EventMessage
	emitChannel chan EventMessage
}

func NewEventQueue(args ...int) *EventQueue {
	var initialCap int = 100

	if len(args) > 0 {
		initialCap = args[0]
	}

	queue := EventQueue{
		isDone:      false,
		channels:    make([]chan<- EventMessage, 0, initialCap),
		emitChannel: make(chan EventMessage, initialCap),
	}

	go func(q *EventQueue) {

		for !q.isDone {
			message := <-q.emitChannel
			for _, c := range q.channels {

				if q.isDone {
					break
				}

				c <- message
			}
		}

	}(&queue)

	return &queue
}

func (t *EventQueue) Emit(message EventMessage) {
	t.Lock()
	defer t.Unlock()

	if t.isDone || len(t.channels) == 0 {
		return
	}

	t.emitChannel <- message
}

func (t *EventQueue) IsClosed() bool {
	t.Lock()
	defer t.Unlock()
	return t.isDone
}

func (t *EventQueue) Close() {
	t.Lock()
	defer t.Unlock()

	t.isDone = true

	for _, channel := range t.channels {
		close(channel)
	}

	close(t.emitChannel)
}

func (t *EventQueue) CreateChannel(args ...int) (<-chan EventMessage, error) {
	t.Lock()
	defer t.Unlock()

	if t.isDone {
		return nil, &EventQueueError{s: "Queue is closed"}
	}

	var channelBuffSize int = 100

	if len(args) > 0 {
		channelBuffSize = args[0]
	}

	c := make(chan EventMessage, channelBuffSize)
	t.channels = append(t.channels, c)

	return c, nil
}
