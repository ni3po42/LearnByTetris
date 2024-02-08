

import { EventStream } from '../eventStream.js';


export function getEventStream() {

    const eventStream = new EventStream();

    window.addEventListener('keydown', e => {

        switch(e.key) {
                case 'a': eventStream.emit({name: 'left'});break;
                case 'd': eventStream.emit({name: 'right'});break;
                case 's': eventStream.emit({name: 'drop'});break;
                case 'q': eventStream.emit({name: 'turnLeft'});break;
                case 'e': eventStream.emit({name: 'turnRight'});break;
            }

    });
    
    
            
  

  return eventStream;
}

export function* resetDropInterval(eventStream, delay) {

  let intervalTimer;

  while(delay !== undefined) {

    if(intervalTimer){
      window.clearInterval(intervalTimer);
    }
    intervalTimer = window.setInterval(()=> {
      eventStream.emit({name: 'drop'})
    }, delay);

    delay = yield;
  }

  if(intervalTimer){
    window.clearInterval(intervalTimer);
  }
  
};