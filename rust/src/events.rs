use crate::common;

#[derive(Clone, Debug, PartialEq)]
pub enum EventType {
    Drop, Slide(common::Direction), Turn(common::Direction), ExitGame
}

#[derive(Clone)]
pub struct EventMessage{
    pub message_type: EventType
}

pub type EventSender = std::sync::mpsc::Sender<Option<EventMessage>>;
pub type EventReceiver = std::sync::mpsc::Receiver<Option<EventMessage>>;

pub fn create_event_channel() -> (EventSender, EventReceiver) {
    std::sync::mpsc::channel()
}
