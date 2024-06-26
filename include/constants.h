#pragma once

#define UPGRADE_PACMAN 0
#define UPGRADE_SPEED  1
#define UPGRADE_BULLET 2

#define MAX_SPEED_UPGRADE  16
#define MAX_BULLET_UPGRADE 8

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

#define MAIN_FIRE_COOLDOWN 200
#define ENEMY_FIRE_COOLDOWN 50
#define ENEMY_SPAWN_RATE 1

#define SCREEN_BG \
	(Color) { 0, 0, 40, 255 }
#define default_bullet \
	(Vector4) { -1, -1, 0, 0 }
#define default_ship_pos \
	(Vector2) { -40, -40}
#define sefault_star \
	(Vector4) { -1, -1, -1, -1 }

#define spaceship_width  30
#define spaceship_height 30
#define screen_width      800
#define screen_height     1000

#define FPS 60
