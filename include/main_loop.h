#pragma once
#include "main.h"

/**
 * Initializes all of the dynamic data useful at runtime
 */
void init_runtime_info();

/**
 * Run the main game Loop as a function
 */
void game_loop(gameState *runtime, Vector4 *bullets, Vector4 *enemiesBullets,  Vector2 *enemies, int *enemiesHealth, const gameState *default_stat);

/**
 * Move, if they exist, spaceship bullets and enemy bullets
 */
void move_bullets();
