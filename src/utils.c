#include "utils.h"

#include "constants.h"

void spawnRandomEnemies(Vector2 *enemies, int *enemiesHealth) {
	addEnemies((Vector2){
	               (float)(GetRandomValue(0, screenWidth - spaceship_height)),
	               (float)(GetRandomValue(0, 40))},
	           enemies, enemiesHealth);
}

void addEnemies(const Vector2 coords, Vector2 *enemies, int *enemiesHealth) {

	for (int i = 0; i < MAX_ENEMY; ++i) {

		if (enemies[i].x == -40 && enemies[i].y == -40) {
			enemies[i]       = coords;
			enemiesHealth[i] = 5;
			return;
		}
	}
}

void addBullet(const Vector4 bullet, const int n, Vector4 *bullets, Vector4 *enemiesBullets) {

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

void resetArrays(Vector4 *bullets, Vector4 *enemiesBullets, char *enemiesFireCooldown, int *enemiesHealth, Vector2 *enemies) {

	for (int i = 0; i < MAX_BULLETS; ++i) {

		bullets[i]        = DefaultBullet;
		enemiesBullets[i] = DefaultBullet;
	}

	for (int i = 0; i < MAX_ENEMY; ++i) {

		enemiesHealth[i]       = 0;
		enemiesFireCooldown[i] = -1;
		enemies[i]             = DefaultShipPos;
	}
}

long getCurrMs() {

	return (long)(GetTime() * 1000);
}
