#ifndef __input_h_
#define __input_h_

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

#ifdef __cplusplus
extern "C" {
#endif


void resetDropInterval(interval_t interval);
void initInput();

void renderClearScreen();
void renderInit();

// test
void renderScreen(GameStatus* status);
void renderDebug(const char* message);

#ifdef __cplusplus
}
#endif


#endif