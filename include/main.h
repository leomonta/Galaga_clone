#pragma once

#include "constants.h"

#include <raylib.h>

// Runtime values that are gonna change
typedef struct game_state {
	// spaceship stats
	long    fire_cool_down; // ms cooldown
	long    last_shot;     // the time which the last shot was fired form the ship
	Vector2 spaceship_box;
	Vector2 upgrade_box;
	float   enemy_spawn_rate; // 10 % base spawn rate per-frame
	int     spaceship_speed;
	int     spaceship_health;
	int     spaceship_num_bullets;
	int     spaceship_maxspeed;
	int     e_fire_cool_down;

	// 0 pacman, 1 speed, 2 bullet
	int upgrade_type;

	int  score;
	bool upgr_pacman_effect;
	bool close; // should close the program
	bool pause; // should pause the game
} gameState;

void    move_enemies();
void    check_bullets_collision();
void    check_entities_collisions();
void    enemy_AI();
void    physics();
void    fire_bullets();
Vector2 intersection(Vector4 line1, Vector4 line2);
void    move_stars();
void    pick_random_upgrade(int enemy_index);
bool    bullet_enemy_collision(int bullet_index, int enemy_index);

// drawing loops
void game_inputs();
