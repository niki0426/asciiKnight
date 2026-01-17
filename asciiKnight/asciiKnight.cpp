/**
*
* Solution to course project # 10
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2025/2026
*
* @author Nikoleta Rasheva
* @idnumber 9MI0600618
* @compiler VC
*
* Implements an ASCII combat game.
*
*/

#include <iostream>
#include <ctime>
#include <conio.h>
#include <cstdlib>
#include <windows.h>

const int ARENA_WIDTH = 80;
const int ARENA_HEIGHT = 20;
const int NUM_PLATFORMS = 5;
const int MAX_PROJECTILES = 5;

char arena[ARENA_HEIGHT][ARENA_WIDTH];

int playerX, playerY;
int playerHP = 5;
int velocityY = 0;
bool onGround = false;
int jumpCount = 0;

int enemyCount = 0;
int* enemyX = nullptr;
int* enemyY = nullptr;
int* enemyVX = nullptr;
int* enemyVY = nullptr;
int* enemyHP = nullptr;
char* enemyType = nullptr;
bool* enemyAlive = nullptr;
int currentWave = 1;

bool bossActive = false;
int bossX, bossY, bossHP;

clock_t lastHit = 0;

int attackX, attackY;
char attackChar;
int attackHeight = 1;
char attackPattern[4][4];
bool attackActive = false;

clock_t attackTime = 0;

bool gameWon = false;
int projectileX[MAX_PROJECTILES];
int projectileY[MAX_PROJECTILES];
int projectileVX[MAX_PROJECTILES];
bool projectileActive[MAX_PROJECTILES];
int projectileVY[MAX_PROJECTILES];

void gotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
// Changes text color using Windows console attributes
void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void generatePlatforms() {
	for (int i = 0; i < NUM_PLATFORMS; i++)
	{
		int platLength = 5 + rand() % 6;
		int platX = 1 + rand() % (ARENA_WIDTH - platLength - 2);
		int platY = 3 + rand() % (ARENA_HEIGHT - 6);

		for (int x = platX; x < platX + platLength; x++)
		{
			arena[platY][x] = '=';
		}
	}
}
void initArena() {
	for (int i = 0; i < ARENA_HEIGHT; i++)
	{
		for (int j = 0; j < ARENA_WIDTH; j++)
		{
			if (i == 0 || i == ARENA_HEIGHT - 1 || j == 0 || j == ARENA_WIDTH - 1)
			{
				arena[i][j] = '#';
			}
			else
			{
				arena[i][j] = ' ';
			}
		}
	}
	generatePlatforms();
}
void renderArena() {
	for (int i = 0; i < ARENA_HEIGHT; i++) {
		for (int j = 0; j < ARENA_WIDTH; j++) {
			std :: cout << arena[i][j];
		}
		std :: cout << std :: endl;
	}
}
// Checks if coordinates (x, y) contain a wall or platform
bool solid(int x, int y) {
	return arena[y][x] == '#' || arena[y][x] == '=';
}
void initPlayer() {
	playerX = ARENA_WIDTH / 2;
	playerY = ARENA_HEIGHT / 2;
}
bool isPlatform(int x, int y) {
	return arena[y][x] == '=';
}

