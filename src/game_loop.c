#include "game_loop.h"

#include "constants.h"
#include "graphics.h"
#include "notifications.h"
#include "utils.h"
#include <math.h>

typedef struct {

	Vector2       main_pos;
	Vector4       main_bullets[MAX_BULLETS];
	Vector2       enemy_pos[MAX_ENEMY];
	Vector4       enemy_bullets[MAX_ENEMY];
	unsigned int  enemy_health[MAX_ENEMY];
	unsigned int  main_health;
	unsigned char enemy_shot_cooldown[MAX_ENEMY];
	double        main_last_shot_time;
	float         main_speed;
	int           enemy_spawn_rate;
	upgradeType   upgrade_type;
	Vector2       upgrade_pos;

	unsigned int bullet_upgrades;
	bool         pacman_upgrade;
	unsigned int speed_upgrades;
	unsigned int score;
} RuntimeInfo;

RuntimeInfo rti;

void game_loop() {

	spaceship_inputs();

	// more upgrade, more enemies
	auto spawn = GetRandomValue(0, 30000);
	if (spawn < rti.enemy_spawn_rate) {
		spawn_random_enemies(rti.enemy_pos, rti.enemy_health);
		rti.enemy_spawn_rate = ENEMY_SPAWN_RATE;

	} else {
		// increase the probablity of spawning if enemy did not spawn
		int inc = rti.bullet_upgrades * BULLET_SPAWN_WEIGHT;
		inc += rti.speed_upgrades * SPEED_SPAWN_WEIGHT;
		inc += rti.pacman_upgrade * PACMAN_SPAWN_WEIGHT;

		rti.enemy_spawn_rate += inc;
	}

	// manage objects movement and interactions
	physics();

	// check if the spaceship has died
	if (rti.main_health <= 0) {
		return;
	}

	// --------------------------------------------------------------------------------- Draw

	// render to framebuffer
	auto framebuffer = get_framebuffer();
	BeginTextureMode(*framebuffer);
	{
		// draw screen
		ClearBackground(SCREEN_BG);

		draw_stars();

		draw_pacman(&rti.main_pos);

		// draw objects
		draw_ships(&rti.main_pos, rti.enemy_pos, NEITHER);

		// draw the correct upgrade
		draw_upgrade(rti.upgrade_type, rti.upgrade_pos);

		draw_bullets(rti.main_bullets, rti.enemy_bullets);
	}
	EndTextureMode();

	BeginDrawing();
	{
		BeginShaderMode(*get_shader());
		{
			DrawTextureRec(framebuffer->texture, (Rectangle){0.f, 0.f, (float)(framebuffer->texture.width), (float)(-framebuffer->texture.height)}, (Vector2){0.f, 0.f}, WHITE);
		}
		EndShaderMode();
		// Draw spaceship stats on the screen
		/*
		std::string text = "Bullets: " + std::to_string(runtime.spaceship_num_bullets);
		text += "\n Speed: " + std::to_string((int)(runtime.spaceship_Maxspeed));
		text += "\n Health: " + std::to_string(runtime.spaceship_health);
		text += "\n Score: " + std::to_string(runtime.score);
		DrawTextEx(Consolas, text.c_str(), {10, 10}, 20, 1, {255, 255, 255, 150});
		*/

		DrawFPS(200, 10);

		notif__render_notifications();
	}
	EndDrawing();
	//---------------------------------------------------------------------------------- End draw
}

/**
 * Move, if they exist, spaceship bullets and enemy bullets
 */
void move_bullets() {

	auto delta_time = get_frametime();

	// move bullets
	for (int i = 0; i < MAX_BULLETS; ++i) {

		// ----------------------------------------spaceship bullet
		// if the bullet exist
		if (rti.main_bullets[i].x != -1 && rti.main_bullets[i].y != -1) {

			// advance the bullet
			rti.main_bullets[i].x += rti.main_bullets[i].z * delta_time * BULLET_SPEED_MULT;
			rti.main_bullets[i].y += rti.main_bullets[i].w * delta_time * BULLET_SPEED_MULT;

			// if it goes offscreen eliminate it
			if (rti.main_bullets[i].x < 0 || rti.main_bullets[i].x > screen_width || rti.main_bullets[i].y < 0 || rti.main_bullets[i].y > screen_height) {
				rti.main_bullets[i] = default_bullet;
			}
		}

		// ----------------------------------------enemy bullet
		// if the bullet exist
		if (rti.enemy_bullets[i].x != -1 && rti.enemy_bullets[i].y != -1) {

			// advance the bullet
			rti.enemy_bullets[i].x += rti.enemy_bullets[i].z * delta_time * BULLET_SPEED_MULT;
			rti.enemy_bullets[i].y += rti.enemy_bullets[i].w * delta_time * BULLET_SPEED_MULT;

			// if it goes offscreen eliminate it
			if (rti.enemy_bullets[i].x < 0 || rti.enemy_bullets[i].x > screen_width || rti.enemy_bullets[i].y < 0 || rti.enemy_bullets[i].y > screen_height) {
				rti.enemy_bullets[i] = default_bullet;
			}
		}
	}
}

