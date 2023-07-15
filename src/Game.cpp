/**
 * Galaga clone by Leonardo Montagner https://github.com/Vectoryx
 *
 * Doing this as a real practice in game developing
 *
 * TODO vfx
 * TODO fix enemy firing
 * TODO add stars
 * TODO add messages for points and upgrades
 **/
#include "nlohmann/json.hpp"
#include "raylib.h"

using json = nlohmann::json;

#include <chrono> // game loop fps synchronization
#include <ctime>  // for random numbers
#include <fstream>
#include <iomanip> // for json formatting
#include <iostream>
#include <math.h> // for sqrt
#include <string>

#define FOR(x) for (int i = 0; i < x; i++)

#define MAX_BULLETS     1000
#define MAX_ENEMY       500
#define MAX_STAR        100
#define MAX_LENGHT_NAME 6
#define DefaultBullet \
	{ -1, -1, 0, 0 }
#define DefaultShip \
	{ -40, -40, 0, 0 }
#define DefaultStar \
	{ -1, -1, -1, -1 }

#define ATTRIB_LOC_LIGHT_POS  26 // last raylib loc is 25
#define ATTRIB_NAME_LIGHT_POS "lightPos"
// drawing loops
void gameLoop();
void deathLoop();

// Each function should be small and execute only what it has to
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
void    save();
void    fillStars();
void    randomStar(int index);
void    renderStars();
void    moveStars();

// The fist two values represent the coordinates, the latter two the accelleration
Vector4 bullets[MAX_BULLETS];
Vector4 e_bullets[MAX_BULLETS];

// The coordinates of all the enemies
Rectangle enemies[MAX_ENEMY];

// other enemies stats
char e_coolDown[MAX_ENEMY];
int  e_health[MAX_ENEMY];

// x, y, z,
// x, y, speed
// the type is deducted from the last number of the x coord
// type = x % 3
Vector3 stars[MAX_STAR];

// Runtime values that are gonna change
struct RuntimeVals {
	// spaceship stats
	Rectangle spaceship_box         = {400, 800, 31, 31};
	float     spaceship_speed       = 4.0f;
	int       spaceship_health      = 10;
	int       spaceship_num_bullets = 1;
	float     spaceship_Maxspeed    = 4.0f;
	int       fireCoolDown          = 5;
	int       e_fireCoolDown        = 10;
	bool      upgr_PacmanEffect     = false;

	Rectangle upgrade_box      = {-40, -40, 31, 31};
	int       enemy_spawn_rate = 1; // 10 % base spawn rate per-frame
	// 0 pacman, 1 speed, 2 bullet
	int upgrade_type           = -1;

	int  score = 0;
	bool close = false; // should close the program
	bool pause = false; // should pause the game
} Runtime;
const RuntimeVals default_stat;

#define spaceship_width  30
#define spaceship_height 30
#define screenWidth      800
#define screenHeight     1000

const int fps = 30;
Texture   spaceship_sprite;
Texture   Enemyship_sprite;
Texture   Upgrade_bullet;
Texture   Upgrade_speed;
Texture   Upgrade_pacman;
Texture   Star_ATL;
Font      Consolas;
double    loopTime;
int       exitCount = 0;