bool isWall(int x, int y) {
	return arena[y][x] == '#';
}
bool insideArena(int x, int y) {
	return x > 0 && x < ARENA_WIDTH && y > 0 && y < ARENA_HEIGHT;
}
void clearEnemies() {
	delete[] enemyX;
	delete[] enemyY;
	delete[] enemyVX;
	delete[] enemyVY;
	delete[] enemyHP;
	delete[] enemyType;
	delete[] enemyAlive;
}
void initEnemies(int count) {
	clearEnemies();
	enemyCount = count;

	enemyX = new int[count];
	enemyY = new int[count];
	enemyVX = new int[count];
	enemyVY = new int[count];
	enemyHP = new int[count];
	enemyType = new char[count];
	enemyAlive = new bool[count];

	char enemyTypesTemplate[4] = { 'E', 'J', 'F', 'C' };

	for (int i = 0; i < enemyCount; i++) {
		enemyAlive[i] = true;
		enemyHP[i] = 1;
		enemyType[i] = enemyTypesTemplate[i % 4];
		bool placed = false;
		while (!placed) {
			int x = 1 + rand() % (ARENA_WIDTH - 2);
			int y = 1 + rand() % (ARENA_HEIGHT - 2);

			if (arena[y][x] == ' ' && arena[y + 1][x] == '=' && arena[y][x - 1] != '#' && arena[y][x + 1] != '#') {
				enemyX[i] = x;
				enemyY[i] = y;
				enemyVX[i] = (rand() % 2 == 0 ? -1 : 1);
				enemyVY[i] = 0;
				placed = true;
			}
		}
	}
}
// Manages enemy waves and triggers boss appearance
void spawnWave() {
	if (bossActive || gameWon) return;

	if (currentWave > 3) return;

	currentWave++;

	if (currentWave == 1) {
		initEnemies(4);
	}
	else if (currentWave == 2) {
		initEnemies(5);
	}
	else if (currentWave == 3) {
		initEnemies(6);
		bossActive = true;
		bossX = ARENA_WIDTH / 2 - 1;
		bossY = 2;
		bossHP = 15;
	}
}
// Updates movement and behavior for all active enemy types
void updateEnemies() {
	for (int i = 0; i < enemyCount; i++) {
		if (!enemyAlive[i]) continue;

		if (enemyType[i] != 'F') {
			enemyVY[i]++;
			if (enemyVY[i] > 3) enemyVY[i] = 3;
		}

		int vy = enemyVY[i];
		int stepY = (vy > 0) ? 1 : -1;

		for (int s = 0; s < abs(vy); s++) {
			if (!insideArena(enemyX[i], enemyY[i] + stepY)) {
				enemyVY[i] = 0;
				break;
			}

			if (!solid(enemyX[i], enemyY[i] + stepY)) {
				enemyY[i] += stepY;
			}
			else {
				enemyVY[i] = 0;
				break;
			}
		}

		if (enemyType[i] == 'E') {
			int nx = enemyX[i] + enemyVX[i];

			if (insideArena(nx, enemyY[i]) && !isWall(nx, enemyY[i]) && isPlatform(nx, enemyY[i] + 1)) {
				enemyX[i] = nx;
			}
			else {
				enemyVX[i] *= -1;
			}
		}

		if (enemyType[i] == 'J') {
			if (abs(playerX - enemyX[i]) < 6 &&
				enemyVY[i] == 0 && (isPlatform(enemyX[i], enemyY[i] + 1) || isWall(enemyX[i], enemyY[i] + 1)))
			{
				enemyVY[i] = -4; // Jump
			}
		}

		if (enemyType[i] == 'F') {
			int dx = (playerX > enemyX[i]) ? 1 : -1;
			int dy = (playerY > enemyY[i]) ? 1 : -1;

			if (insideArena(enemyX[i] + dx, enemyY[i]))
				enemyX[i] += dx;

			if (insideArena(enemyX[i], enemyY[i] + dy))
				enemyY[i] += dy;
		}

		if (enemyType[i] == 'C') {
			if (solid(enemyX[i] + enemyVX[i], enemyY[i])) {
				enemyVX[i] *= -1;
			}
			else
			{
				enemyX[i] += enemyVX[i];
			}
		}
	}
}
void checkPlayerDamage() {
	clock_t currentTime = clock();
	// Convert CPU ticks to milliseconds
	double elapsedMs = (double)(currentTime - lastHit) * 1000.0 / CLOCKS_PER_SEC;

	if (elapsedMs < 1000) return;

	bool hit = false;
	for (int i = 0; i < enemyCount; i++) {
		// Check 3x3 area around player (distance <= 1) for enemy contact
		if (enemyAlive[i] && abs(enemyX[i] - playerX) <= 1 && abs(enemyY[i] - playerY) <= 1) {
			playerHP--;
			hit = true;
			break;
		}
	}
	// Boss collision: checks if player is inside the 3x3 Boss sprite area
	if (!hit && bossActive && playerX >= bossX && playerX < bossX + 3 && playerY >= bossY && playerY < bossY + 3) {
		playerHP -= 2;
		hit = true;
	}

	if (hit) lastHit = currentTime;
}
void bossShoot() {
	if (!bossActive || rand() % 15 != 0) {
		return;
	}
	for (int i = 0; i < MAX_PROJECTILES; i++) {
		if (!projectileActive[i]) {
			projectileX[i] = bossX + 1;
			projectileY[i] = bossY + 1;
			
			projectileVX[i] = 0;      
			projectileVY[i] = 1;      

			if (playerX < bossX) {
				projectileVX[i] = -1;
			}
			else {
				projectileVX[i] = 1;
			}

			projectileActive[i] = true;
			break;
		}
	}
}
// Updates movement and behavior for all active enemy types
void updateProjectiles() {
	for (int i = 0; i < MAX_PROJECTILES; i++) {
		if (!projectileActive[i]) continue;

		projectileX[i] += projectileVX[i];
		projectileY[i] += projectileVY[i]; 

		if (!insideArena(projectileX[i], projectileY[i]) ||
			isWall(projectileX[i], projectileY[i])) {
			projectileActive[i] = false;
			continue;
		}

		if (projectileX[i] == playerX && projectileY[i] == playerY) {
			playerHP--;
			projectileActive[i] = false;
		}
	}
}
void clearProjectilesInAttack() {
	for (int i = 0; i < MAX_PROJECTILES; i++) {
		if (!projectileActive[i]) {
			continue;
		}

		for (int h = 0; h < attackHeight; h++) {
			int ay = attackY + h;
			for (int w = 0; attackPattern[h][w] != '\0'; w++) {
				int ax = attackX + w;
				if (projectileX[i] == ax && projectileY[i] == ay) {
					projectileActive[i] = false;
				}
			}
		}
	}
}
void attack(char dir) {
	attackX = playerX;
	attackY = playerY;
	attackHeight = 1;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			attackPattern[i][j] = ' ';
		}
	}
	if (dir == 'i') {  // ↑
		attackY = playerY - 1;
		attackX = playerX - 1;
		attackPattern[0][0] = '/';
		attackPattern[0][1] = '-';
		attackPattern[0][2] = '\\';
		attackPattern[0][3] = '\0';
		attackHeight = 1;
	}
	else if (dir == 'k') {  // ↓
		attackY = playerY + 1;
		attackX = playerX - 1;
		attackPattern[0][0] = '\\';
		attackPattern[0][1] = '_';
		attackPattern[0][2] = '/';
		attackPattern[0][3] = '\0';
		attackHeight = 1;
	}
	else if (dir == 'j') {  // ←
		attackX = playerX - 1;
		attackY = playerY - 1;
		attackPattern[0][0] = '/';
		attackPattern[0][1] = '\0';
		attackPattern[1][0] = '|';
		attackPattern[1][1] = '\0';
		attackPattern[2][0] = '\\';
		attackPattern[2][1] = '\0';
		attackHeight = 3;
	}
	else if (dir == 'l') {  // →
		attackX = playerX + 1;
		attackY = playerY - 1;
		attackPattern[0][0] = '\\';
		attackPattern[0][1] = '\0';
		attackPattern[1][0] = '|';
		attackPattern[1][1] = '\0';
		attackPattern[2][0] = '/';
		attackPattern[2][1] = '\0';
		attackHeight = 3;
	}
	attackTime = clock();
	attackActive = true;

	clearProjectilesInAttack();

	for (int i = 0; i < enemyCount; i++) {
		if (!enemyAlive[i]) continue;

		for (int h = 0; h < attackHeight; h++) {
			int ay = attackY + h;
			for (int w = 0; attackPattern[h][w] != '\0'; w++) {
				int ax = attackX + w;
				if (enemyX[i] == ax && enemyY[i] == ay) {
					enemyAlive[i] = false;
				}
			}
		}
	}
	if (bossActive) {
		for (int h = 0; h < attackHeight; h++) {
			int ay = attackY + h;
			for (int w = 0; attackPattern[h][w] != '\0'; w++) {
				int ax = attackX + w;
				if (ax >= bossX && ax < bossX + 3 && ay >= bossY && ay < bossY + 3) {
					bossHP--;
					if (bossHP <= 0) bossActive = false;
				}
			}
		}
	}
	if (bossHP <= 0) {
		bossActive = false;
	}
}
// Handles gravity, jumping logic, and vertical collisions
void updatePhysics()
{
	if (!onGround)
	{
		velocityY++;

		if (velocityY > 3) velocityY = 3; // Terminal velocity (max fall speed)

		int steps = abs(velocityY);
		int direction = (velocityY > 0) ? 1 : -1;

		for (int i = 0; i < steps; i++) {
			int nextY = playerY + direction;

			if (!insideArena(playerX, nextY)) {
				velocityY = 0;
				break;
			}

			if (!solid(playerX, nextY)){
				playerY = nextY;
			}
			else{
				if (direction == -1) velocityY = 0;

				else if (direction == 1) {
					onGround = true;
					jumpCount = 0;
					velocityY = 0;
				}
				break;
			}
		}
	}

	if (onGround && !solid(playerX, playerY + 1))
		onGround = false;
}

