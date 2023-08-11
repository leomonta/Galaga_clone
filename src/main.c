/**
 * Galaga clone by Leonardo Montagner https://github.com/Vectoryx
 *
 * Doing this as a real practice in game developing
 *
 * TODO vfx
 * TODO fix enemy firing
 * TODO add stars
 **/

#include "main.h"

#include "constants.h"
#include "loops.h"
#include "notifications.h"
#include "utils.h"

#include <math.h> // for sqrt
#include <raylib.h>
#include <stdint.h>
#include <time.h> // for random numbers

gameState runtime;

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

const gameState default_stat = {
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

int main() {
	// ------------------------------------------------------------------------------------- Initialization

	// Initial preparations
	InitWindow(screenWidth, screenHeight, "Galaga clone by Leonardo");

	// TODO: check if the textures are actually loaded, they return id <= 0 if so
	spaceship_sprite         = LoadTexture("./res/img/ships/spaceship.png");
	Enemyship_sprite         = LoadTexture("./res/img/ships/enemyship.png");
	Upgrades[UPGRADE_BULLET] = LoadTexture("./res/img/upgrades/upgrade_bullet.png");
	Upgrades[UPGRADE_SPEED]  = LoadTexture("./res/img/upgrades/upgrade_speed.png");
	Upgrades[UPGRADE_PACMAN] = LoadTexture("./res/img/upgrades/upgrade_pacman.png");
	Star_ATL                 = LoadTexture("./res/img/stars/star_atlas.png");
	Consolas                 = LoadFont("/usr/share/fonts/noto/NotoSansMono-Bold.ttf");
	frameBuffer              = LoadRenderTexture(screenWidth, screenHeight);
	bloomShader              = LoadShader(nullptr, "./res/shaders/bloom.frag");

	HideCursor();
	// Loading textures

	fillStars(stars);

	resetArrays(bullets, e_bullets, e_coolDown, e_health, enemies);

	runtime = default_stat;
	SetTargetFPS(fps); // Set our game to run at given frames-per-second
	SetExitKey(KEY_NULL);
	//-------------------------------------------------------------------------------------- End initialization

	// repeat loop
	while (!runtime.close) {

		notif__tick();

		// Main game loop
		if (!runtime.pause && runtime.spaceship_health > 0) { // Detect window close button or ESC key
			gameLoop(&runtime, enemies, e_health, &frameBuffer, &spaceship_sprite, &Enemyship_sprite, Upgrades, &bloomShader, &default_stat);
		}

		// death screnn loop
		if (runtime.spaceship_health <= 0) {
			deathLoop(&runtime, bullets, e_bullets, e_coolDown, e_health, enemies, &default_stat);
		}

		// pause screen loop
		if (runtime.pause) {
			pauseLoop(&runtime);
		}
	}

	//-------------------------------------------------------------------------------------- De-Initialization
	UnloadTexture(spaceship_sprite);
	UnloadTexture(Enemyship_sprite);
	UnloadTexture(Upgrades[UPGRADE_BULLET]);
	UnloadTexture(Upgrades[UPGRADE_SPEED]);
	UnloadTexture(Upgrades[UPGRADE_PACMAN]);
	UnloadTexture(Star_ATL);
	UnloadFont(Consolas);

	CloseWindow(); // Close window and OpenGL context

	return 0;
	//-------------------------------------------------------------------------------------- End de-Initialization
}

/**
 * Move, if they exist, spaceship bullets and enemy bullets
 */
void moveBullets() {

	auto deltaTime = GetFrameTime();

	// move bullets
	for (int i = 0; i < MAX_BULLETS; ++i) {

		// ----------------------------------------spaceship bullet
		// if the bullet exist
		if (bullets[i].x != -1 && bullets[i].y != -1) {

			// advance the bullet
			bullets[i].x += bullets[i].z * deltaTime * BULLET_SPEED_MULT;
			bullets[i].y += bullets[i].w * deltaTime * BULLET_SPEED_MULT;

			// if it goes offscreen eliminate it
			if (bullets[i].x < 0 || bullets[i].x > screenWidth || bullets[i].y < 0 || bullets[i].y > screenHeight) {
				bullets[i] = DefaultBullet;
			}
		}

		// ----------------------------------------enemy bullet
		// if the bullet exist
		if (e_bullets[i].x != -1 && e_bullets[i].y != -1) {

			// advance the bullet
			e_bullets[i].x += e_bullets[i].z * deltaTime * BULLET_SPEED_MULT;
			e_bullets[i].y += e_bullets[i].w * deltaTime * BULLET_SPEED_MULT;

			// if it goes offscreen eliminate it
			if (e_bullets[i].x < 0 || e_bullets[i].x > screenWidth || e_bullets[i].y < 0 || e_bullets[i].y > screenHeight) {
				e_bullets[i] = DefaultBullet;
			}
		}
	}
}

/**
 * move the enemies down the screen
 */
void moveEnemies() {

	auto deltaTime = GetFrameTime();
	// move enemies
	for (int i = 0; i < MAX_ENEMY; ++i) {

		// if it exist
		if (enemies[i].x == -40 || enemies[i].y == -40) {
			continue;
		}
		// move the enemy
		enemies[i].y += ENEMY_SPEED_MULT * deltaTime;

		// and remove it if it goes offscreen or is dead
		if (e_health[i] <= 0) {
			notif__scheduleNotification("+ 10", (Vector2){enemies[i].x, enemies[i].y}, (unsigned char)(2 * fps));
			enemies[i] = DefaultShipPos;
			runtime.score += 10;
		}

		if (enemies[i].y > screenHeight) {
			enemies[i] = DefaultShipPos;
			runtime.score -= 50;
		}
	}
}

/**
 * Check collision between bullets, enemies and spaceship. the decrease health/spawn upgrades
 */
void checkBulletsCollision() {

	// -------------------------------------------------------------------------------------------- enemy ship collision

	Vector2 intersect_point;

	// the quest to find one bullet for frame
	for (int j = 0; j < MAX_ENEMY; j++) {

		// if the enemy is visible
		if (enemies[j].x == -40 || e_health[j] <= 0) {
			continue;
		}

		for (int i = 0; i < MAX_BULLETS; ++i) {
			// if the bullet exist and if the enemy is still alive after being eventually hit
			if (bullets[i].x == -1 || bullets[i].y == -1 || e_health[j] <= 0) {
				continue;
			}

			if (bulletEnemyCollision(i, j)) {
				return; // only one bullet per frame
			}
		}
	}

	// -------------------------------------------------------------------------------------------- spaceship collision

	// i don't need to recalculate these for each bullet, i calculate once per frame
	Vector4 linespace[2] = {
	    {runtime.spaceship_box.x,
	     runtime.spaceship_box.y + spaceship_height,
	     runtime.spaceship_box.x + spaceship_width,
	     runtime.spaceship_box.y                   }, // linespace 1

	    {runtime.spaceship_box.x,
	     runtime.spaceship_box.y,
	     runtime.spaceship_box.x + spaceship_width,
	     runtime.spaceship_box.y + spaceship_height}  // linespace 2
	};

	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (e_bullets[i].x == -1) {
			continue;
		}

		for (int k = 0; k < 2; k++) { // check collision for both diagonals
			Vector4 lineBullet = {e_bullets[i].x,
			                      e_bullets[i].y,
			                      e_bullets[i].x + (e_bullets[i].z * 2),
			                      e_bullets[i].y + (e_bullets[i].w * 2)};

			intersect_point = intersection(lineBullet, linespace[k]);

			if (intersect_point.x != -1) {

				e_bullets[i] = DefaultBullet; // delete bullet
				runtime.spaceship_health--;   // inflict damage
				break;                        // next bullet
			}
		}

		if (runtime.spaceship_health <= 0) {
			return;
		}
	}
}