void reset_game() {
	reset_arrays(rti.main_bullets, rti.enemy_bullets, rti.enemy_shot_cooldown, rti.enemy_health, rti.enemy_pos);
}

/**
 * Listens for inputs from keyboards
 */
void spaceship_inputs() {

	auto deltaTime = get_frametime();

	auto curr_speed = rti.main_speed;

	// fire bullets when spacebar pressed
	// --------------------------------------------------------------------------------- Check Keyboard
	if (IsKeyDown(KEY_SPACE)) {
		curr_speed /= 2;

		auto now = get_curr_ms();
		if (now - rti.main_last_shot_time >= MAIN_FIRE_COOLDOWN) {
			fire_bullets();
			rti.main_last_shot_time = now;
		}
	}

	// movement
	if (IsKeyDown(KEY_RIGHT)) {
		rti.main_pos.x += curr_speed * deltaTime * MOVEMENT_SPEED_MULT;
	} else if (IsKeyDown(KEY_LEFT)) {
		rti.main_pos.x -= curr_speed * deltaTime * MOVEMENT_SPEED_MULT;
	}
	if (IsKeyDown(KEY_UP)) {
		rti.main_pos.y -= curr_speed * deltaTime * MOVEMENT_SPEED_MULT;
	} else if (IsKeyDown(KEY_DOWN)) {
		rti.main_pos.y += curr_speed * deltaTime * MOVEMENT_SPEED_MULT;
	}
}

void physics() {

	// -------------------------------------------------------------------------------------------------------------- Move Entities
	if (rti.main_pos.x < 0) {
		rti.main_pos.x = 0;
	}

	if (rti.main_pos.x + spaceship_width > screen_width) {
		rti.main_pos.x = screen_width - spaceship_width;
	}

	if (rti.main_pos.y < 0) {
		rti.main_pos.y = 0;
	}

	if (rti.main_pos.y + spaceship_height > screen_height) {
		rti.main_pos.y = screen_height - spaceship_height;
	}

	move_bullets();

	move_enemies();

	move_stars();

	// -------------------------------------------------------------------------------------------------------------- Collisions
	check_bullets_collision();

	check_entities_collisions();

	// -------------------------------------------------------------------------------------------------------------- Interactions
	enemy_AI();
}

void move_enemies() {

	auto deltaTime = get_frametime();
	// move enemies
	for (int i = 0; i < MAX_ENEMY; ++i) {

		// if it exist
		if (rti.enemy_pos[i].x == -40 || rti.enemy_pos[i].y == -40) {
			continue;
		}
		// move the enemy
		rti.enemy_pos[i].y += ENEMY_SPEED_MULT * deltaTime;

		// and remove it if it goes offscreen or is dead
		if (rti.enemy_health[i] <= 0) {
			notif__schedule_notification("+ 10", (Vector2){rti.enemy_pos[i].x, rti.enemy_pos[i].y}, (unsigned char)(2 * FPS));
			rti.enemy_pos[i] = default_ship_pos;
			rti.score += 10;
		}

		if (rti.enemy_pos[i].y > screen_height) {
			rti.enemy_pos[i] = default_ship_pos;
			rti.score -= 50;
		}
	}
}

