import { rows, cols, staticMask, wallMask, activeMask } from './constants.js';

export const wallColor = 0x0f;

export class Board {

    constructor() {
        this.board = new Array(rows * cols).fill(0x00);

        this.board
        .map((_,i)=>i)
        .filter(i => {
            const c = i % cols;
            const r = i / cols | 0;
            return c === 0 || c === (cols - 1) || r === (rows - 1);
        }).forEach(i => this.board[i] = (staticMask | wallMask | wallColor));

    }
    
    push(count) {

        while(count--){
            const line = new Array(cols).fill(0x00);
            line[0] = staticMask | wallMask | wallColor;
            line[cols-1] = staticMask | wallMask | wallColor;
            this.board.unshift(...line);
        }

    }

    slice(...removeCols) {

        removeCols.forEach(row => {
            const start = row * cols;
            for(let index = start; index < start + cols; index++ ) {
                this.board[index] = undefined;
            }
        });
        
        this.board = this.board.filter(x => x !== undefined);
    }

    get(row, col) {
        return this.board[row * cols + col];
    }

    set(row, col, data) {
        const index = row * cols + col;
        this.board[index] = data; 
    } 

    xor(row, col, data) {
        const index = row * cols + col;
        this.board[index] ^= data;
    }

    *scan({ row = 0, height = rows, col = 0, width = cols } = {}) {

        for(let r = row; r < row + height; r++) {
            for(let c = col; c < col + width; c++) {
                const index = r * cols + c;
                const data = this.board[index];
                const collision = (data & (activeMask | staticMask)) === (activeMask | staticMask);
                yield { row: r, col: c, data, collision };
            }
        }

    }

}
