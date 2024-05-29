#include "loops.h"

#include "constants.h"
#include "graphics.h"
#include "notifications.h"
#include "utils.h"

#include <raylib.h>

void game_loop(gameState *runtime, Vector4 *bullets, Vector4 *enemiesBullets, Vector2 *enemies, int *enemiesHealth, RenderTexture2D *frameBuffer, Texture *spaceship_sprite, Texture *Enemyship_sprite, Texture *Upgrades, Shader *bloomShader, const gameState *default_stat) {

	if (WindowShouldClose()) {
		runtime->close = true;
		return;
	}

	// Pause Button
	if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
		runtime->pause = true;
		return;
	}

	game_inputs();

	// limit the speed to the maximum possible as base
	runtime->spaceship_speed = runtime->spaceship_maxspeed;

	// more upgrade, more enemies
	float spawn = (float)(GetRandomValue(0, 30000));
	if (spawn < runtime->enemy_spawn_rate) {
		spawn_random_enemies(enemies, enemiesHealth);
		runtime->enemy_spawn_rate = default_stat->enemy_spawn_rate;

	} else {
		// increase the probablity of spawning if enemy did not spawn
		float inc = (float)(runtime->spaceship_num_bullets) * BULLET_SPAWN_WEIGHT;
		inc += (float)(runtime->spaceship_maxspeed - default_stat->spaceship_maxspeed) * SPEED_SPAWN_WEIGHT;
		inc += (float)(runtime->upgr_pacman_effect) * PACMAN_SPAWN_WEIGHT;

		runtime->enemy_spawn_rate += inc;
	}

	// manage objects movement and interactions
	physics();

	// check if the spaceship has died
	if (runtime->spaceship_health <= 0) {
		return;
	}

	// --------------------------------------------------------------------------------- Draw

	// render to framebuffer
	BeginTextureMode(*frameBuffer);
	{
		// draw screen
		ClearBackground(SCREEN_BG);

		draw_stars();

		draw_pacman(&runtime->spaceship_box);
		if (runtime->spaceship_box.x < 0) {
			runtime->spaceship_box.x = 0;
		}

		if (runtime->spaceship_box.x + spaceship_width > screen_width) {
			runtime->spaceship_box.x = screen_width - spaceship_width;
		}

		if (runtime->spaceship_box.y < 0) {
			runtime->spaceship_box.y = 0;
		}

		if (runtime->spaceship_box.y + spaceship_height > screen_height) {
			runtime->spaceship_box.y = screen_height - spaceship_height;
		}

		// draw objects
		DrawTextureV(*spaceship_sprite, (Vector2){runtime->spaceship_box.x, runtime->spaceship_box.y}, WHITE);
		for (int i = 0; i < MAX_ENEMY; ++i) {

			if (enemies[i].x != -40 && enemies[i].y != -40) {
				DrawTextureV(*Enemyship_sprite, (Vector2){enemies[i].x, enemies[i].y}, WHITE);
			}
		}

		// draw the correct upgrade
		DrawTexture(Upgrades[runtime->upgrade_type], (int)(runtime->upgrade_box.x), (int)(runtime->upgrade_box.y), WHITE);

		draw_bullets(bullets, enemiesBullets);
	}
	EndTextureMode();

	BeginDrawing();
	{
		BeginShaderMode(*bloomShader);
		{
			DrawTextureRec(frameBuffer->texture, (Rectangle){0.f, 0.f, (float)(frameBuffer->texture.width), (float)(-frameBuffer->texture.height)}, (Vector2){0.f, 0.f}, WHITE);
		}
		EndShaderMode();
		// Draw spaceship stats on the screen
		/*
		std::string text = "Bullets: " + std::to_string(runtime.spaceship_num_bullets);
		text += "\n Speed: " + std::to_string((int)(runtime.spaceship_Maxspeed));
		text += "\n Health: " + std::to_string(runtime.spaceship_health);
		text += "\n Score: " + std::to_string(runtime.score);
		DrawTextEx(Consolas, text.c_str(), {10, 10}, 20, 1, {255, 255, 255, 150});
		*/

		DrawFPS(200, 10);

		notif__render_notifications();
	}
	EndDrawing();
	//---------------------------------------------------------------------------------- End draw
}

void pause_loop(gameState *runtime) {

	BeginDrawing();

	// screen opacity
	DrawRectangle(0, 0, screen_width, screen_height, (Color){0, 0, 0, 128});

	// pause message
	DrawText("Game Paused", 200, 400, 30, WHITE);
	DrawText("Press Esc or P to unpause", 250, 440, 20, WHITE);
	DrawText("Press Q to close", 250, 470, 20, WHITE);

	EndDrawing();

	while (true) {

		// poll inputs and keep the frame time usable
		// without this the moment the pause is unpaused the frame time would be equal to all of the time that the game was paused
		// leading to very big movements
		BeginDrawing();
		EndDrawing();

		if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
			runtime->close = true;
			break;
		}

		// Unpause Button
		if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
			runtime->pause = false;
			break;
		}
	}

	// get rid of the lingering <Esc> / <P> pressed
	PollInputEvents();
}

/**
 * black screen and user input
 */
void death_loop(gameState *runtime, Vector4 *bullets, Vector4 *enemiesBullets, char *enemiesFireCooldown, int *enemiesHealth, Vector2 *enemies, const gameState *default_stat) {

	BeginDrawing();

	ClearBackground(BLACK);
	DrawText("Game Over", 200, 400, 30, WHITE);
	DrawText("Press Q to exit", 250, 440, 20, WHITE);
	DrawText("Press Enter to retry", 250, 470, 20, WHITE);

	EndDrawing();

	if (IsKeyPressed(KEY_ENTER)) {
		reset_arrays(bullets, enemiesBullets, enemiesFireCooldown, enemiesHealth, enemies);

		*runtime = *default_stat;
		return;
	}

	if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
		runtime->close = true;
		// save();
		return;
	}
}
