import { Game } from '../game.js';
import { getEventStream, resetDropInterval } from './web.eventstream.js';
import { WebRenderer  } from './web.renderer.js';


let level = 0;

const eventStream = getEventStream();
const stream = eventStream.start();

const game = new Game(stream);
const dropInterval = resetDropInterval(eventStream, game.getInterval(level));
const renderer = new WebRenderer();

renderer.init(game);


dropInterval.next();
(async()=>{
for await(const status of game.loop({ startLevel: level})){
        renderer.render(game, status);
        if (status.gameover) {
                break;
        }
        if(level !== status.level) {
                level = status.level;
                dropInterval.next(game.getInterval(level));
        }
}
dropInterval.next();
eventStream.stop();
})();