int main(void) {
	// ------------------------------------------------------------------------------------- Initialization

	// Initial preparations
	srand((unsigned int)time(0));
	InitWindow(screenWidth, screenHeight, "Galaga clone by Leonardo");

	spaceship_sprite = LoadTexture("./res/img/ships/spaceship.png");
	Enemyship_sprite = LoadTexture("./res/img/ships/enemyship.png");
	Upgrade_bullet   = LoadTexture("./res/img/upgrades/upgrade_bullet.png");
	Upgrade_speed    = LoadTexture("./res/img/upgrades/upgrade_speed.png");
	Upgrade_pacman   = LoadTexture("./res/img/upgrades/upgrade_pacman.png");
	Star_ATL         = LoadTexture("./res/img/stars/star_atlas.png");
	Consolas         = LoadFont("/usr/share/fonts/noto/NotoSansMono-Bold.ttf");

	HideCursor();
	// Loading textures

	fillStars();

	resetArrays();

	resetStats();
	SetTargetFPS(fps); // Set our game to run at given frames-per-second
	SetExitKey(KEY_NULL);
	//-------------------------------------------------------------------------------------- End initialization

	auto      prev = std::chrono::high_resolution_clock::now();
	auto      now  = std::chrono::high_resolution_clock::now();
	long long diff; // previous and current time in ms

	// repeat loop
	while (!Runtime.close) {

		now = std::chrono::high_resolution_clock::now();

		diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();

		// if a frame has passed
		if (diff < 1000 / fps) {
			continue;
		}

		prev = std::chrono::high_resolution_clock::now();

		// Main game loop
		if (!Runtime.pause) { // Detect window close button or ESC key
			gameLoop();
		}

		// death screnn loop
		if (Runtime.spaceship_health <= 0) {
			deathLoop();
		}

		// pause screen loop
		if (Runtime.pause) {
			BeginDrawing();

			// screen opacity
			DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 128});

			// pause message
			DrawText("Game Paused", 200, 400, 30, WHITE);
			DrawText("Press Esc or P to unpause", 250, 440, 20, WHITE);
			DrawText("Press Q to close", 250, 470, 20, WHITE);

			EndDrawing();

			while (true) {

				PollInputEvents();

				if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
					Runtime.close = true;
					break;
				}

				// Unpause Button
				if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
					Runtime.pause = false;
					break;
				}
			}
			PollInputEvents();
		}
	}

	//-------------------------------------------------------------------------------------- De-Initialization
	UnloadTexture(spaceship_sprite);
	UnloadTexture(Enemyship_sprite);
	UnloadTexture(Upgrade_bullet);
	UnloadTexture(Upgrade_pacman);
	UnloadTexture(Upgrade_speed);
	UnloadTexture(Star_ATL);
	UnloadFont(Consolas);

	CloseWindow(); // Close window and OpenGL context

	return 0;
	//-------------------------------------------------------------------------------------- End de-Initialization
}

/**
 * take care of the entire game loop, drawing stuff
 */
void gameLoop() {

	if (WindowShouldClose()) {
		Runtime.close = true;
		return;
	}

	Runtime.spaceship_speed = Runtime.spaceship_Maxspeed;

	// --------------------------------------------------------------------------------- Check Keyboard
	// fire bullets when spacebar pressed
	if (IsKeyDown(KEY_SPACE)) {
		Runtime.spaceship_speed /= 2;
		if (Runtime.fireCoolDown < 0) {
			Runtime.fireCoolDown = default_stat.fireCoolDown;
			fireBullets();
		}
	}

	// movement
	if (IsKeyDown(KEY_RIGHT)) {
		Runtime.spaceship_box.x += Runtime.spaceship_speed;
	} else if (IsKeyDown(KEY_LEFT)) {
		Runtime.spaceship_box.x -= Runtime.spaceship_speed;
	}
	if (IsKeyDown(KEY_UP)) {
		Runtime.spaceship_box.y -= Runtime.spaceship_speed;
	} else if (IsKeyDown(KEY_DOWN)) {
		Runtime.spaceship_box.y += Runtime.spaceship_speed;
	}

	// Pause Button
	if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
		Runtime.pause = true;
		return;
	}

	// more upgrade, more enemies
	int spawn = int(rand()) % 10000;
	if (spawn < int(Runtime.enemy_spawn_rate)) {
		spawnEnemies();
		Runtime.enemy_spawn_rate = default_stat.enemy_spawn_rate;
	} else {
		// increase the probablity of spawning if enemy did not spawn
		//							0.1 for every bullet upgrade,	for every speed upgrade,											  and for the pacman effect
		Runtime.enemy_spawn_rate += Runtime.spaceship_num_bullets + (int)(Runtime.spaceship_Maxspeed - default_stat.spaceship_Maxspeed) + Runtime.upgr_PacmanEffect;
	}

	// manage objects movement and interactions
	physics();

	// check if the spaceship has died
	if (Runtime.spaceship_health <= 0) {
		return;
	}

	// --------------------------------------------------------------------------------- Draw
	BeginDrawing();

	// draw screen
	ClearBackground({0, 0, 40, 255});

	pacmanEffect(spaceship_sprite);

	// draw objects
	DrawTextureV(spaceship_sprite, {Runtime.spaceship_box.x, Runtime.spaceship_box.y}, WHITE);
	FOR(MAX_ENEMY) {

		if (enemies[i].x != -40 && enemies[i].y != -40) {
			DrawTextureV(Enemyship_sprite, {enemies[i].x, enemies[i].y}, WHITE);
		}
	}

	// draw the correct upgrade
	if (Runtime.upgrade_type == 0) {
		DrawTexture(Upgrade_pacman, (int)(Runtime.upgrade_box.x), (int)(Runtime.upgrade_box.y), WHITE);

	} else if (Runtime.upgrade_type == 1) {
		DrawTexture(Upgrade_speed, (int)(Runtime.upgrade_box.x), (int)(Runtime.upgrade_box.y), WHITE);

	} else if (Runtime.upgrade_type == 2) {
		DrawTexture(Upgrade_bullet, (int)(Runtime.upgrade_box.x), (int)(Runtime.upgrade_box.y), WHITE);
	}

	renderStars();

	renderBullets();

	// Draw spaceship stats on the screen
	std::string text = "Bullets: " + std::to_string(Runtime.spaceship_num_bullets);
	text += "\n Speed: " + std::to_string((int)(Runtime.spaceship_Maxspeed));
	text += "\n Health: " + std::to_string(Runtime.spaceship_health);
	text += "\n Score: " + std::to_string(Runtime.score);
	DrawTextEx(Consolas, text.c_str(), {10, 10}, 20, 1, {255, 255, 255, 150});

	EndDrawing();
	//---------------------------------------------------------------------------------- End draw
	Runtime.fireCoolDown--;
}

