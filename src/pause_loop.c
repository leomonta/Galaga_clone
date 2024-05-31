#include "pause_loop.h"

#include <raylib.h>
#include "constants.h"

void pause_loop(bool *game_closed) {

	BeginDrawing();

	// screen opacity
	DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 128});

	// pause message
	DrawText("Game Paused", 200, 400, 30, WHITE);
	DrawText("Press Esc or P to unpause", 250, 440, 20, WHITE);
	DrawText("Press Q to close", 250, 470, 20, WHITE);

	EndDrawing();

	while (true) {

		if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
			*game_closed = true;
			break;
		}

		// Unpause Button
		if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
			break;
		}

		PollInputEvents();
	}

	//reset_frametime();

	// get rid of the lingering <Esc> / <P> pressed
	PollInputEvents();

	// allow correct recalculation of the frame time
	EndDrawing();
	EndDrawing();
}
