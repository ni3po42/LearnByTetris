import { Game } from '../game.js';
import { getEventStream, resetDropInterval } from './terminal.eventstream.js';
import { ConsoleRenderer } from './terminal.renderer.js';

import process from 'process';

let level = 25;

const eventStream = getEventStream();
const stream = eventStream.start();

const game = new Game(stream);
const dropInterval = resetDropInterval(eventStream, game.getInterval(level));
const renderer = new ConsoleRenderer();

renderer.init(game);


dropInterval.next();

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

process.exit();