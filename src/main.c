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
#include "notifications.h"

#include <math.h> // for sqrt
#include <raylib.h>
#include <stdint.h>
#include <time.h> // for random numbers

#define FOR(x) for (int i = 0; i < x; ++i)

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

	fillStars();

	resetArrays();

	resetStats();
	SetTargetFPS(fps); // Set our game to run at given frames-per-second
	SetExitKey(KEY_NULL);
	//-------------------------------------------------------------------------------------- End initialization

	// repeat loop
	while (!runtime.close) {

		notif__tick();

		// Main game loop
		if (!runtime.pause && runtime.spaceship_health > 0) { // Detect window close button or ESC key
			gameLoop();
		}

		// death screnn loop
		if (runtime.spaceship_health <= 0) {
			deathLoop();
		}

		// pause screen loop
		if (runtime.pause) {
			pauseLoop();
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
 * print a couple messages just once
 */
void pauseLoop() {

	BeginDrawing();

	// screen opacity
	DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 128});

	// pause message
	DrawText("Game Paused", 200, 400, 30, WHITE);
	DrawText("Press Esc or P to unpause", 250, 440, 20, WHITE);
	DrawText("Press Q to close", 250, 470, 20, WHITE);

	EndDrawing();

	while (true) {

		// poll inputs and keep the frame time usable
		BeginDrawing();
		EndDrawing();

		if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
			runtime.close = true;
			break;
		}

		// Unpause Button
		if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
			runtime.pause = false;
			break;
		}
	}

	// get rid of the lingering <Esc> / <P> pressed
	PollInputEvents();
}

/**
 * take care of the entire game loop, drawing stuff
 */
void gameLoop() {


	if (WindowShouldClose()) {
		runtime.close = true;
		return;
	}

	// Pause Button
	if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
		runtime.pause = true;
		return;
	}

	gameInputs();

	// limit the speed to the maximum possible as base
	runtime.spaceship_speed = runtime.spaceship_Maxspeed;

	// more upgrade, more enemies
	float spawn = (float)(GetRandomValue(0, 30000));
	if (spawn < runtime.enemy_spawn_rate) {
		spawnEnemies();
		runtime.enemy_spawn_rate = default_stat.enemy_spawn_rate;

	} else {
		// increase the probablity of spawning if enemy did not spawn
		float inc = (float)(runtime.spaceship_num_bullets) * BULLET_SPAWN_WEIGHT;
		inc += (float)(runtime.spaceship_Maxspeed - default_stat.spaceship_Maxspeed) * SPEED_SPAWN_WEIGHT;
		inc += (float)(runtime.upgr_PacmanEffect) * PACMAN_SPAWN_WEIGHT;

		runtime.enemy_spawn_rate += inc;
	}

	// manage objects movement and interactions
	physics();

	// check if the spaceship has died
	if (runtime.spaceship_health <= 0) {
		return;
	}

	// --------------------------------------------------------------------------------- Draw

	// render to framebuffer
	BeginTextureMode(frameBuffer);
	{
		// draw screen
		ClearBackground(SCREEN_BG);

		renderStars();

		pacmanEffect(spaceship_sprite);

		// draw objects
		DrawTextureV(spaceship_sprite, (Vector2){runtime.spaceship_box.x, runtime.spaceship_box.y}, WHITE);
		FOR(MAX_ENEMY) {

			if (enemies[i].x != -40 && enemies[i].y != -40) {
				DrawTextureV(Enemyship_sprite, (Vector2){enemies[i].x, enemies[i].y}, WHITE);
			}
		}

		// draw the correct upgrade
		DrawTexture(Upgrades[runtime.upgrade_type], (int)(runtime.upgrade_box.x), (int)(runtime.upgrade_box.y), WHITE);

		renderBullets();
	}
	EndTextureMode();

	BeginDrawing();
	{
		BeginShaderMode(bloomShader);
		{
			DrawTextureRec(frameBuffer.texture, (Rectangle){0.f, 0.f, (float)(frameBuffer.texture.width), (float)(-frameBuffer.texture.height)}, (Vector2){0.f, 0.f}, WHITE);
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

		notif__renderNotifications();
	}
	EndDrawing();
	//---------------------------------------------------------------------------------- End draw
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
		// save();
		resetArrays();

		resetStats();
		return;
	}

	if (WindowShouldClose() || IsKeyPressed(KEY_Q)) {
		runtime.close = true;
		// save();
		return;
	}
}

/**
 * Quick shortcut to spawn a random enemy in the top of the map
 */
void spawnEnemies() {
	addEnemies((Vector2){
	    (float)(GetRandomValue(0, screenWidth - Enemyship_sprite.height)),
	    (float)(GetRandomValue(0, 40))});
}

/**
 * Add the given enemy in the first spot available and set its health
 */
void addEnemies(Vector2 coords) {

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
			Vector2 bstart = {
			    bullets[i].x,
			    bullets[i].y};
			Vector2 bend = {
			    bullets[i].x + bullets[i].z * 2,
			    bullets[i].y + bullets[i].w * 2};

			DrawLineEx(bstart, bend, 2.5, WHITE);
		}
		if (e_bullets[i].x != -1 && e_bullets[i].y != -1) {
			// I want specifick tickness, therefore the DrawLineEx(startpos, endpos, thickness, color)

			Vector2 bstart = {
			    e_bullets[i].x,
			    e_bullets[i].y};
			Vector2 bend = {
			    e_bullets[i].x + e_bullets[i].z * 2,
			    e_bullets[i].y + e_bullets[i].w * 2};

			DrawLineEx(bstart, bend, 2.5, YELLOW);
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
		e_coolDown[i] = (char)(default_stat.e_fireCoolDown);
		enemies[i]    = DefaultShipPos;
	}
}

