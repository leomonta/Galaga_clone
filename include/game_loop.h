#pragma once

#include <raylib.h>

/**
 * Initializes all of the dynamic data useful at runtime
 */
void init_runtime_info();

/**
 * Run the main game Loop as a function
 */
void game_loop();

/**
 * Move, if they exist, spaceship bullets and enemy bullets
 */
void move_bullets();

/**
 * reset all of the game state to the initial
 */
void reset_game();

/**
 * Listens for inputs from keyboards
 */
void spaceship_inputs();

/**
 * Move entities, remove them and check the interaction between them
 */
void physics();

/**
 * move the enemies down the screen
 */
void move_enemies();

/**
 * Check collision between bullets, enemies and spaceship. the decrease health/spawn upgrades
 */
void check_bullets_collision();

/**
 * Check collisions between spaceship and upgrades, and spaceships with enemies
 */
void check_entities_collisions();

/**
 * every enemy shoot at the spaceship randomly,
 */
void enemy_AI();

/**
 * fire the current amount of simultaneous bullets spread out as a 'fan'
 */
void fire_bullets();

/**
 * Shorthand to check for hitting an enemy
 */
bool bullet_enemy_collision(int bulletIndex, int enemyIndex);

/**
 * Return the point of intersection between two line defined by two points
 */
Vector2 intersection(Vector4 line1, Vector4 line2);

/**
 * RAndompli choose and placee an upgrade in the field
 */
void pickRandomUpgrade(int enemyIndex);

/**
 * @return if the main spaceship is dead
 */
bool is_main_dead();
