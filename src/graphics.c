#include "graphics.h"

#include "constants.h"
#include "utils.h"

#include <raylib.h>
#include <stdio.h>

struct graphicresources {
	Vector3       stars[MAX_STAR];
	Texture       star_atlas;
	Texture       main_spaceship_sprite;
	Texture       enemy_spaceship_sprite;
	Texture       upgrades[3];
	Font          monospace;
	RenderTexture framebuffer;
	Shader        bloom_shader;

} res;

void initialize_graphics() {

	res.star_atlas               = LoadTexture("./res/img/stars/star_atlas.png");
	res.main_spaceship_sprite    = LoadTexture("./res/img/ships/spaceship.png");
	res.enemy_spaceship_sprite   = LoadTexture("./res/img/ships/enemyship.png");
	res.upgrades[UPGRADE_BULLET] = LoadTexture("./res/img/upgrades/upgrade_bullet.png");
	res.upgrades[UPGRADE_SPEED]  = LoadTexture("./res/img/upgrades/upgrade_speed.png");
	res.upgrades[UPGRADE_PACMAN] = LoadTexture("./res/img/upgrades/upgrade_pacman.png");
	res.monospace                = LoadFont("/usr/share/fonts/noto/NotoSansMono-Bold.ttf");
	res.framebuffer              = LoadRenderTexture(screen_width, screen_height);
	res.bloom_shader             = LoadShader(nullptr, "./res/shaders/bloom.frag");
}

void terminate_graphics() {
	UnloadTexture(res.star_atlas);
	UnloadTexture(res.main_spaceship_sprite);
	UnloadTexture(res.enemy_spaceship_sprite);
	UnloadTexture(res.upgrades[0]);
	UnloadTexture(res.upgrades[1]);
	UnloadTexture(res.upgrades[2]);
	UnloadFont(res.monospace);
	UnloadRenderTexture(res.framebuffer);
	UnloadShader(res.bloom_shader);
}

/**
 * Set the given star to a random pos, speed and type
 */
void randomStar(const int index) {

	float x = (float)(GetRandomValue(0, screen_width));
	float y = (float)(GetRandomValue(0, screen_height));

	float speed = ((float)(GetRandomValue(0, 3)) + 1.f);
	speed *= STAR_SPEED_MULT;

	res.stars[index] = (Vector3){x, y, speed};
}

void scatter_stars() {
	for (int i = 0; i < MAX_STAR; ++i) {
		randomStar(i);
	}
}

void draw_stars() {

	for (int i = 0; i < MAX_STAR; ++i) {

		// this can be moved to the shader
		int type = (int)(res.stars[i].x) % 3;

		unsigned char alpha = (unsigned char)(res.stars[i].z / STAR_SPEED_MULT * 30.f);
		Color         col   = {100, 100, 100, 100 + alpha};

		switch (type) {
		case 0:
			col.g = 10;
			col.b = 10; // RED
			break;

		case 1:
			col.r = 128;
			col.g = 128;
			col.b = 10; // YELLOW
			break;

		case 2:
			col.r = 10;
			col.g = 10; // BLUE
			break;
		}

		float width = (float)(res.star_atlas.width) / 3.f;

		Vector2 pos = {res.stars[i].x, res.stars[i].y};

		Rectangle tile = {width * (float)(type),
		                  0.f,
		                  width,
		                  (float)(res.star_atlas.height)};

		DrawTextureRec(res.star_atlas, tile, pos, col);
	}
}

/**
 * move the stars by their own speed
 */
void move_stars() {

	auto deltaTime = get_frametime();

	for (int i = 0; i < MAX_STAR; ++i) {
		res.stars[i].y += res.stars[i].z * STAR_SPEED_MULT * deltaTime;

		if ((res.stars[i].y) - (float)(res.star_atlas.height) > screen_height) {

			randomStar(i);

			// spawn the at the top of the screen
			res.stars[i].y = (float)(-res.star_atlas.height);
		}
	}
}

/**
 * Teleport the spaceship at the opposite side of the screen
 */
void draw_pacman(const Vector2 *pos) {

	Vector2 spaceshipPos = *pos;
	if (spaceshipPos.x < 0) {
		// if (spaceshipPos.x + spaceship_width / 2 < 0) {
		// spaceshipPos.x = spaceshipPos.x + screen_width;
		//}
		DrawTexture(res.main_spaceship_sprite, (int)(screen_width + spaceshipPos.x), (int)(spaceshipPos.y), WHITE);
	}

	if (spaceshipPos.x + spaceship_width > screen_width) {
		// if (spaceshipPos.x + spaceship_width / 2 > screen_width) {
		// spaceshipPos.x = spaceshipPos.x - screen_width;
		//}
		DrawTexture(res.main_spaceship_sprite, (int)(spaceshipPos.x - screen_width), (int)(spaceshipPos.y), WHITE);
	}

	if (spaceshipPos.y < 0) {
		// if (spaceshipPos.y + spaceship_height / 2 < 0) {
		// spaceshipPos.y = spaceshipPos.y + screen_height;
		//}
		DrawTexture(res.main_spaceship_sprite, (int)(spaceshipPos.x), (int)(spaceshipPos.y + screen_height), WHITE);
	}

	if (spaceshipPos.y + spaceship_height > screen_height) {
		// if (spaceshipPos.y + spaceship_height / 2 > screen_height) {
		// spaceshipPos.y = spaceshipPos.y - screen_height;
		//}
		DrawTexture(res.main_spaceship_sprite, (int)(spaceshipPos.x), (int)(spaceshipPos.y - screen_height), WHITE);
	}

	//*pos = spaceshipPos;
}

/**
 * Render all active bullets
 */
void draw_bullets(const Vector4 *bullets, const Vector4 *enemiesBullets) {

	for (int i = 0; i < MAX_BULLETS; ++i) {

		if (bullets[i].x >= -1) {
			Vector2 bstart = {
			    bullets[i].x,
			    bullets[i].y};
			Vector2 bend = {
			    bullets[i].x + bullets[i].z * 2,
			    bullets[i].y + bullets[i].w * 2};

			// I want specifick tickness, therefore the DrawLineEx(startpos, endpos, thickness, color)
			DrawLineEx(bstart, bend, 2.5, WHITE);
		}
		if (enemiesBullets[i].x >= -1) {

			Vector2 bstart = {
			    enemiesBullets[i].x,
			    enemiesBullets[i].y};
			Vector2 bend = {
			    enemiesBullets[i].x + enemiesBullets[i].z * 2,
			    enemiesBullets[i].y + enemiesBullets[i].w * 2};

			DrawLineEx(bstart, bend, 2.5, YELLOW);
		}
	}
}

void draw_ships(const Vector2 *mainship, const Vector2 *enemyships, const hitboxmask hitboxes) {

	DrawTextureV(res.main_spaceship_sprite, (Vector2){mainship->x, mainship->y}, WHITE);
	for (int i = 0; i < MAX_ENEMY; ++i) {

		if (enemyships[i].x >= -40) {
			DrawTextureV(res.enemy_spaceship_sprite, (Vector2){enemyships[i].x, enemyships[i].y}, WHITE);
		}
	}
}

void draw_upgrade(const upgradeType upgrade_type, const Vector2 upgrade_pos) {
	DrawTexture(res.upgrades[upgrade_type], (int)(upgrade_pos.x), (int)(upgrade_pos.y), WHITE);
}

RenderTexture *get_framebuffer() {
	return &res.framebuffer;
}

Shader *get_shader() {
	return &res.bloom_shader;
}