/**
 * Check collisions between spaceship and upgrades, and spaceships with enemies
 */
void checkEntitiesCollisions() {

	Rectangle uBox = {runtime.upgrade_box.x,
	                  runtime.upgrade_box.y,
	                  spaceship_width,
	                  spaceship_height};

	Rectangle sBox = {runtime.spaceship_box.x,
	                  runtime.spaceship_box.y,
	                  spaceship_width,
	                  spaceship_height};

	// check collision with upgrades
	if (CheckCollisionRecs(uBox, sBox)) {

		Vector2 pos = {runtime.upgrade_box.x,
		               runtime.upgrade_box.y};

		switch (runtime.upgrade_type) {

		case UPGRADE_PACMAN:
			notif__scheduleNotification("+ 500", pos, (unsigned char)(2 * fps));
			runtime.score += 500;
			runtime.upgr_PacmanEffect = true;
			break;

		case UPGRADE_SPEED:
			notif__scheduleNotification("+ 200", pos, (unsigned char)(2 * fps));
			runtime.score += 200;
			runtime.spaceship_Maxspeed++;
			if (runtime.spaceship_Maxspeed >= MAX_SPEED) {
				runtime.spaceship_Maxspeed = 15;
			}
			break;

		case UPGRADE_BULLET:
			notif__scheduleNotification("+ 200", pos, (unsigned char)(2 * fps));
			runtime.score += 200;
			runtime.spaceship_num_bullets++;
			if (runtime.spaceship_num_bullets > MAX_BULLET) {
				runtime.spaceship_num_bullets = 10;
			}
			break;

		default:
			break;
		}

		runtime.upgrade_box = DefaultShipPos; // delete upgrade
	}
}