void check_bullets_collision() {

	// -------------------------------------------------------------------------------------------- enemy ship collision

	Vector2 intersect_point;

	// the quest to find one bullet for frame
	for (int j = 0; j < MAX_ENEMY; j++) {

		// if the enemy is visible
		if (rti.enemy_pos[j].x == -40 || rti.enemy_health[j] <= 0) {
			continue;
		}

		for (int i = 0; i < MAX_BULLETS; ++i) {
			// if the bullet exist and if the enemy is still alive after being eventually hit
			if (rti.enemy_bullets[i].x == -1 || rti.enemy_bullets[i].y == -1 || rti.enemy_health[j] <= 0) {
				continue;
			}

			if (bullet_enemy_collision(i, j)) {
				return; // only one bullet per frame
			}
		}
	}

	// -------------------------------------------------------------------------------------------- spaceship collision

	// i don't need to recalculate these for each bullet, i calculate once per frame
	Vector4 linespace[2] = {
	    {rti.main_pos.x,
	     rti.main_pos.y + spaceship_height,
	     rti.main_pos.x + spaceship_width,
	     rti.main_pos.y                   }, // linespace 1

	    {rti.main_pos.x,
	     rti.main_pos.y,
	     rti.main_pos.x + spaceship_width,
	     rti.main_pos.y + spaceship_height}  // linespace 2
	};

	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (rti.enemy_bullets[i].x == -1) {
			continue;
		}

		for (int k = 0; k < 2; k++) { // check collision for both diagonals
			Vector4 lineBullet = {rti.enemy_bullets[i].x,
			                      rti.enemy_bullets[i].y,
			                      rti.enemy_bullets[i].x + (rti.enemy_bullets[i].z * 2),
			                      rti.enemy_bullets[i].y + (rti.enemy_bullets[i].w * 2)};

			intersect_point = intersection(lineBullet, linespace[k]);

			if (intersect_point.x != -1) {

				rti.enemy_bullets[i] = default_bullet; // delete bullet
				rti.main_health--;                     // inflict damage
				break;                                 // next bullet
			}
		}

		if (rti.main_health <= 0) {
			return;
		}
	}
}

void check_entities_collisions() {

	Rectangle uBox = {rti.upgrade_pos.x,
	                  rti.upgrade_pos.y,
	                  spaceship_width,
	                  spaceship_height};

	Rectangle sBox = {rti.upgrade_pos.x,
	                  rti.upgrade_pos.y,
	                  spaceship_width,
	                  spaceship_height};

	// check collision with upgrades
	if (CheckCollisionRecs(uBox, sBox)) {

		Vector2 pos = {rti.upgrade_pos.x,
		               rti.upgrade_pos.y};

		switch (rti.upgrade_type) {

		case UPGRADE_PACMAN:
			notif__schedule_notification("+ 500", pos, (unsigned char)(2 * FPS));
			rti.score += 500;
			rti.pacman_upgrade = true;
			break;

		case UPGRADE_SPEED:
			notif__schedule_notification("+ 200", pos, (unsigned char)(2 * FPS));
			rti.score += 200;
			rti.main_speed++;
			if (rti.main_speed > MAX_SPEED_UPGRADE) {
				rti.main_speed = MAX_SPEED_UPGRADE;
			}
			break;

		case UPGRADE_BULLET:
			notif__schedule_notification("+ 200", pos, (unsigned char)(2 * FPS));
			rti.score += 200;
			rti.bullet_upgrades++;
			if (rti.bullet_upgrades > MAX_BULLET_UPGRADE) {
				rti.bullet_upgrades = MAX_BULLET_UPGRADE;
			}
			break;

		default:
			break;
		}

		rti.upgrade_pos = default_ship_pos; // delete upgrade
	}
}

void enemy_AI() {
	// enemy AI
	for (int i = 0; i < MAX_ENEMY; ++i) {
		// check if the enemy exist and i put i little bit of RNG
		if (rti.enemy_pos[i].x == -40) {
			continue;
		}

		if (rti.enemy_shot_cooldown[i] <= 0 && (GetRandomValue(0, 8)) == 1) {

			// distance from spaceship
			float distx = (rti.enemy_pos[i].x + spaceship_width / 2) - (rti.main_pos.x + spaceship_width / 2);
			float disty = (rti.enemy_pos[i].y + spaceship_height) - (rti.main_pos.y + spaceship_height / 2);

			// total distance
			double num = pow(distx, 2) + pow(disty, 2);

			// i need to reduce the distance to a maximun of 12 togheter, using pitagora
			// the formula is this
			float mult = (float)((10 * sqrt(num)) / num);
			float movx = distx * mult * -1;
			float movy = disty * mult * -1;

			Vector4 bullet = {rti.enemy_pos[i].x + spaceship_width / 2,
			                  rti.enemy_pos[i].y + spaceship_height,
			                  movx,
			                  movy};

			add_bullet(bullet, i, rti.main_bullets, rti.enemy_bullets);
			rti.enemy_shot_cooldown[i] = ENEMY_FIRE_COOLDOWN;
		}
		--rti.enemy_shot_cooldown[i];
	}
}

