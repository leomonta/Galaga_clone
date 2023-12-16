#pragma once
#include "main.h"
/**
 * Run the main game Loop as a function
 */
void gameLoop(gameState *runtime, Vector4 *bullets, Vector4 *enemiesBullets,  Vector2 *enemies, int *enemiesHealth, RenderTexture2D *frameBuffer, Texture *spaceship_sprite, Texture *Enemyship_sprite, Texture *Upgrades, Shader *bloomShader, const gameState *default_stat);

/**
 * Run the pause screen loop as a function
 */
void pauseLoop(gameState *runtime);

/**
 * Run the death screen loop as a function
 */
void deathLoop(gameState *runtime, Vector4 *bullets, Vector4 *enemiesBullets, char *enemiesFireCooldown, int *enemiesHealth, Vector2 *enemies, const gameState *default_stat);
