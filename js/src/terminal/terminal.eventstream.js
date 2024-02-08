
import { emitKeypressEvents } from 'readline';
import process from 'process';
import { EventStream } from '../eventStream.js';

import { setInterval, clearInterval } from 'timers'

export function getEventStream() {

    const eventStream = new EventStream();

    if(process.stdin.isTTY){
        
      emitKeypressEvents(process.stdin);
      process.stdin.setRawMode(true);
      
      process.stdin.on('keypress', (str, key) => {
          if (key.ctrl && key.name === 'c') {
            process.exit();
          } else {
      
              switch(key.name) {
                  case 'a': eventStream.emit({name: 'left'});break;
                  case 'd': eventStream.emit({name: 'right'});break;
                  case 's': eventStream.emit({name: 'drop'});break;
                  case 'q': eventStream.emit({name: 'turnLeft'});break;
                  case 'e': eventStream.emit({name: 'turnRight'});break;
              }
          }
        });
    }

  return eventStream;
}

export function* resetDropInterval(eventStream, delay) {

  let intervalTimer;

  while(delay !== undefined) {

    if(intervalTimer){
      clearInterval(intervalTimer);
    }
    intervalTimer = setInterval(()=> {
      eventStream.emit({name: 'drop'})
    }, delay);

    delay = yield;
  }

  if(intervalTimer){
    clearInterval(intervalTimer);
  }
  
};