/**
 * every enemy shoot at the spaceship randomly,
 */
void enemyAI() {
	// enemy AI
	for (int i = 0; i < MAX_ENEMY; ++i) {
		// check if the enemy exist and i put i little bit of RNG
		if (enemies[i].x == -40 || e_health[i] < 0) {
			continue;
		}

		if (e_coolDown[i] <= 0 && (GetRandomValue(0, 8)) == 1) {

			// distance from spaceship
			float distx = (enemies[i].x + 15) - (runtime.spaceship_box.x + 15);
			float disty = (enemies[i].y + 30) - (runtime.spaceship_box.y + 15);

			// total distance
			double num = pow(distx, 2) + pow(disty, 2);

			// i need to reduce the distance to a maximun of 12 togheter, using pitagora
			// the formula is this
			float mult = (float)((10 * sqrt(num)) / num);
			float movx = distx * mult * -1;
			float movy = disty * mult * -1;

			Vector4 bullet = {enemies[i].x + spaceship_width / 2,
			                  enemies[i].y + spaceship_height,
			                  movx,
			                  movy};

			addBullet(bullet, i, bullets, e_bullets);
			e_coolDown[i] = (char)(default_stat.e_fireCoolDown);
		}
		e_coolDown[i]--;
	}
}

/**
 * Move entities, remove them and check the interaction between them
 */
void physics() {

	// -------------------------------------------------------------------------------------------------------------- Move Entities
	moveBullets();

	moveEnemies();

	moveStars();

	// -------------------------------------------------------------------------------------------------------------- Collisions
	checkBulletsCollision();

	checkEntitiesCollisions();

	// -------------------------------------------------------------------------------------------------------------- Interactions
	enemyAI();
}

/**
 * Fire the amount, given by the var spaceship_num_bullets, of bullets with the correct angle
 */
void fireBullets() {

	float step = 0.5;

	float counter = ((float)(runtime.spaceship_num_bullets) / 2.f - step);
	for (int i = 0; i < runtime.spaceship_num_bullets; i++) {
		Vector4 newBull = {
		    runtime.spaceship_box.x + spaceship_width / 2,
		    runtime.spaceship_box.y + spaceship_height / 3,
		    counter,
		    DEFAULT_BULLET_SPEED};
		addBullet(newBull, -1, bullets, e_bullets);

		counter -= 1;
	}
}