/**
 * Reset all the stats and upgrades
 */
void resetStats() {
	runtime = default_stat;
}

/**
 * Move, if they exist, spaceship bullets and enemy bullets
 */
void moveBullets() {

	auto deltaTime = GetFrameTime();

	// move bullets
	FOR(MAX_BULLETS) {

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
	FOR(MAX_ENEMY) {

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

		FOR(MAX_BULLETS) {
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

	FOR(MAX_BULLETS) {
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
	FOR(MAX_ENEMY) {
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

			addBullet(bullet, i);
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
		addBullet((Vector4){
		              runtime.spaceship_box.x + spaceship_width / 2,
		              runtime.spaceship_box.y + spaceship_height / 3,
		              counter,
		              DEFAULT_BULLET_SPEED},
		          -1);

		counter -= 1;
	}
}

/**
 * Teleport the spaceship at the opposite side of the screen
 */
void pacmanEffect(Texture sprite) {
	if (runtime.upgr_PacmanEffect) {
		if (runtime.spaceship_box.x < 0) {
			if (runtime.spaceship_box.x + spaceship_width / 2 < 0) {
				runtime.spaceship_box.x = runtime.spaceship_box.x + screenWidth;
			}
			DrawTexture(sprite, (int)(screenWidth + runtime.spaceship_box.x), (int)(runtime.spaceship_box.y), WHITE);
		}

		if (runtime.spaceship_box.x + spaceship_width > screenWidth) {
			if (runtime.spaceship_box.x + spaceship_width / 2 > screenWidth) {
				runtime.spaceship_box.x = runtime.spaceship_box.x - screenWidth;
			}
			DrawTexture(sprite, (int)(runtime.spaceship_box.x - screenWidth), (int)(runtime.spaceship_box.y), WHITE);
		}

		if (runtime.spaceship_box.y < 0) {
			if (runtime.spaceship_box.y + spaceship_height / 2 < 0) {
				runtime.spaceship_box.y = runtime.spaceship_box.y + screenHeight;
			}
			DrawTexture(sprite, (int)(runtime.spaceship_box.x), (int)(runtime.spaceship_box.y + screenHeight), WHITE);
		}

		if (runtime.spaceship_box.y + spaceship_height > screenHeight) {
			if (runtime.spaceship_box.y + spaceship_height / 2 > screenHeight) {
				runtime.spaceship_box.y = runtime.spaceship_box.y - screenHeight;
			}
			DrawTexture(sprite, (int)(runtime.spaceship_box.x), (int)(runtime.spaceship_box.y - screenHeight), WHITE);
		}
	} else {
		if (runtime.spaceship_box.x < 0) {
			runtime.spaceship_box.x = 0;
		}

		if (runtime.spaceship_box.x + spaceship_width > screenWidth) {
			runtime.spaceship_box.x = screenWidth - spaceship_width;
		}

		if (runtime.spaceship_box.y < 0) {
			runtime.spaceship_box.y = 0;
		}

		if (runtime.spaceship_box.y + spaceship_height > screenHeight) {
			runtime.spaceship_box.y = screenHeight - spaceship_height;
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
		res = (Vector2){-1, -1};
	}
	return res;
}

/**
 * Save the score and the name of the player in a json
 *
void save() {

    SetMouseCursor(MOUSE_CURSOR_IBEAM);

    char name[MAX_LENGHT_NAME + 1];
    memset(name, 0, sizeof(name));
    int nameIndex = 0;

    char character;
    while (true) {
        character = char(GetCharPressed());

        std::string hiscore = "Your score is : " + std::to_string(runtime.score);

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
            runtime.close = true;
            return;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            break;
        }
    }

    std::ifstream HiScores("./res/HiScores.json");

    json j;

    HiScores >> j;

    if (j["hiscores"][name] <= runtime.score) {
        j["hiscores"][name] = runtime.score;
    }

    std::ofstream oHiScores("./res/HiScores.json");

    oHiScores << std::setfill('\t') << std::setw(1) << j << std::endl;
}*/

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

	float x = (float)(GetRandomValue(0, screenWidth));
	float y = (float)(GetRandomValue(0, screenHeight));

	float speed = ((float)(GetRandomValue(0, 3)) + 1.f);
	speed *= STAR_SPEED_MULT;

	stars[index] = (Vector3){x, y, speed};
}

/**
 * Draw the right texture for each star
 */
void renderStars() {

	FOR(MAX_STAR) {

		// this can be moved to the shader
		int type = (int)(stars[i].x) % 3;

		unsigned char alpha = (unsigned char)(stars[i].z / STAR_SPEED_MULT * 30.f);
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

		float width = (float)(Star_ATL.width) / 3.f;

		Vector2 pos = {stars[i].x, stars[i].y};

		Rectangle tile = {width * (float)(type),
		                  0.f,
		                  width,
		                  (float)(Star_ATL.height)};

		DrawTextureRec(Star_ATL, tile, pos, col);
	}
}

/**
 * move the stars by their own speed
 */
void moveStars() {

	auto deltaTime = GetFrameTime();

	FOR(MAX_STAR) {
		stars[i].y += stars[i].z * STAR_SPEED_MULT * deltaTime;

		if ((stars[i].y) - (float)(Star_ATL.height) > screenHeight) {

			randomStar(i);

			// spawn the at the top of the screen
			stars[i].y = (float)(-Star_ATL.height);
		}
	}
}

/**
 * Return the time in ms since the start of the window
 */
long getCurrMs() {

	return (long)(GetTime() * 1000);
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