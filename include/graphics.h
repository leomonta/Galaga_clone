#pragma once

#include <raylib.h>

typedef enum : char {
	NEITHER,
	ENEMIES_ONLY,
	MAIN_ONLY,
	BOTH
} hitboxmask;

/**
 * Loads the textures and the files needed for the game
 *
 * @return false if anything fails, true otherwise
 */
bool initialize_graphics();

/**
 * Unloads everything loaded by the init function
 *
 * @return false if anything fails, true otherwise
 */
bool terminate_graphics();

/**
 * fill the stars array with random stars in random positions at random speed
 */
void scatter_stars();

/**
 * Draw the right texture for each star
 */
void draw_stars();

/**
 * move the stars by their own speed
 */
void move_stars();

/**
 * Teleport the spaceship at the opposite side of the screen
 *
 * @param pos the current position of the spaceship, needed to know if it's needed to draw the pacman effect or not
 */
void draw_pacman(const Vector2 *pos);

/**
 * Render all active bullets
 *
 * @param bullerts the player bullers
 * @param enemiesbullets all of the enmies bullets
 */
void draw_bullets(const Vector4 *bullets, const Vector4 *enemiesbullets);

/**
 * Render all of the ships, and (if asked their hitbox)
 *
 * @param mainship the player spaceships
 * @param enemyships an array containing all of the enemy spaceships
 * @param hitboxes wheather or not to draw hitboxes fot the spaceships
 */
void draw_ships(const Vector2 *mainship, const Vector2 *enemyships, const hitboxmask hitboxes);
