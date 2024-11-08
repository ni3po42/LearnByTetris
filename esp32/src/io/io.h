#ifndef __input_h_
#define __input_h_

#define SCORE_OFFSET_ROW 7
#define SCORE_OFFSET_COL 1

#define LEVEL_OFFSET_ROW 10
#define LEVEL_OFFSET_COL 1

#define GAMEOVER_OFFSET_ROW 10
#define GAMEOVER_OFFSET_COL 12

#define BOARD_OFFSET_ROW 4
#define BOARD_OFFSET_COL 14

#define NEXTPIECE_OFFSET_ROW 4
#define NEXTPIECE_OFFSET_COL 2

#define DEBUG_OFFSET_ROW 0
#define DEBUG_OFFSET_COL 0

#ifdef __cplusplus
extern "C" {
#endif


void resetDropInterval(interval_t interval);
void initInput();

void renderClearScreen();
void renderInit();

void renderScreen(GameStatus* status);
void renderDebug(const char* message);

#ifdef __cplusplus
}
#endif


#endif