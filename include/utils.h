#pragma once

#include <raylib.h>

/**
 * Quick shortcut to spawn a random enemy in the top of the map
 */
void spawn_random_enemies(Vector2 *enemies, int *enemiesHealth);

/**
 * Add the given enemy in the first spot available and set its health
 */
void add_enemies(const Vector2 coords, Vector2 *enemies, int *enemiesHealth);

/**
 * Add the given bullet in the first spot available
 * n is the index of the enemy who shoot it, -1 if it was shot by the main spaceship
 */
void add_bullet(const Vector4 bullet, const int n, Vector4 *bullets, Vector4 *enemiesBullets);

/**
 * Set the initial value for all the used arrays
 */
void reset_arrays(Vector4 *bullets, Vector4 *enemiesBullets, char *enemiesFireCooldown, int *enemiesHealth, Vector2 *enemies);

/**
 * Return the time in ms since the start of the window
 */
long get_curr_ms();
