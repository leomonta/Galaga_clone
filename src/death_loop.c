#include "death_loop.h"

#include "game_loop.h"

#include <raylib.h>

void death_loop(bool *game_closed) {

	BeginDrawing();

	ClearBackground(BLACK);
	DrawText("Game Over", 200, 400, 30, WHITE);
	DrawText("Press Q to exit", 250, 440, 20, WHITE);
	DrawText("Press Enter to retry", 250, 470, 20, WHITE);

	EndDrawing();

	if (IsKeyPressed(KEY_ENTER)) {
		reset_game();
		return;
	}

	if (IsKeyPressed(KEY_Q)) {
		*game_closed = true;
		return;
	}
}