void fire_bullets() {

	float step = 0.5;

	float counter = ((float)(rti.bullet_upgrades) / 2.f - step);
	for (int i = 0; i < rti.bullet_upgrades; i++) {
		Vector4 newbull = {
		    rti.main_pos.x + spaceship_width / 2,
		    rti.main_pos.y + spaceship_height / 3,
		    counter,
		    DEFAULT_BULLET_SPEED};
		add_bullet(newbull, -1, rti.main_bullets, rti.enemy_bullets);

		counter -= 1;
	}
}

bool bullet_enemy_collision(int bulletIndex, int enemyIndex) {
	// check collision for both diagonals
	for (int k = 0; k < 2; k++) {

		Vector4 lineEnemy;

		if (k == 0) {
			// diagonal 1
			lineEnemy = (Vector4){
			    rti.enemy_pos[enemyIndex].x,
			    rti.enemy_pos[enemyIndex].y,
			    rti.enemy_pos[enemyIndex].x + spaceship_width,
			    rti.enemy_pos[enemyIndex].y + spaceship_height};
		} else {
			// diagonal 2
			lineEnemy = (Vector4){
			    rti.enemy_pos[enemyIndex].x + spaceship_width,
			    rti.enemy_pos[enemyIndex].y,
			    rti.enemy_pos[enemyIndex].x,
			    rti.enemy_pos[enemyIndex].y + spaceship_height};
		}

		Vector4 lineBullet = {rti.main_bullets[bulletIndex].x,
		                      rti.main_bullets[bulletIndex].y,
		                      rti.main_bullets[bulletIndex].x + (rti.main_bullets[bulletIndex].z * 2),
		                      rti.main_bullets[bulletIndex].y + (rti.main_bullets[bulletIndex].w * 2)};

		// DrawLine(lineEnemy.x, lineEnemy.y, lineEnemy.z, lineEnemy.w, WHITE);

		Vector2 intersect_point = intersection(lineBullet, lineEnemy);

		// if something collided
		if (intersect_point.x == -1) {
			continue;
		}

		rti.main_bullets[bulletIndex] = default_bullet; // delete bullet
		--rti.enemy_health[enemyIndex];                // inflict damage

		pickRandomUpgrade(enemyIndex);

		return true;
	}

	return false;
}

Vector2 intersection(Vector4 line1, Vector4 line2) {

	// explanation at https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line_segment

	Vector2 res = {-1, -1};

	float den  = ((line1.x - line1.z) * (line2.y - line2.w)) - ((line1.y - line1.w) * (line2.x - line2.z));
	float num1 = ((line1.x - line2.x) * (line2.y - line2.w)) - ((line1.y - line2.y) * (line2.x - line2.z));
	float num2 = ((line1.z - line1.x) * (line1.y - line2.y)) - ((line1.w - line1.y) * (line1.x - line2.x));

	// i use both of those cus i need to check the boundaries of both lines
	float t = num1 / den;
	float u = num2 / den;

	if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
		// x coord of the intersection
		res.x = line1.x + t * (line1.z - line1.x);

		// y coord of the intersection
		res.y = line1.y + t * (line1.w - line1.y);

	} else {
		res = (Vector2){-1, -1};
	}
	return res;
}

void pickRandomUpgrade(int enemyIndex) {

	// drop upgrades if the enemy is dead
	// 20% drop rate and check if the enemy is dead
	if (GetRandomValue(0, 100) < 20 && rti.enemy_health[enemyIndex] <= 0) {
		rti.upgrade_pos = (Vector2){
		    rti.enemy_pos[enemyIndex].x,
		    rti.enemy_pos[enemyIndex].y}; // drop upgrade at enemy old position

		auto temp = GetRandomValue(0, 100); // decide which upgrade
		if (temp < 6) {
			rti.upgrade_type = UPGRADE_PACMAN;
		} else if (temp < 53) {
			rti.upgrade_type = UPGRADE_SPEED;
		} else if (temp < 100) {
			rti.upgrade_type = UPGRADE_BULLET;
		}
	}
}

bool is_main_dead() {
	return rti.main_health <= 0;
}
