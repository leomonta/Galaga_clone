#pragma once

#include "constants.h"

#include <raylib.h>

// Runtime values that are gonna change
typedef struct RuntimeVals {
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
	bool    upgr_PacmanEffect;
	bool close; // should close the program
	bool pause; // should pause the game
} RuntimeVals;

const RuntimeVals default_stat = {
    200,
    0,
    {400, 800},
    DefaultShipPos,
    1,
    6,
    10,
    1,
    6,
    10,
    0,
    0,
    false,
    false,
    false
};

RuntimeVals runtime;

void    renderBullets();
void    moveBullets();
void    moveEnemies();
void    checkBulletsCollision();
void    checkEntitiesCollisions();
void    enemyAI();
void    physics();
void    fireBullets();
void    pacmanEffect(Texture sprite);
Vector2 intersection(Vector4 line1, Vector4 line2);
void    renderStars();
void    moveStars();
long    getCurrMs();
void    pickRandomUpgrade(int enemyIndex);
bool    bulletEnemyCollision(int bulletIndex, int enemyIndex);

// drawing loops
void gameLoop();
void gameInputs();

void pauseLoop();
void deathLoop();

// The fist two values represent the coordinates, the latter two the accelleration
Vector4 bullets[MAX_BULLETS];
Vector4 e_bullets[MAX_BULLETS];

// The coordinates of all the enemies
Vector2 enemies[MAX_ENEMY];

// other enemies stats
char e_coolDown[MAX_ENEMY];
int  e_health[MAX_ENEMY];

// x, y, z,
// x, y, speed
// the type is deducted from the last number of the x coord
// type = x % 3
Vector3 stars[MAX_STAR];

int             fps = 60;
Texture         spaceship_sprite;
Texture         Enemyship_sprite;
Texture         Upgrades[3];
Texture         Star_ATL;
RenderTexture2D frameBuffer;
Shader          bloomShader;
Font            Consolas;