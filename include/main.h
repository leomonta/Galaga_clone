#pragma once

#include "constants.h"

#include <raylib.h>

// Runtime values that are gonna change
typedef struct RuntimeVals {
	// spaceship stats
	Rectangle spaceship_box;
	int       spaceship_speed;
	int       spaceship_health;
	int       spaceship_num_bullets;
	int       spaceship_Maxspeed;
	long      fireCoolDown; // ms cooldown
	long      lastShot;     // the time which the last shot was fired form the ship
	int       e_fireCoolDown;
	bool      upgr_PacmanEffect;

	Rectangle upgrade_box;
	float     enemy_spawn_rate; // 10 % base spawn rate per-frame
	// 0 pacman, 1 speed, 2 bullet
	int upgrade_type;

	int  score;
	bool close; // should close the program
	bool pause; // should pause the game
} RuntimeVals;

const RuntimeVals default_stat = {
    {400, 800, 31, 31},
    4,
    10,
    1,
    4,
    200,
    0,
    10,
    false,
    DefaultShip,
    1,
    0,
    0,
    false,
    false
};

RuntimeVals runtime;

void    spawnEnemies();
void    addEnemies(Rectangle coords);
void    addBullet(Vector4 bullet, int n);
void    renderBullets();
void    resetArrays();
void    resetStats();
void    moveBullets();
void    moveEnemies();
void    checkBulletsCollision();
void    checkEntitiesCollisions();
void    enemyAI();
void    physics();
void    fireBullets();
void    pacmanEffect(Texture sprite);
Vector2 intersection(Vector4 line1, Vector4 line2);
void    fillStars();
void    randomStar(int index);
void    renderStars();
void    moveStars();
long    getCurrMs();
void    pickRandomUpgrade(int enemyIndex);
bool    bulletEnemyCollision(int bulletIndex, int enemyIndex);