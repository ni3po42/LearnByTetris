#include "Arduino.h"
#include "core.h"
#include "io.h"
#include "board.h"
#include "piece.h"
#include "events.h"

#include "xboxcontroller.h"

/**
 * Private APIs
*/
#ifdef __cplusplus
extern "C" {
#endif

static Board boardBuffer;
static int nextPieceId = -1;
static XboxController controller;

static void drawText(const char* text, int row, int col) {
    // if (text == NULL) {
    //     return;
    // }
    // mvprintw(row, col, text);
}

static void drawCell(int row, int col, CellData data, int rowOffset, int colOffset) {
    
    // int color = (data & COLOR_MASK);
    
    // if (color != 0) {
    //     attron(COLOR_PAIR(color));
    //     attron(A_REVERSE);
    // } else {
    //     attron(COLOR_PAIR(8));
    // }
    
    // mvprintw(row + rowOffset, (col + colOffset) * 2, "  ");
    
    // if (color != 0) {
    //     attroff(A_REVERSE);
    //     attroff(COLOR_PAIR(color));
    // } else {
    //     attroff(COLOR_PAIR(8));
    // }
     
}

static void drawInteger(int value, int row, int col) {
    // char str[10];
    // sprintf(str, "%d", value);
    // drawText(str, row, col);
}

static void renderNextPiece_erase(int row, int col, CellData unsued) {
    //drawCell(row, col, EMPTY_CELL_DATA, NEXTPIECE_OFFSET_ROW, NEXTPIECE_OFFSET_COL);
}

static void renderNextPiece_draw(int row, int col, CellData data) {
    //drawCell(row, col, data, NEXTPIECE_OFFSET_ROW, NEXTPIECE_OFFSET_COL);
}

static void renderNextPiece(Piece* nextPieceData) {
    
    // if (nextPieceId != getPieceId(nextPieceData.nextPiece)) {
    //     nextPieceId = getPieceId(nextPieceData.nextPiece);
        
    //     pieceScan(nextPieceData.currentPiece, true, renderNextPiece_erase);
    //     pieceScan(nextPieceData.nextPiece, true, renderNextPiece_draw);    
    // }
}

static void debugMessage(const char* message) {
    Serial.write(message);
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
             //drawCell(scanData.row, scanData.col, scanData.data, BOARD_OFFSET_ROW, BOARD_OFFSET_COL);
         }
    }

    int index;
    for(index= 0;index<BOARD_SIZE; index++) {
        if (index % BOARD_COLS == 0) {            
            debugMessage("\n");
        }
        
        if ((boardBuffer[index] & (ACTIVE_MASK | STATIC_MASK)) == (ACTIVE_MASK | STATIC_MASK) ) {
            debugMessage("C");
        } else if ((boardBuffer[index] & ACTIVE_MASK) == ACTIVE_MASK) {
            debugMessage("A");
        } else if ((boardBuffer[index] & STATIC_MASK) == STATIC_MASK) {
            debugMessage("S");
        } else {
            debugMessage(" ");
        }

    }
    debugMessage("\n");
    debugMessage("\n");
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
    controller.listen(handleXboxStateUpdate);
    controller.start();

    while(!controller.isConnected()){
        controller.reconnect();
        delay(500);
    }

    while (controller.isWaiting()) {
        delay(500);
    }    
}


void resetDropInterval(interval_t interval) {
   
}



void renderDebug(const char* message) {
    debugMessage(message);
    // drawText(message, DEBUG_OFFSET_ROW, DEBUG_OFFSET_COL);
    // refresh();
}

void renderClearScreen() {
    // move(10, 20);
    // clear();
    // refresh();
}

void renderInit() {
    Serial.begin(115200);
    // TickType_t shortDelay = 1;

    // pinMode(32, OUTPUT);
    // bool high = false;
    // for(;;) {
    //     vTaskDelay(shortDelay);
    //     if (high) {
    //         high = false;
    //         REG_SET_BIT(GPIO_OUT_W1TC_REG, GPIO_PIN_REG_32);
    //         //GPIO_OUT_W1TC_REG
    //         //digitalWrite(32, HIGH);
    //     } else {
    //         high = true;
    //         REG_CLR_BIT(GPIO_OUT_W1TC_REG, GPIO_PIN_REG_32);
    //         //digitalWrite(32, LOW);
    //     }
    // }

    // keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	// noecho();			/* Don't echo() while we do getch */

    // start_color();

    // init_pair(1, COLOR_RED, COLOR_BLACK);
    // init_pair(2, COLOR_GREEN, COLOR_BLACK);
    // init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    // init_pair(4, COLOR_BLUE, COLOR_BLACK);
    // init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    // init_pair(6, COLOR_CYAN, COLOR_BLACK);
    // init_pair(7, COLOR_WHITE, COLOR_BLACK);
    // init_pair(8, COLOR_BLACK, COLOR_BLACK);


    // renderClearScreen();
    // drawText("SCORE", SCORE_OFFSET_ROW, SCORE_OFFSET_COL);
    // drawText("LEVEL", LEVEL_OFFSET_ROW, LEVEL_OFFSET_COL);
    // refresh();
    
    // int i;
    // for(i = 0; i < BOARD_SIZE; i++) {
    //     boardBuffer[i] = 0x00;
    // }
}

void renderScreen(GameStatus* status) {
    
    // renderNextPiece(status.nextPieceData);
    gameBoard();
    // drawInteger(status.score, SCORE_OFFSET_ROW + 1, SCORE_OFFSET_COL);
    // drawInteger(status.level, LEVEL_OFFSET_ROW + 1, LEVEL_OFFSET_COL);
    // drawText(status.gameover ? "GAME OVER" : NULL, GAMEOVER_OFFSET_ROW, GAMEOVER_OFFSET_COL);
    // refresh();
}

#ifdef __cplusplus
}
#endif

