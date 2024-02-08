import { rows, cols } from '../constants.js';
import process from 'process';
import { cursorTo, clearLine, clearScreenDown } from 'readline';

const boardOffset = {rowOffset: 1, colOffset: 7};
const nextPieceOffset = {rowOffset: 3, colOffset: 2}

const scoreOffset = { row: 7, col: 4};
const levelOffset = { row: 10, col: 4};
const gameOverOffset = { row: 10, col: 22};

export class ConsoleRenderer {

    buffer = new Array(rows * cols).fill(undefined);
    nextPiece = null;

    clear() {
        cursorTo(process.stdout, 0, 0);
        clearLine(process.stdout, 0);
        clearScreenDown(process.stdout);

    }

    init(game) {
        this.clear();
        this.drawText('SCORE', scoreOffset.row, scoreOffset.col);
    
        this.drawText('LEVEL', levelOffset.row, levelOffset.col);
    }



    drawCell({row = 0, col = 0, data}, {rowOffset = 0, colOffset = 0}) {

        const t = (data & 0x0f).toString(16);

        const d = ''.padEnd(2, String.fromCharCode(9619));
        const color = (data & 0x07);
        
        cursorTo(process.stdout, (col+colOffset) * 2, row + rowOffset);
        
        process.stdout.write(`\x1b[${30 + color}m`);

        if (!(data & 0x08)) {
            process.stdout.write(`\x1b[${40 + color}m`);
                    
        }

        process.stdout.write(d);
        process.stdout.write('\x1b[0m');
        
    }



    drawScore(score) {
        this.drawText(score, scoreOffset.row + 1, scoreOffset.col);
    }

    drawLevel(level) {
        this.drawText(level, levelOffset.row + 1, levelOffset.col);
    }

    drawText(val, row, col) {
        if (val !== undefined) {
            cursorTo(process.stdout, col, row);
            process.stdout.write(val + '');
        }
    }

    render(game, gameStatus) {

        this.renderNextPiece(gameStatus);
        this.gameBoard(game);
        this.drawScore(gameStatus.score);
        this.drawLevel(gameStatus.level);
        this.drawText(gameStatus.gameover ? 'GAME OVER': undefined, gameOverOffset.row, gameOverOffset.col);
    }

    renderNextPiece(gameStatus) {
        if(this.nextPiece !== gameStatus.nextPiece){
            this.nextPiece = gameStatus.nextPiece;  
            for(const cell of gameStatus.currentPiece.scan(true)) {
                this.drawCell({row: cell.row, col: cell.col, data: 0x00}, nextPieceOffset);
            }

            for(const cell of gameStatus.nextPiece.scan(true)) {
                this.drawCell(cell, nextPieceOffset);
            }
        }
    }

    gameBoard(game) {
        for(const cell of game.board.scan()){
            const bCell = this.buffer[cell.row * cols + cell.col];
            if (cell.data !== bCell){
                this.buffer[cell.row * cols + cell.col] = cell.data;
                this.drawCell(cell, boardOffset);
                //console.log(cell.data ? '#' : ' ');    
            }
    
        }
        cursorTo(process.stdout, 0, 27);
    
    }

}