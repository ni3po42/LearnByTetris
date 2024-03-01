package events

import (
	"testing"

	"time"
)

func TestDefaultEventMessageTypeIsNone(t *testing.T) {

	m := EventMessage{}

	if m.MessageType != NONE {
		t.Fail()
	}
}

func TestCanCreateEventQueue(t *testing.T) {

	queue := NewEventQueue()
	defer queue.Close()

	if queue.IsClosed() {
		t.Errorf("queue is closed")
	}

}

func TestCanCreateChannel(t *testing.T) {

	queue := NewEventQueue()
	defer queue.Close()

	c, err := queue.CreateChannel()

	if c == nil {
		t.Errorf("channel not created.")
	}

	if err != nil {
		t.Errorf("error returned from CreateChannel: %s", err.Error())
	}

}

func TestCanEmitMessage(t *testing.T) {

	queue := NewEventQueue()
	defer queue.Close()

	c, _ := queue.CreateChannel()

	expectedMessage := EventMessage{MessageType: DROP}
	timer := time.After(2 * time.Second)
	queue.Emit(expectedMessage)

	select {
	case actualMessage := <-c:

		if expectedMessage.MessageType != actualMessage.MessageType {
			t.Errorf(
				"Expected message type did not match actual: e=%d, a=%d",
				expectedMessage.MessageType,
				actualMessage.MessageType,
			)
		}

	case <-timer:
		t.Errorf("Emit <=> channel timeout")

	}

}

func TestCanEmitMultipleMessages(t *testing.T) {

	queue := NewEventQueue()
	defer queue.Close()

	c, _ := queue.CreateChannel()
	timer := time.After(2 * time.Second)
	expectedTypes := []EventType{DROP, LEFT, RIGHT}

	for _, expectedType := range expectedTypes {
		queue.Emit(EventMessage{MessageType: expectedType})
	}

	for _, expectedType := range expectedTypes {
		select {
		case actualMessage := <-c:

			if expectedType != actualMessage.MessageType {
				t.Errorf(
					"Expected message type did not match actual: e=%d, a=%d",
					expectedType,
					actualMessage.MessageType,
				)
			}

		case <-timer:
			t.Errorf("Emit <=> channel timeout")

		}
	}

}

func TestCanEmitMessageOnSeparateThread(t *testing.T) {

	queue := NewEventQueue()
	defer queue.Close()

	c, _ := queue.CreateChannel()

	expectedMessage := EventMessage{MessageType: DROP}

	go func() {
		queue.Emit(expectedMessage)
	}()

	actualMessage := <-c

	if expectedMessage.MessageType != actualMessage.MessageType {
		t.Errorf(
			"Expected message type did not match actual: e=%d, a=%d",
			expectedMessage.MessageType,
			actualMessage.MessageType,
		)
	}
}

func TestCanEmitMessageToMultipleChannels(t *testing.T) {

	queue := NewEventQueue()
	defer queue.Close()

	c1, _ := queue.CreateChannel()
	c2, _ := queue.CreateChannel()

	expectedMessage := EventMessage{MessageType: DROP}

	go func() {
		queue.Emit(expectedMessage)
	}()

	actualMessage1 := <-c1
	actualMessage2 := <-c2

	if expectedMessage.MessageType != actualMessage1.MessageType {
		t.Errorf(
			"Expected message type did not match actual: e=%d, a=%d",
			expectedMessage.MessageType,
			actualMessage1.MessageType,
		)
	}

	if expectedMessage.MessageType != actualMessage2.MessageType {
		t.Errorf(
			"Expected message type did not match actual: e=%d, a=%d",
			expectedMessage.MessageType,
			actualMessage2.MessageType,
		)
	}
}
