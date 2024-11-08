#include "Arduino.h"
#include "core.h"
#include "io.h"
#include "board.h"
#include "piece.h"
#include "events.h"
#include "vga.h"

#include "xboxcontroller.h"

/**
 * Private APIs
*/
#ifdef __cplusplus
extern "C" {
#endif

static Board boardBuffer;
static XboxController controller;

static void drawText(const char* text, int row, int col) {
    if (text == NULL) {
        return;
    }
    renderMessage(text, row * 10, col * 10, 0x07, 0x08);
}

static void drawCell(int row, int col, CellData data, int rowOffset, int colOffset) {    
    uint8_t color = (data & COLOR_MASK);    
    renderMessage("@", (row+rowOffset) * 8, (col + colOffset) * 8, color, 0x08);
     
}

static void drawInteger(int value, int row, int col) {
    char str[10];
    sprintf(str, "%d", value);
    renderMessage(str, row*10, col*10, 0x07, 0x08);
}

static void renderNextPiece_erase(int row, int col, CellData unsued) {
    drawCell(row, col, EMPTY_CELL_DATA, NEXTPIECE_OFFSET_ROW, NEXTPIECE_OFFSET_COL);
}

static void renderNextPiece_draw(int row, int col, CellData data) {
    drawCell(row, col, data, NEXTPIECE_OFFSET_ROW, NEXTPIECE_OFFSET_COL);
}

static void renderNextPiece() {
    
    Piece* nextPiece = getNextPiece();
    Piece* currentPiece = getCurrentPiece();

    if (currentPiece) {
        pieceScan(currentPiece, true, renderNextPiece_erase);        
    }

    if (nextPiece) {        
        pieceScan(nextPiece, true, renderNextPiece_draw);    
    }
}

static void debugMessage(const char* message) {
    renderMessage(message, 310, 0, 0x07, 0x08);
}

static void gameBoard() {
   
    BoardScanArguments scanArgs = {
        .row = 0, .height = BOARD_ROWS, .col = 0, .width = BOARD_COLS, .start = true
    };
    
    BoardScanData scanData;
        
    while(scanBoard(&scanArgs, &scanData)) {
    
         int bCell = boardBuffer[scanData.row * BOARD_COLS + scanData.col];
         if (scanData.data != bCell){
             boardBuffer[scanData.row * BOARD_COLS + scanData.col] = scanData.data;
             drawCell(scanData.row, scanData.col, scanData.data, BOARD_OFFSET_ROW, BOARD_OFFSET_COL);
         }
    }
}

static bool btnA = false;
static bool btnB = false;
static bool btnDown = false;
static bool btnLeft = false;
static bool btnRight = false;

void handleXboxStateUpdate(const XboxState& state) {
    
    if (state.A && !btnA) {
        emit(TURN_RIGHT);
        btnA = true;
    } else if (!state.A) {
        btnA = false;
    }

    if (state.B && !btnB) {
        emit(TURN_LEFT);
        btnB = true;
    } else if (!state.B) {
        btnB = false;
    }

    if (state.Down && !btnDown) {
        emit(DROP);
        btnDown = true;
    } else if (!state.Down) {
        btnDown = false;
    }

    if (state.Left && !btnLeft) {
        emit(LEFT);
        btnLeft = true;
    } else if (!state.Left) {
        btnLeft = false;
    }

    if (state.Right && !btnRight) {
        emit(RIGHT);
        btnRight = true;
    } else if (!state.Right) {
        btnRight = false;
    }

}

/** 
 * Public APIs
*/

void initInput() {    
    renderMessage("HERE1", 20, 1, 0x07, 0x00);
    controller.listen(handleXboxStateUpdate);
    renderMessage("HERE2", 20, 1, 0x07, 0x00);
    controller.start();
    renderMessage("HERE3", 20, 1, 0x07, 0x00);
   
    while(!controller.isConnected()){
        controller.reconnect();
        delay(500);
        // sprintf(buffer, "%d", c);
        
        // renderMessage(buffer, 14, 2, 0x07, 0x00);
       
   
    }

    while (controller.isWaiting()) {
        delay(500);
    }    
}


void resetDropInterval(interval_t interval) {
   //??
}



void renderDebug(const char* message) {    
    drawText(message, DEBUG_OFFSET_ROW, DEBUG_OFFSET_COL);
     if (message == NULL) {
        return;
    }
    renderMessage(message, DEBUG_OFFSET_ROW * 10, DEBUG_OFFSET_COL * 10, 0x07, 0x00);
}


void renderClearScreen() {    
    clearVRAM();
}

void renderInit() {
    vga_init();
    
    renderClearScreen();
    drawText("SCORE", SCORE_OFFSET_ROW, SCORE_OFFSET_COL);
    drawText("LEVEL", LEVEL_OFFSET_ROW, LEVEL_OFFSET_COL);
        
    int i;
    for(i = 0; i < BOARD_SIZE; i++) {
        boardBuffer[i] = 0x00;
    }
}

void renderScreen(GameStatus* status) {
    
    renderNextPiece();
    gameBoard();
    drawInteger(status->score, SCORE_OFFSET_ROW + 1, SCORE_OFFSET_COL);
    drawInteger(status->level, LEVEL_OFFSET_ROW + 1, LEVEL_OFFSET_COL);
    if (status->gameover) {
        renderMessage("GAME OVER", GAMEOVER_OFFSET_ROW * 10, GAMEOVER_OFFSET_COL * 10 - 4, 0x01, 0x04);
    }
    
}

#ifdef __cplusplus
}
#endif