void input()
{
	while (_kbhit()) {
		char c = _getch();
		if (c == 'a' && !solid(playerX - 1, playerY)) {
			playerX--;
			if (!solid(playerX, playerY + 1)) {
				onGround = false;
				jumpCount = 2;
			}
		} 
		if (c == 'd' && !solid(playerX + 1, playerY)) {
			playerX++;
			if (!solid(playerX, playerY + 1)) {
				onGround = false;
				jumpCount = 2;
			}
		}

		if (c == 'w' && jumpCount < 2) {

			if (!solid(playerX, playerY - 1)) {
				jumpCount++;
				onGround = false;
				velocityY = -5;
			}
		}
		if (c == 'k') {
			if (!solid(playerX, playerY + 1)) {
				attack(c);
			}
		}
		else if (c == 'i' || c == 'j' || c == 'l') {
			attack(c);
		}
	}
}
bool allEnemiesDead()
{
	if (enemyCount == 0) return true;

	for (int i = 0; i < enemyCount; i++)
	{
		if (enemyAlive[i]) return false;
	}
	return true;
}
// Determines the symbol and color priority for each cell
void determineCellVisuals(int x, int y, char& symbol, int& nextAttr) {
	bool found = false;

	for (int i = 0; i < MAX_PROJECTILES; i++) {
		if (projectileActive[i] && projectileX[i] == x && projectileY[i] == y) {
			symbol = '*';
			nextAttr = 14; // Yellow
			found = true;
			break;
		}
	}
	if (found) return;

	for ( int e = 0; e < enemyCount; e++ ) {
		if ( enemyAlive[e] && enemyX[e] == x && enemyY[e] == y ) {
			symbol = enemyType[e];
			// E: 12 (Bright Red), J: 10 (Bright Green), Others: 11 (Bright Cyan)
			nextAttr = ( symbol == 'E') ? 12 : (symbol == 'J' ? 10 : 11 );
			found = true;
			break;
		}
	}
	if ( !found && attackActive ) {
		for ( int h = 0; h < attackHeight; h++ ) {
			if ( attackY + h == y ) {
				for ( int w = 0; attackPattern[h][w] != '\0'; w++ ) {
					if ( attackX + w == x && !isWall(x, y) ) {
						symbol = attackPattern[h][w];
						nextAttr = 15; // White
						found = true;
						break;
					}
				}
			}
			if (found) {
				break;
			}
		}
	}
	if (!found) {
		// 8: Dark Gray (Walls/Platforms), 7: Light Gray (Empty space/Background)
		nextAttr = (symbol == '#' || symbol == '=') ? 8 : 7;
	}
}
void render() {
	gotoXY(0, 0);
	setColor(15); std :: cout << "HP: " << playerHP;
	setColor(15); std :: cout << "   Wave: " << currentWave << "          \n";

	int currentAttr = 7; // Lightgray
	setColor(currentAttr);
	// Auto-disable attack visuals after 200ms duration
	if (attackActive && clock() - attackTime > 200) {
		attackActive = false;
	}

	for ( int i = 0; i < ARENA_HEIGHT; i++ ) {
		for ( int j = 0; j < ARENA_WIDTH; j++ ) {
			int nextAttr = 7;
			char symbol = arena[i][j];

			if ( i == playerY && j == playerX ) {
				symbol = '@'; nextAttr = 15; // White
			}
			else if ( bossActive && j >= bossX-1 && j < bossX + 3 && i >= bossY && i < bossY + 3 ) {
				symbol = 'B'; nextAttr = 14;
			}
			else
			{
				determineCellVisuals( j, i, symbol, nextAttr );
			}
			if ( nextAttr != currentAttr ) {
				setColor(nextAttr);
				currentAttr = nextAttr;
			}
			std :: cout << symbol;
		}
		std :: cout << "\n";
	}
}
void gameLoop() {
	currentWave = 0;
	bossActive = false;
	gameWon = false;
	lastHit = clock();

	while (playerHP > 0 && !gameWon) {
		updateEnemies();
		if (bossActive) {
			bossShoot();
			updateProjectiles();

			// Random Boss movement
			if (rand() % 10 == 0) {
				int moveY = (rand() % 2 == 0) ? 1 : -1; // Choose Up or Down
				if (insideArena(bossX, bossY + moveY)) {
					bossY += moveY;
				}
			}
		}
		input();
		updatePhysics();
		//checkPlayerDamage();

		if (!bossActive && allEnemiesDead()) {
			spawnWave();
		}
		if (currentWave > 3 && allEnemiesDead() && !bossActive) {
			gameWon = true;
		}
		if (bossActive && bossHP <= 0) {
			bossActive = false;
		}

		render();
		Sleep(80);
	}
}
void endConditions() {
	if (gameWon) {
		gotoXY(ARENA_WIDTH / 2 - 4, ARENA_HEIGHT / 2);
		setColor(10);
		std::cout << "YOU WON!";
	}
	else if (playerHP <= 0) {
		gotoXY(ARENA_WIDTH / 2 - 4, ARENA_HEIGHT / 2);
		setColor(12);
		std::cout << "GAME OVER";
	}
}
int main()
{
	srand((unsigned)time(0));
	initArena();
	initPlayer();

	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

	gameLoop();
	endConditions();
}
