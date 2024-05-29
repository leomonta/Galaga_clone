#include "utils.h"

#include "constants.h"

#include <raylib.h>

static bool skip_frametime = false;

void spawn_random_enemies(Vector2 *enemies, int *enemiesHealth) {
	add_enemies((Vector2){
	                (float)(GetRandomValue(0, screen_width - spaceship_height)),
	                (float)(GetRandomValue(0, 40))},
	            enemies, enemiesHealth);
}

void add_enemies(const Vector2 coords, Vector2 *enemies, int *enemiesHealth) {

	for (int i = 0; i < MAX_ENEMY; ++i) {

		if (enemies[i].x == -40 && enemies[i].y == -40) {
			enemies[i]       = coords;
			enemiesHealth[i] = 5;
			return;
		}
	}
}

void add_bullet(const Vector4 bullet, const int n, Vector4 *bullets, Vector4 *enemiesBullets) {

	if (n == -1) {
		for (int i = 0; i < MAX_BULLETS; ++i) {

			if (bullets[i].x == -1 && bullets[i].y == -1) {
				bullets[i] = bullet;
				return;
			}
		}
	} else {

		if (enemiesBullets[n].x == -1 && enemiesBullets[n].y == -1) {
			enemiesBullets[n] = bullet;
			return;
		}
	}
}

void reset_arrays(Vector4 *bullets, Vector4 *enemiesBullets, char *enemiesFireCooldown, int *enemiesHealth, Vector2 *enemies) {

	for (int i = 0; i < MAX_BULLETS; ++i) {

		bullets[i]        = default_bullet;
		enemiesBullets[i] = default_bullet;
	}

	for (int i = 0; i < MAX_ENEMY; ++i) {

		enemiesHealth[i]       = 0;
		enemiesFireCooldown[i] = -1;
		enemies[i]             = default_ship_pos;
	}
}

long get_curr_ms() {

	return (long)(GetTime() * 1000);
}

void reset_frametime() {
	skip_frametime = true;
}

float get_frametime() {
	if (skip_frametime) {
		skip_frametime = false;
		GetFrameTime();
		return 0;
	}
	return GetFrameTime();
}
