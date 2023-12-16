#pragma once

#include "constants.h"

#include <raylib.h>

// Runtime values that are gonna change
typedef struct gameState {
	// spaceship stats
	long    fireCoolDown; // ms cooldown
	long    lastShot;     // the time which the last shot was fired form the ship
	Vector2 spaceship_box;
	Vector2 upgrade_box;
	float   enemy_spawn_rate; // 10 % base spawn rate per-frame
	int     spaceship_speed;
	int     spaceship_health;
	int     spaceship_num_bullets;
	int     spaceship_Maxspeed;
	int     e_fireCoolDown;

	// 0 pacman, 1 speed, 2 bullet
	int upgrade_type;

	int  score;
	bool upgr_PacmanEffect;
	bool close; // should close the program
	bool pause; // should pause the game
} gameState;

void    moveBullets();
void    moveEnemies();
void    checkBulletsCollision();
void    checkEntitiesCollisions();
void    enemyAI();
void    physics();
void    fireBullets();
Vector2 intersection(Vector4 line1, Vector4 line2);
void    renderStars();
void    moveStars();
long    getCurrMs();
void    pickRandomUpgrade(int enemyIndex);
bool    bulletEnemyCollision(int bulletIndex, int enemyIndex);

// drawing loops
void gameInputs();