/**
 * Return the point of intersection between two line defined by two points
 */
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

/**
 * RAndompli choose and placee an upgrade in the field
 */
void pickRandomUpgrade(int enemyIndex) {

	// drop upgrades if the enemy is dead
	// 20% drop rate and check if the enemy is dead
	if (GetRandomValue(0, 100) < 20 && e_health[enemyIndex] <= 0) {
		runtime.upgrade_box = (Vector2){
		    enemies[enemyIndex].x,
		    enemies[enemyIndex].y}; // drop upgrade at enemy old position

		auto temp = GetRandomValue(0, 100); // decide which upgrade
		if (temp < 6) {
			runtime.upgrade_type = UPGRADE_PACMAN;
		} else if (temp < 53) {
			runtime.upgrade_type = UPGRADE_SPEED;
		} else if (temp < 100) {
			runtime.upgrade_type = UPGRADE_BULLET;
		}
	}
}

/**
 * Shorthand to check for hitting an enemy
 */
bool bulletEnemyCollision(int bulletIndex, int enemyIndex) {
	// check collision for both diagonals
	for (int k = 0; k < 2; k++) {

		Vector4 lineEnemy;

		if (k == 0) {
			// diagonal 1
			lineEnemy = (Vector4){
			    enemies[enemyIndex].x,
			    enemies[enemyIndex].y,
			    enemies[enemyIndex].x + spaceship_width,
			    enemies[enemyIndex].y + spaceship_height};
		} else {
			// diagonal 2
			lineEnemy = (Vector4){
			    enemies[enemyIndex].x + spaceship_width,
			    enemies[enemyIndex].y,
			    enemies[enemyIndex].x,
			    enemies[enemyIndex].y + spaceship_height};
		}

		Vector4 lineBullet = {bullets[bulletIndex].x,
		                      bullets[bulletIndex].y,
		                      bullets[bulletIndex].x + (bullets[bulletIndex].z * 2),
		                      bullets[bulletIndex].y + (bullets[bulletIndex].w * 2)};

		// DrawLine(lineEnemy.x, lineEnemy.y, lineEnemy.z, lineEnemy.w, WHITE);

		Vector2 intersect_point = intersection(lineBullet, lineEnemy);

		// if something collided
		if (intersect_point.x == -1) {
			continue;
		}

		bullets[bulletIndex] = DefaultBullet; // delete bullet
		--e_health[enemyIndex];               // inflict damage

		pickRandomUpgrade(enemyIndex);

		return true;
	}

	return false;
}

/**
 * Listens for inputs from keyboards
 */
void gameInputs() {

	auto deltaTime = GetFrameTime();

	// --------------------------------------------------------------------------------- Check Keyboard
	// fire bullets when spacebar pressed
	if (IsKeyDown(KEY_SPACE)) {
		runtime.spaceship_speed /= 2;

		auto now = getCurrMs();
		if (now - runtime.lastShot >= runtime.fireCoolDown) {
			fireBullets();
			runtime.lastShot = now;
		}
	}

	// movement
	if (IsKeyDown(KEY_RIGHT)) {
		runtime.spaceship_box.x += (float)(runtime.spaceship_speed) * deltaTime * MOVEMENT_SPEED_MULT;
	} else if (IsKeyDown(KEY_LEFT)) {
		runtime.spaceship_box.x -= (float)(runtime.spaceship_speed) * deltaTime * MOVEMENT_SPEED_MULT;
	}
	if (IsKeyDown(KEY_UP)) {
		runtime.spaceship_box.y -= (float)(runtime.spaceship_speed) * deltaTime * MOVEMENT_SPEED_MULT;
	} else if (IsKeyDown(KEY_DOWN)) {
		runtime.spaceship_box.y += (float)(runtime.spaceship_speed) * deltaTime * MOVEMENT_SPEED_MULT;
	}
}