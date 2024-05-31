/**
 * Galaga clone by Leonardo Montagner https://github.com/Vectoryx
 *
 * Doing this as a real practice in game developing
 *
 * TODO vfx
 * TODO fix enemy firing
 * TODO add stars
 **/

#include "main.h"

#include "constants.h"
#include "death_loop.h"
#include "game_loop.h"
#include "graphics.h"
#include "notifications.h"
#include "pause_loop.h"

#include <raylib.h>

int main() {
	// ------------------------------------------------------------------------------------- Initialization

	// Initial preparations
	InitWindow(screen_width, screen_height, "Galaga clone by Leonardo");

	initialize_graphics();

	HideCursor();

	scatter_stars();

	reset_game();
	SetTargetFPS(FPS); // Set our game to run at given frames-per-second
	SetExitKey(KEY_NULL);
	//-------------------------------------------------------------------------------------- End initialization

	bool game_should_close = false;
	// repeat loop
	while (!game_should_close) {

		notif__tick();

		if (WindowShouldClose()) {
			break;
		}

		// Pause Button
		if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
			pause_loop(&game_should_close);
			continue;
		}

		// else !runtime.pause

		// Main game loop
		if (!is_main_dead()) { // Detect window close button or ESC key
			game_loop();

		} else {
			death_loop(&game_should_close);
		}
	}

	//-------------------------------------------------------------------------------------- De-Initialization

	CloseWindow(); // Close window and OpenGL context

	return 0;
	//-------------------------------------------------------------------------------------- End de-Initialization
}
