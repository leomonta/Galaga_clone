#include "graphics.h"

#include "constants.h"

#include <raylib.h>

struct GraphicResources {
	Vector3 stars[MAX_STAR];
	Texture starAtlas;
	Texture spacehipSprite;
	Texture enemySpacehipSprite;
} Res;

/**
 * Set the given star to a random pos, speed and type
 */
void randomStar(const int index) {

	float x = (float)(GetRandomValue(0, screen_width));
	float y = (float)(GetRandomValue(0, screen_height));

	float speed = ((float)(GetRandomValue(0, 3)) + 1.f);
	speed *= STAR_SPEED_MULT;

	Res.stars[index] = (Vector3){x, y, speed};
}

void scatter_stars() {
	for (int i = 0; i < MAX_STAR; ++i) {
		randomStar(i);
	}
}

bool initialize_graphics() {

	Res.starAtlas           = LoadTexture("./res/img/stars/star_atlas.png");
	Res.spacehipSprite      = LoadTexture("./res/img/ships/spaceship.png");
	Res.enemySpacehipSprite = LoadTexture("./res/img/ships/enemyship.png");

	return true;
}

void draw_stars() {

	for (int i = 0; i < MAX_STAR; ++i) {

		// this can be moved to the shader
		int type = (int)(Res.stars[i].x) % 3;

		unsigned char alpha = (unsigned char)(Res.stars[i].z / STAR_SPEED_MULT * 30.f);
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

		float width = (float)(Res.starAtlas.width) / 3.f;

		Vector2 pos = {Res.stars[i].x, Res.stars[i].y};

		Rectangle tile = {width * (float)(type),
		                  0.f,
		                  width,
		                  (float)(Res.starAtlas.height)};

		DrawTextureRec(Res.starAtlas, tile, pos, col);
	}
}

/**
 * move the stars by their own speed
 */
void move_stars() {

	auto deltaTime = GetFrameTime();

	for (int i = 0; i < MAX_STAR; ++i) {
		Res.stars[i].y += Res.stars[i].z * STAR_SPEED_MULT * deltaTime;

		if ((Res.stars[i].y) - (float)(Res.starAtlas.height) > screen_height) {

			randomStar(i);

			// spawn the at the top of the screen
			Res.stars[i].y = (float)(-Res.starAtlas.height);
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
		DrawTexture(Res.spacehipSprite, (int)(screen_width + spaceshipPos.x), (int)(spaceshipPos.y), WHITE);
	}

	if (spaceshipPos.x + spaceship_width > screen_width) {
		// if (spaceshipPos.x + spaceship_width / 2 > screen_width) {
		// spaceshipPos.x = spaceshipPos.x - screen_width;
		//}
		DrawTexture(Res.spacehipSprite, (int)(spaceshipPos.x - screen_width), (int)(spaceshipPos.y), WHITE);
	}

	if (spaceshipPos.y < 0) {
		// if (spaceshipPos.y + spaceship_height / 2 < 0) {
		// spaceshipPos.y = spaceshipPos.y + screen_height;
		//}
		DrawTexture(Res.spacehipSprite, (int)(spaceshipPos.x), (int)(spaceshipPos.y + screen_height), WHITE);
	}

	if (spaceshipPos.y + spaceship_height > screen_height) {
		// if (spaceshipPos.y + spaceship_height / 2 > screen_height) {
		// spaceshipPos.y = spaceshipPos.y - screen_height;
		//}
		DrawTexture(Res.spacehipSprite, (int)(spaceshipPos.x), (int)(spaceshipPos.y - screen_height), WHITE);
	}

	//*pos = spaceshipPos;
}

/**
 * Render all active bullets
 */
void draw_bullets(const Vector4 *bullets, const Vector4 *enemiesBullets) {

	for (int i = 0; i < MAX_BULLETS; ++i) {

		if (bullets[i].x <= -1) {
			Vector2 bstart = {
			    bullets[i].x,
			    bullets[i].y};
			Vector2 bend = {
			    bullets[i].x + bullets[i].z * 2,
			    bullets[i].y + bullets[i].w * 2};

			// I want specifick tickness, therefore the DrawLineEx(startpos, endpos, thickness, color)
			DrawLineEx(bstart, bend, 2.5, WHITE);
		}
		if (enemiesBullets[i].x <= -1) {

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
