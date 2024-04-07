#include "render.h"
#include "../board/board.h"
#include <ncurses.h>
#include "../constants.h"
#include <stdio.h>
#include <assert.h>

#define SCORE_OFFSET_ROW 7
#define SCORE_OFFSET_COL 4

#define LEVEL_OFFSET_ROW 10
#define LEVEL_OFFSET_COL 4

#define GAMEOVER_OFFSET_ROW 10
#define GAMEOVER_OFFSET_COL 22

#define BOARD_OFFSET_ROW 1
#define BOARD_OFFSET_COL 7

#define NEXTPIECE_OFFSET_ROW 3
#define NEXTPIECE_OFFSET_COL 2

#define DEBUG_OFFSET_ROW 13
#define DEBUG_OFFSET_COL 4

static Board boardBuffer;
static int nextPieceId = -1;

static void drawText(const char* text, int row, int col) {
    if (text == NULL) {
        return;
    }
    mvprintw(row, col, text);
}

static void drawCell(int row, int col, int data, int rowOffset, int colOffset) {
    
    int color = (data & 0x07);
    
    if (color != 0) {
        attron(COLOR_PAIR(color));
        attron(A_REVERSE);
    } else {
        attron(COLOR_PAIR(8));
    }
    
    mvprintw(row + rowOffset, (col + colOffset) * 2, "  ");
    
    if (color != 0) {
        attroff(A_REVERSE);
        attroff(COLOR_PAIR(color));
    } else {
        attroff(COLOR_PAIR(8));
    }
     
}

static void drawInteger(int value, int row, int col) {
    char str[10];
    sprintf(str, "%d", value);
    drawText(str, row, col);
}

static void renderNextPiece_erase(int row, int col, int data) {
    drawCell(row, col, 0x00, NEXTPIECE_OFFSET_ROW, NEXTPIECE_OFFSET_COL);
}

static void renderNextPiece_draw(int row, int col, int data) {
    drawCell(row, col, data, NEXTPIECE_OFFSET_ROW, NEXTPIECE_OFFSET_COL);
}

static void renderNextPiece(NextPieceData nextPieceData) {
    
    if (nextPieceId != getPieceId(nextPieceData.nextPiece)) {
        nextPieceId = getPieceId(nextPieceData.nextPiece);
        
        pieceScan(nextPieceData.currentPiece, true, renderNextPiece_erase);
        pieceScan(nextPieceData.nextPiece, true, renderNextPiece_draw);    
    }
}

static void gameBoard() {
   
    GeneratorHandle boardHandle = scanBoard(0,0,BOARD_ROWS, BOARD_COLS);
    BoardScanData scanData;
    
    while(gen_next(boardHandle, NULL, &scanData)) {
    
         int bCell = boardBuffer[scanData.row * BOARD_COLS + scanData.col];
         if (scanData.data != bCell){
             boardBuffer[scanData.row * BOARD_COLS + scanData.col] = scanData.data;
             drawCell(scanData.row, scanData.col, scanData.data, BOARD_OFFSET_ROW, BOARD_OFFSET_COL);
         }
    }
    
    freeGenerator(&boardHandle);
    
    move(0, 27);
}

void renderDebug(const char* message) {
    drawText(message, DEBUG_OFFSET_ROW, DEBUG_OFFSET_COL);
    refresh();
}

void renderClearScreen() {
    move(10, 20);
    clear();
    refresh();
}

void renderCleanup() {
    //endwin();
}

void renderInit() {

    keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_BLACK, COLOR_BLACK);


    renderClearScreen();
    drawText("SCORE", SCORE_OFFSET_ROW, SCORE_OFFSET_COL);
    drawText("LEVEL", LEVEL_OFFSET_ROW, LEVEL_OFFSET_COL);
    refresh();
    
    int i;
    for(i = 0; i < BOARD_SIZE; i++) {
        boardBuffer[i] = 0x00;
    }
}

void renderScreen(GameStatus status) {   
    renderNextPiece(status.nextPieceData);
    gameBoard();
    drawInteger(status.score, SCORE_OFFSET_ROW + 1, SCORE_OFFSET_COL);
    drawInteger(status.level, LEVEL_OFFSET_ROW + 1, LEVEL_OFFSET_COL);
    drawText(status.gameover ? "GAME OVER" : NULL, GAMEOVER_OFFSET_ROW, GAMEOVER_OFFSET_COL);
    refresh();
}