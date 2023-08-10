#pragma once

#define UPGRADE_PACMAN 0
#define UPGRADE_SPEED  1
#define UPGRADE_BULLET 2

#define MAX_SPEED  16
#define MAX_BULLET 8

#define MAX_BULLETS       1000
#define MAX_NOTIFICATIONS 10
#define MAX_ENEMY         500
#define MAX_STAR          300
#define MAX_LENGHT_NAME   6

#define BULLET_SPAWN_WEIGHT 0.5f
#define PACMAN_SPAWN_WEIGHT 2.0f
#define SPEED_SPAWN_WEIGHT  0.7f

#define MOVEMENT_SPEED_MULT  15.f
#define ENEMY_SPEED_MULT     10.f
#define STAR_SPEED_MULT      2.f
#define BULLET_SPEED_MULT    20.f
#define DEFAULT_BULLET_SPEED -12.f

#define SCREEN_BG \
	(Color) { 0, 0, 40, 255 }
#define DefaultBullet \
	(Vector4) { -1, -1, 0, 0 }
#define DefaultShipPos \
	(Vector2) { -40, -40}
#define DefaultStar \
	(Vector4) { -1, -1, -1, -1 }

#define spaceship_width  30
#define spaceship_height 30
#define screenWidth      800
#define screenHeight     1000