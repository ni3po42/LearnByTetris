#ifndef _render_h_
#define _render_h_

#include "../game/game.h"

void renderClearScreen();
void renderInit();
void renderCleanup();
void renderScreen(GameStatus status);

void renderDebug(const char* message);

#endif