/**
 * black screen and user input
 */
void deathLoop() {

	BeginDrawing();

	ClearBackground(BLACK);
	DrawText("Game Over", 200, 400, 30, WHITE);
	DrawText("Press Q to exit", 250, 440, 20, WHITE);
	DrawText("Press Enter to retry", 250, 470, 20, WHITE);

	EndDrawing();

	if (IsKeyPressed(KEY_ENTER)) {
		save();
		resetArrays();

		resetStats();
		return;
	}

	if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
		Runtime.close = true;
		save();
		return;
	}
}

/**
 * Quick shortcut to spawn a random enemy in the top of the map
 */
void spawnEnemies() {
	addEnemies({(float)(rand() % (screenWidth - 31)), (float)(rand() % 40), 31.0, 31.0});
}

/**
 * Add the given enemy in the first spot available and set its health
 */
void addEnemies(Rectangle coords) {

	FOR(MAX_ENEMY) {

		if (enemies[i].x == -40 && enemies[i].y == -40) {
			enemies[i]  = coords;
			e_health[i] = 5;
			return;
		}
	}
}

/**
 * Add the given bullet in the first spot available
 * n is the index of the enemy who shoot it, -1 if it was shot by the main spaceship
 */
void addBullet(Vector4 bullet, int n) {

	if (n == -1) {
		FOR(MAX_BULLETS) {

			if (bullets[i].x == -1 && bullets[i].y == -1) {
				bullets[i] = bullet;
				return;
			}
		}
	} else {

		if (e_bullets[n].x == -1 && e_bullets[n].y == -1) {
			e_bullets[n] = bullet;
			return;
		}
	}
}

/**
 * Render all active bullets
 */
void renderBullets() {

	FOR(MAX_BULLETS) {

		if (bullets[i].x != -1 && bullets[i].y != -1) {
			// I want specifick tickness, therefore the DrawLineEx(startpos, endpos, thickness, color)
			DrawLineEx({bullets[i].x, bullets[i].y}, {bullets[i].x + bullets[i].z, bullets[i].y + bullets[i].w}, 2.5, WHITE);
		}
		if (e_bullets[i].x != -1 && e_bullets[i].y != -1) {
			// I want specifick tickness, therefore the DrawLineEx(startpos, endpos, thickness, color)
			DrawLineEx({e_bullets[i].x, e_bullets[i].y}, {e_bullets[i].x + e_bullets[i].z, e_bullets[i].y + e_bullets[i].w}, 2.5, YELLOW);
		}
	}
}

/**
 * Set the initial value for all the used arrays
 */
void resetArrays() {

	FOR(MAX_BULLETS) {

		bullets[i]   = DefaultBullet;
		e_bullets[i] = DefaultBullet;
	}

	FOR(MAX_ENEMY) {

		e_health[i]   = 0;
		e_coolDown[i] = static_cast<char>(default_stat.e_fireCoolDown);
		enemies[i]    = DefaultShip;
	}
}

/**
 * Reset all the stats and upgrades
 */
void resetStats() {
	Runtime = default_stat;
}

/**
 * Move, if they exist, spaceship bullets and enemy bullets
 */
