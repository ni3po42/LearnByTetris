import { Board } from './board.js';
import { pieces } from './piece.js';
import {activeMask, cols, staticMask, wallMask } from './constants.js';

const scoring = [
     0,
     40,
    100,
    300,
    1200
];

export class Game {

    constructor(eventStream) {
        this._eventStream = eventStream;
    }
    board = new Board();

    xor(piece) {
        for(const cell of piece.scan()) {
            this.board.xor(cell.row, cell.col, cell.data);
        }
    }



    hasCollision(piece) {
        for(const check of this.board.scan(piece)) {
            if (check.collision){
                return true;
            }
        }
        return false;
    }

    getInterval(level) {

        let frames;
        if( level <= 8) {
            frames = 48 - level * 5;
        } else if (level === 9) {
            frames = 6;
        } else if (level <= 12) {
            frames = 5;
        } else if (level <= 15) {
            frames = 4;
        } else if (level <= 18) {
            frames = 3;
        } else if (level <= 28) {
            frames = 2;
        } else {
            frames = 1;
        }

        return (frames / 6) * 100;
    }
    
    handleEvent({ name }, currentPiece) {
        switch(name) {
            case 'drop': currentPiece.drop(); return true;
            case 'turnLeft': currentPiece.rotateLeft(); break;
            case 'turnRight': currentPiece.rotateRight(); break;
            case 'left': currentPiece.left(); break;
            case 'right': currentPiece.right(); break;
        }
        return false;
    }

    async *loop({startLevel = 0} = {}) {

        let score = 0;
        let level = startLevel;
        const linesToClear = Math.min(level * 10 + 10, Math.max(100, level * 10 - 50));
        let linesCleared = 0;

        const stream = this._eventStream;

        for(const { currentPiece, nextPiece} of pieces()){
            
            //draw
            this.xor(currentPiece);
            const gameover = this.hasCollision(currentPiece);

            yield { currentPiece, nextPiece, level, score, gameover };

            if (gameover) {
                break;
            }

            while(true) {
                
              const event = await stream.next();
              if (event.done) {
                  return;
              }
              const eventData = event.value;

                //clear
                this.xor(currentPiece);
                
                const wasDrop = this.handleEvent(eventData, currentPiece);

                //draw new
                this.xor(currentPiece);

                if (this.hasCollision(currentPiece)) {
                    //clear
                    this.xor(currentPiece);
                    currentPiece.revert();
                    
                    if (wasDrop) {
                        currentPiece.commit();
                    }
                    
                    //draw
                    this.xor(currentPiece);

                    if (wasDrop) {
                        break;
                    }
                }
                
                
                yield { currentPiece, nextPiece, level, score };
            }
            

            const clearRows = this.getClearRows(currentPiece);

            score += scoring[clearRows.length] * (level + 1);

            linesCleared += clearRows.length;

            const bound = linesToClear + ((level - startLevel) * 10)
            if (linesCleared >= bound) {
                level++;
                linesCleared -= bound;
            }

            yield { currentPiece, nextPiece, clearRows, level, score };

            this.board.slice(...clearRows);
            this.board.push(clearRows.length);

        }

    }

    getClearRows(piece) {
        const rowCounts = new Map();
        for (const cell of this.board.scan({ row: piece.row, height: piece.height })) {
            let count = 0;
            if (rowCounts.has(cell.row)) {
                count = rowCounts.get(cell.row);
            } else {
                rowCounts.set(cell.row, 0);
            }

            const val = (cell.data & wallMask) ? 0 : (cell.data & (activeMask | staticMask));
            rowCounts.set(cell.row, count + (val ? 1 : 0));
        }

        const removeRows = [...rowCounts.entries()].reduce((acc, [key, value]) => {
            if (value === cols - 2) {
                return [...acc, key];
            }
            return acc;
        }, []);
        return removeRows;
    }
}
