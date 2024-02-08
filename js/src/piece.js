import { cols, activeMask, staticMask } from './constants.js';
import { random } from './utilities.js';

class Piece {

    constructor(pieceData, scanWidth, ordinals, color, name) {
        this.pieceData = pieceData.slice();
        this.scanWidth = scanWidth;
        this.scanHeight = pieceData.length / scanWidth;
        this.name = name;
        this.color = color;
        this._ordinals = ordinals;
    }

    _direction = 0;

    _active = true;

    _row = 0;
    _col = 0;   

    _prevRow = 0;
    _prevCol = 0;
    _prevDirection = 0;

    revert() {
        this._row = this._prevRow;
        this._col = this._prevCol;
        this._direction = this._prevDirection;
    }

    init(){
        this._row = 0;
        this._col = (cols / 2 | 0) - 1;   

        this._prevRow = 0;
        this._prevCol = (cols / 2 | 0) - 1;
        this._prevDirection = 0;
    }

    saveState() {
        this._prevRow = this._row;
        this._prevCol = this._col;
        this._prevDirection = this._direction;
    }

    rotateLeft() {
        this.saveState();
        this._direction = (--this._direction + this._ordinals) % this._ordinals;
    }

    rotateRight() {
        this.saveState();
        this._direction = (++this._direction) % this._ordinals;
    }

    drop() {
        this.saveState();
        this._row++;
    }

    left() {
        this.saveState();
        this._col -= 1;
    }

    right() {
        this.saveState();
        this._col += 1; 
    }

    commit() {
        this._active = false;
    }

    get width() {
        return this._direction % 2 ? this.scanHeight : this.scanWidth;
    }

    get height() {
        return this._direction % 2 ? this.scanWidth : this.scanHeight;
    }

    get row() {
        return this._row;
    }

    get col() {
        return this._col;
    }

    *scan(posNeutral = false) {
        const bound = this.scanWidth * this.scanHeight;
        for(let index = 0; index < bound; index++) {
            let r;
            let c;
            let data = this.pieceData[index] ? this.color : 0x00;
            if (data === 0){
                continue;
            }

            data |= this._active ? activeMask : staticMask;

            switch(this._direction) {
                case 0:
                    r = index / this.scanWidth | 0;
                    c = index % this.scanWidth;
                    break;
                case 1:
                    r = index % this.scanWidth;
                    c = this.scanHeight - (index / this.scanWidth | 0) - 1;
                    break;
                case 2:
                    r = (bound - index - 1) / this.scanWidth | 0;
                    c = (bound - index - 1) % this.scanWidth;                    
                    break;
                case 3:
                    r = this.scanWidth - (index % this.scanWidth) - 1;
                    c = index / this.scanWidth | 0;                    
                    break;
            }

            yield { 
                row: r + (posNeutral ? 0 :this._row), 
                col: c + (posNeutral ? 0 :this._col), 
                data };
        }


    }

}


const pieceData = {
    T: [[
        0,1,0,
        1,1,1,
        0,0,0
    ], 3, 4],
    
    O: [[
        1,1,
        1,1
    ],2,1],
    S: [[
        0,1,1,
        1,1,0,
        0,0,0
    ],3,2],
    Z: [[
        1,1,0,
        0,1,1,
        0,0,0
    ],3,2],
    J: [[
        0,0,0,
        1,1,1,
        0,0,1
    ],3,4],
    L: [[
        0,0,0,
        1,1,1,
        1,0,0
    ],3,4],
    I: [[
        0,0,0,0,
        1,1,1,1,
        0,0,0,0,
        0,0,0,0
    ],4,2]
};

const pieceKeys = Object.keys(pieceData);

export function* pieces() {

    let currentPiece;
    while(true) {
        const nextIndex = random(0, 6);
        const key = pieceKeys[nextIndex];
        const args = [...pieceData[key], nextIndex + 1, key];
        const nextPiece = new Piece(...args);

        if (currentPiece){
            currentPiece.init();
            yield { nextPiece, currentPiece };
        } 
        currentPiece = nextPiece;
    }

}

