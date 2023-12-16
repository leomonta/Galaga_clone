#pragma once

#include <raylib.h>

/**
 * fill the stars array with random stars in random positions at random speed
 */
void fillStars();

/**
 * Draw the right texture for each star
 */
void renderStars();

/**
 * move the stars by their own speed
 */
void moveStars();

/**
 * Teleport the spaceship at the opposite side of the screen
 */
void pacmanEffect(Vector2 *pos);

/**
 * Render all active bullets
 */
void renderBullets(Vector4 *bullets, Vector4 *enemiesBullets);
