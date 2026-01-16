#include <iostream>
#include <ctime>
#include <conio.h>
#include <cstdlib>
#include <windows.h>

using namespace std;

const int WIDTH = 80;
const int HEIGHT = 20;

char arena[HEIGHT][WIDTH];
const int NUM_PLATFORMS = 5;

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

void gotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void generatePlatforms() {
	for (int i = 0; i < NUM_PLATFORMS; i++)
	{
		int platLength = 5 + rand() % 6;
		int platX = 1 + rand() % (WIDTH - platLength - 2);
		int platY = 3 + rand() % (HEIGHT - 6);

		for (int x = platX; x < platX + platLength; x++)
		{
			arena[platY][x] = '=';
		}
	}
}
void initArena() {
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
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
void renderArena()
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			cout << arena[i][j];
		}
		cout << endl;
	}
}
bool solid(int x, int y)
{
	return arena[y][x] == '#' || arena[y][x] == '=';
}
//Player
void initPlayer()
{
	playerX = WIDTH / 2;
	playerY = HEIGHT / 2;
}
bool isPlatform(int x, int y)
{
	return arena[y][x] == '=';
}

bool isWall(int x, int y)
{
	return arena[y][x] == '#';
}
bool insideArena(int x, int y)
{
	return x > 0 && x < WIDTH && y > 0 && y < HEIGHT;
}
void spawnE(int i)
{
	while (true)
	{
		int x = 1 + rand() % (WIDTH - 2);
		int y = 1 + rand() % (HEIGHT - 2);

		if (arena[y][x] != ' ')
			continue;

		if (arena[y + 1][x] != '=')
			continue;

		if (arena[y][x - 1] == '#' || arena[y][x + 1] == '#')
			continue;

		enemyX[i] = x;
		enemyY[i] = y;
		enemyVX[i] = (rand() % 2 == 0) ? -1 : 1;
		enemyVY[i] = 0;
		return;
	}
}

void initEnemies(int count)
{
	enemyCount = count;

	enemyX = new int[enemyCount];
	enemyY = new int[enemyCount];
	enemyVX = new int[enemyCount];
	enemyVY = new int[enemyCount];
	enemyHP = new int[enemyCount];
	enemyType = new char[enemyCount];
	enemyAlive = new bool[enemyCount];

	char enemyTypesTemplate[4] = { 'E', 'J', 'F', 'C' };

	for (int i = 0; i < enemyCount; i++)
	{
		enemyAlive[i] = true;
		enemyHP[i] = 1;

		enemyType[i] = enemyTypesTemplate[i % 4];

		enemyX[i] = 2 + rand() % (WIDTH - 4);
		enemyY[i] = 2 + rand() % (HEIGHT - 6);

		enemyVX[i] = (rand() % 2 == 0 ? -1 : 1);
		enemyVY[i] = 0;
		if (enemyType[i] == 'E') {
			spawnE(i);
		}
	}
}
void spawnWave() {
	if (bossActive) return;

	if (currentWave >= 5) {
		bossActive = true;
		bossX = WIDTH / 2 - 1;
		bossY = 2;
		bossHP = 15;
		return;
	}

	int newEnemies = 2 + rand() % 3 + currentWave;
	initEnemies(newEnemies);
	currentWave++;
}
void updateEnemies()
{
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
				enemyVY[i] = -4;
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
			if (solid(enemyX[i] + enemyVX[i], enemyY[i] + enemyVY[i]))
			{
				enemyVX[i] *= -1;
				enemyVY[i] *= -1;
			}
			else
			{
				enemyX[i] += enemyVX[i];
				enemyY[i] += enemyVY[i];
			}
		}
	}
}
void attack(char dir) {
	attackX = playerX;
	attackY = playerY;
	attackHeight = 1;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
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
}

void updatePhysics()
{
	if (!onGround)
	{
		velocityY++;
		int nextY = playerY + velocityY;
		for (int i = 0; i < velocityY; i++)
		{
			if (solid(playerX, playerY + i)) {

				onGround = true;
				jumpCount = 0;
				velocityY = 0;
				nextY = playerY + i - 1;
				break;
			}
		}
		playerY = nextY;
	}
}
void input()
{
	while (_kbhit()) {
		char c = _getch();
		if (c == 'a' && !solid(playerX - 1, playerY))
		{
			playerX--;
			if (!solid(playerX, playerY + 1))
			{
				onGround = false;
			}
		}
		if (c == 'd' && !solid(playerX + 1, playerY))
		{
			playerX++;
			if (!solid(playerX, playerY + 1))
			{
				onGround = false;
			}
		}

		if (c == 'w' && jumpCount < 2) {
			jumpCount++;
			onGround = false;
			velocityY = -4;
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
void checkPlayerDamage() {
	for (int i = 0; i < enemyCount; i++) {
		if (enemyAlive[i] && enemyX[i] == playerX && enemyY[i] == playerY) {
			if (clock() - lastHit > 500) {
				playerHP--;
				lastHit = clock();
			}
		}
	}

	if (bossActive &&
		playerX >= bossX && playerX < bossX + 3 &&
		playerY >= bossY && playerY < bossY + 3) {
		if (clock() - lastHit > 500) {
			playerHP -= 2;
			lastHit = clock();
		}
	}
}

bool allEnemiesDead()
{
	for (int i = 0; i < enemyCount; i++)
	{
		if (enemyAlive[i])
		{
			return false;
		}
	}
	return true;
}

void render()
{
	gotoXY(0, 0);
	cout << "HP: " << playerHP << endl;
	bool attackDrawn = false;

	if (attackActive && clock() - attackTime > 200) {
		attackActive = false;
	}
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			bool drawn = false;

			if (i == playerY && j == playerX)
			{
				cout << '@';
				continue;
			}

			if (bossActive && j >= bossX && j < bossX + 3 && i >= bossY && i < bossY + 3)
			{
				cout << 'B';
			}
			else
			{
				for (int e = 0; e < enemyCount; e++)
				{
					if (enemyAlive[e] && enemyX[e] == j && enemyY[e] == i)
					{
						cout << enemyType[e];
						drawn = true;
						break;
					}
				}
			}
			bool attackDrawn = false;

			if (attackActive) {
				for (int h = 0; h < attackHeight; h++) {
					int ay = attackY + h;
					for (int w = 0; attackPattern[h][w] != '\0'; w++) {
						int ax = attackX + w;
						if (ax == j && ay == i) {
							cout << attackPattern[h][w];
							attackDrawn = true;
							break;
						}
						if (ax == j && ay == i && !isWall(ax, ay)) {
							cout << attackPattern[h][w];
							attackDrawn = true;
							break;
						}
					}
					if (attackDrawn) break;
				}
			}

			if (attackDrawn) continue;

			if (!drawn) cout << arena[i][j];
		}
		cout << endl;
	}

}

int main()
{
	srand((unsigned)time(0));
	initArena();
	initPlayer();
	initEnemies(6);

	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

	while (true)
	{
		updateEnemies();
		input();
		updatePhysics();

		if (allEnemiesDead() && !bossActive) {
			spawnWave();
		}
		render();
		Sleep(60);
	}
}