void moveBullets() {

	// move bullets
	FOR(MAX_BULLETS) {

		// ----------------------------------------spaceship bullet
		// if the bullet exist
		if (bullets[i].x != -1 && bullets[i].y != -1) {

			// advance the bullet
			bullets[i].x += bullets[i].z;
			bullets[i].y += bullets[i].w;

			// if it goes offscreen eliminate it
			if (bullets[i].x < 0 || bullets[i].x > screenWidth || bullets[i].y < 0 || bullets[i].y > screenHeight) {
				bullets[i] = DefaultBullet;
			}
		}

		// ----------------------------------------enemy bullet
		// if the bullet exist
		if (e_bullets[i].x != -1 && e_bullets[i].y != -1) {

			// advance the bullet
			e_bullets[i].x += e_bullets[i].z;
			e_bullets[i].y += e_bullets[i].w;

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
	// move enemies
	FOR(MAX_ENEMY) {

		// if it exist
		if (enemies[i].x == -40 || enemies[i].y == -40) {
			continue;
		}
		// move the enemy
		enemies[i].y++;

		// and remove it if it goes offscreen or is dead
		if (e_health[i] <= 0) {
			enemies[i] = DefaultShip;
			Runtime.score += 10;
		}

		if (enemies[i].y > screenHeight) {
			enemies[i] = DefaultShip;
			Runtime.score -= 50;
		}
	}
}

/**
 * Check collision between bullets, enemies and spaceship. the decrease health/spawn upgrades
 */
void checkBulletsCollision() {

	// -------------------------------------------------------------------------------------------- enemy ship collision

	int     temp;
	Vector4 lineBullet;
	Vector2 intersect_point;

	// the quest to find one bullet for frame
	for (int j = 0; j < MAX_ENEMY; j++) {

		// if the enemy is visible
		if (enemies[j].x == -40) {
			continue;
		}

		FOR(MAX_BULLETS) {
			// if the bullet exist and if the enemy is alive
			if (bullets[i].x == -1 || bullets[i].y == -1 || e_health[j] <= 0) {
				continue;
			}

			// check collision for both diagonals
			for (int k = 0; k < 2; k++) {

				Vector4 lineEnemy;

				if (k == 0) {
					// diagonal 1
					lineEnemy = {enemies[j].x + enemies[j].width / 2, enemies[j].y, enemies[j].x + enemies[j].width / 2, enemies[j].y + enemies[j].height};
				} else {
					// diagonal 2
					lineEnemy = {enemies[j].x, enemies[j].y + enemies[j].height / 2, enemies[j].x + enemies[j].width, enemies[j].y + enemies[j].height / 2};
				}

				lineBullet = {bullets[i].x, bullets[i].y, bullets[i].x + (bullets[i].z * 2), bullets[i].y + (bullets[i].w * 2)};

				intersect_point = intersection(lineBullet, lineEnemy);

				// if something collided
				if (intersect_point.x == -1) {
					continue;
				}

				bullets[i] = DefaultBullet; // delete bullet
				e_health[j]--;              // inflict damage

				// drop upgrades if the enemy is dead
				// 20% drop rate and check if the enemy is dead
				if (rand() % 100 < 20 && e_health[j] <= 0) {
					Runtime.upgrade_box = {enemies[j].x, enemies[j].y, enemies[j].width, enemies[j].height}; // drop upgrade at enemy old position
					temp                = rand() % 100;                                                      // decide which upgrade
					if (temp < 6) {
						Runtime.upgrade_type = 0; // pacman
					} else if (temp < 53) {
						Runtime.upgrade_type = 1; // speed
					} else if (temp < 100) {
						Runtime.upgrade_type = 2; // bullet
					}
				}
				break; // next bullet
			}
		}
	}

	// -------------------------------------------------------------------------------------------- spaceship collision

	// i don't need to recalculate these for each bullet, i calculate once per frame
	Vector4 linespace[2] = {
	    {Runtime.spaceship_box.x,
	     Runtime.spaceship_box.y + Runtime.spaceship_box.height,
	     Runtime.spaceship_box.x + Runtime.spaceship_box.width,
	     Runtime.spaceship_box.y	                                                                                  }, // linespace1

	    {Runtime.spaceship_box.x,
	     Runtime.spaceship_box.y,
	     Runtime.spaceship_box.x + Runtime.spaceship_box.width, Runtime.spaceship_box.y + Runtime.spaceship_box.height}
    }; // linespace2

	FOR(MAX_BULLETS) {
		if (e_bullets[i].x != -1 && e_bullets[i].y != -1) {

			for (int k = 0; k < 2; k++) { // check collision for both diagonals
				lineBullet = {e_bullets[i].x, e_bullets[i].y, e_bullets[i].x + (e_bullets[i].z * 2), e_bullets[i].y + (e_bullets[i].w * 2)};

				intersect_point = intersection(lineBullet, linespace[k]);

				if (intersect_point.x != -1) {

					e_bullets[i] = DefaultBullet; // delete bullet
					Runtime.spaceship_health--;   // inflict damage
					break;                        // next bullet
				}
			}

			if (Runtime.spaceship_health <= 0) {
				return;
			}
		}
	}
}

/**
 * Check collisions between spaceship and upgrades, and spaceships with enemies
 */
void checkEntitiesCollisions() {

	// check collision with upgrades
	if (CheckCollisionRecs(Runtime.upgrade_box, Runtime.spaceship_box)) {

		switch (Runtime.upgrade_type) {

		case 0: // pacman upgrade
			Runtime.score += 500;
			Runtime.upgr_PacmanEffect = true;
			break;

		case 1: // speed upgrade
			Runtime.score += 200;
			Runtime.spaceship_Maxspeed++;
			if (Runtime.spaceship_Maxspeed >= 15) {
				Runtime.spaceship_Maxspeed = 15;
			}
			break;

		case 2: // bullet upgrade
			Runtime.score += 200;
			Runtime.spaceship_num_bullets++;
			if (Runtime.spaceship_num_bullets > 10) {
				Runtime.spaceship_num_bullets = 10;
			}
			break;

		default:
			break;
		}

		Runtime.upgrade_box = DefaultShip; // delete upgrade
	}
}

/**
 * every enemy shoot at the spaceship randomly,
 */
void enemyAI() {
	// enemy AI
	FOR(MAX_ENEMY) {
		// check if the enemy exist and i put i little bit of RNG
		if (enemies[i].x == -40 || e_health[i] < 0) {
			continue;
		}

		if (e_coolDown[i] <= 0 && (rand() % 8) == 1) {

			// distance from spaceship
			float distx = (enemies[i].x + 15) - (Runtime.spaceship_box.x + 15);
			float disty = (enemies[i].y + 30) - (Runtime.spaceship_box.y + 15);

			// total distance
			double num = pow(distx, 2) + pow(disty, 2);

			// i need to reduce the distance to a maximun of 12 togheter, using pitagora
			// the formula is this
			float mult = (float)((10 * sqrt(num)) / num);
			float movx = distx * mult * -1;
			float movy = disty * mult * -1;

			Vector4 bullet = {enemies[i].x + 15, enemies[i].y + enemies[i].height, movx, movy};

			addBullet(bullet, i);
			e_coolDown[i] = static_cast<char>(default_stat.e_fireCoolDown);
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

	float counter = (static_cast<float>(Runtime.spaceship_num_bullets) / 2.f - step);
	for (int i = 0; i < Runtime.spaceship_num_bullets; i++) {
		addBullet({Runtime.spaceship_box.x + spaceship_width / 2, Runtime.spaceship_box.y, counter, -12}, -1);

		counter -= 1;
	}
}

/**
 * Teleport the spaceship at the opposite side of the screen
 */
void pacmanEffect(Texture sprite) {
	if (Runtime.upgr_PacmanEffect) {
		if (Runtime.spaceship_box.x < 0) {
			if (Runtime.spaceship_box.x + 15 < 0) {
				Runtime.spaceship_box.x = Runtime.spaceship_box.x + screenWidth;
			}
			DrawTexture(sprite, (int)(screenWidth + Runtime.spaceship_box.x), (int)(Runtime.spaceship_box.y), WHITE);
		}

		if (Runtime.spaceship_box.x + Runtime.spaceship_box.width > screenWidth) {
			if (Runtime.spaceship_box.x + 15 > screenWidth) {
				Runtime.spaceship_box.x = Runtime.spaceship_box.x - screenWidth;
			}
			DrawTexture(sprite, (int)(Runtime.spaceship_box.x - screenWidth), (int)(Runtime.spaceship_box.y), WHITE);
		}

		if (Runtime.spaceship_box.y < 0) {
			if (Runtime.spaceship_box.y + 15 < 0) {
				Runtime.spaceship_box.y = Runtime.spaceship_box.y + screenHeight;
			}
			DrawTexture(sprite, (int)(Runtime.spaceship_box.x), (int)(Runtime.spaceship_box.y + screenHeight), WHITE);
		}

		if (Runtime.spaceship_box.y + Runtime.spaceship_box.height > screenHeight) {
			if (Runtime.spaceship_box.y + 15 > screenHeight) {
				Runtime.spaceship_box.y = Runtime.spaceship_box.y - screenHeight;
			}
			DrawTexture(sprite, (int)(Runtime.spaceship_box.x), (int)(Runtime.spaceship_box.y - screenHeight), WHITE);
		}
	} else {
		if (Runtime.spaceship_box.x < 0) {
			Runtime.spaceship_box.x = 0;
		}

		if (Runtime.spaceship_box.x + Runtime.spaceship_box.width > screenWidth) {
			Runtime.spaceship_box.x = screenWidth - Runtime.spaceship_box.width;
		}

		if (Runtime.spaceship_box.y < 0) {
			Runtime.spaceship_box.y = 0;
		}

		if (Runtime.spaceship_box.y + Runtime.spaceship_box.height > screenHeight) {
			Runtime.spaceship_box.y = screenHeight - Runtime.spaceship_box.height;
		}
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
		res = {-1, -1};
	}
	return res;
}

/**
 * Save the score and the name of the player in a json
 */
void save() {

	SetMouseCursor(MOUSE_CURSOR_IBEAM);

	char name[MAX_LENGHT_NAME + 1];
	memset(name, 0, sizeof(name));
	int nameIndex = 0;

	char character;
	while (true) {
		character = char(GetCharPressed());

		std::string hiscore = "Your score is : " + std::to_string(Runtime.score);

		BeginDrawing();
		ClearBackground(BLACK);

		DrawText(hiscore.c_str(), 200, 400, 30, WHITE);
		DrawText("Insert Your Name", 250, 440, 20, WHITE);
		DrawText("Press Enter to confirm", 250, 470, 20, WHITE);
		DrawText(name, 250, 500, 20, WHITE);

		EndDrawing();

		if ((character > 0) && (character >= 32) && (character <= 125) && (nameIndex < MAX_LENGHT_NAME)) {
			name[nameIndex]     = character;
			name[nameIndex + 1] = '\0';
			nameIndex++;
		}
		if (IsKeyPressed(KEY_BACKSPACE)) {
			nameIndex--;
			if (nameIndex < 0) {
				nameIndex = 0;
			}
			name[nameIndex] = '\0';
		}

		if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
			Runtime.close = true;
			return;
		}

		if (IsKeyPressed(KEY_ENTER)) {
			break;
		}
	}

	std::ifstream HiScores("./res/HiScores.json");

	json j;

	HiScores >> j;

	if (j["hiscores"][name] <= Runtime.score) {
		j["hiscores"][name] = Runtime.score;
	}

	std::ofstream oHiScores("./res/HiScores.json");

	oHiScores << std::setfill('\t') << std::setw(1) << j << std::endl;
}

/**
 * fill the stars array with random stars in random positions at random speed
 */
void fillStars() {
	FOR(MAX_STAR) {
		randomStar(i);
	}
}

/**
 * Set the given star to a random pos, speed and type
 */
void randomStar(int index) {

	float x = static_cast<float>(rand() % screenWidth);
	float y = static_cast<float>(rand() % screenHeight);

	float speed = static_cast<float>(rand() % 3) + 1.f;

	stars[index] = {x, y, speed};
}

/**
 * Draw the right texture for each star
 */
void renderStars() {

	FOR(MAX_STAR) {

		// this can be moved to the shader
		auto type = static_cast<int>(stars[i].x) % 3;

		unsigned char light = static_cast<unsigned char>(stars[i].z * 60 + 20);
		Color         col   = {255, 255, 255, light};

		if (type == 0) {
			col = {255, 10, 10, light}; // RED
		} else if (type == 1) {
			col = {128, 128, 10, light}; // YELLOW
		} else if (type == 2) {
			col = {10, 10, 255, light}; // BLUE
		}

		auto width = Star_ATL.width / 3.f;

		Vector2 pos = {stars[i].x, stars[i].y};

		Rectangle tile = {width * type, 0.f, width, Star_ATL.height};

		// DrawTexture(Star_ATL, posX, posY, col);
		DrawTextureRec(Star_ATL, tile, pos, col);
	}
}

/**
 * move the stars by their own speed
 */
void moveStars() {
	FOR(MAX_STAR) {
		stars[i].y += stars[i].z;

		if (stars[i].y - Star_ATL.height > static_cast<float>(screenHeight)) {

			randomStar(i);

			// spawn the at the top of the screen
			stars[i].y = -Star_ATL.height;
		}
	}
}
