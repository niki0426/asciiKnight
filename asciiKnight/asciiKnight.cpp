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


clock_t lastHit = 0;

void gotoXY(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void generatePlatforms()
{
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
void initArena()
{
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
int enemyCount = 0;
int* enemyX = nullptr;
int* enemyY = nullptr;
int* enemyVX = nullptr;
int* enemyVY = nullptr;
int* enemyHP = nullptr;
char* enemyType = nullptr;
bool* enemyAlive = nullptr;
int currentWave = 1;

// Бос характеристики
bool bossActive = false;
int bossX, bossY, bossHP;

bool isPlatform(int x, int y)
{
    return arena[y][x] == '=';
}

bool isWall(int x, int y)
{
    return arena[y][x] == '#';
}
void spawnWave() {
    if (wave >= 5) { // На вълна 5 идва босът
        bossActive = true;
        bossX = 10; bossY = 5; bossHP = 15;
        return;
    }

    // Случайно добавяне на 2 до 4 нови врага
    int newEnemies = 2 + (rand() % 3);
    for (int i = 0; i < newEnemies && currentEnemyCount < MAX_ENEMIES; i++) {
        enemyX[currentEnemyCount] = rand() % 20 + 1;
        enemyY[currentEnemyCount] = rand() % 10 + 1;
        enemyActive[currentEnemyCount] = true;
        currentEnemyCount++;
    }
    wave++;
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

    for (int i = 0; i < enemyCount; i++)
    {
        enemyAlive[i] = true;
        enemyHP[i] = 1;

        int r = rand() % 4;
        enemyType[i] = (r == 0 ? 'E' : r == 1 ? 'J' : r == 2 ? 'F' : 'C');

        enemyX[i] = 2 + rand() % (WIDTH - 4);
        enemyY[i] = 2 + rand() % (HEIGHT - 6);

        enemyVX[i] = (rand() % 2 == 0 ? -1 : 1);
        enemyVY[i] = 0;
        if (enemyType[i] == 'E')
        {
            spawnE(i);
        }

    }
}
bool insideArena(int x, int y)
{
    return x > 0 && x < WIDTH && y > 0 && y < HEIGHT;
}

void updateEnemies()
{
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemyAlive[i]) continue;

        if (enemyType[i] != 'F')
        {
            enemyVY[i]++;
            if (enemyVY[i] > 3) enemyVY[i] = 3;
        }

        int vy = enemyVY[i];
        int stepY = (vy > 0) ? 1 : -1;

        for (int s = 0; s < abs(vy); s++)
        {
            if (!insideArena(enemyX[i], enemyY[i] + stepY))
            {
                enemyVY[i] = 0;
                break;
            }

            if (!solid(enemyX[i], enemyY[i] + stepY))
                enemyY[i] += stepY;
            else
            {
                enemyVY[i] = 0;
                break;
            }
        }

        if (enemyType[i] == 'E')
        {
            int nx = enemyX[i] + enemyVX[i];

            if (insideArena(nx, enemyY[i]) &&
                !isWall(nx, enemyY[i]) &&
                isPlatform(nx, enemyY[i] + 1))
            {
                enemyX[i] = nx;
            }
            else
            {
                enemyVX[i] *= -1;
            }
        }

        if (enemyType[i] == 'J')
        {
            if (abs(playerX - enemyX[i]) < 6 &&
                enemyVY[i] == 0 /*&&
                isPlatform(enemyX[i], enemyY[i] + 1)*/)
            {
                enemyVY[i] = -4;
            }
        }

        if (enemyType[i] == 'F')
        {
            int dx = (playerX > enemyX[i]) ? 1 : -1;
            int dy = (playerY > enemyY[i]) ? 1 : -1;

            if (insideArena(enemyX[i] + dx, enemyY[i]))
                enemyX[i] += dx;

            if (insideArena(enemyX[i], enemyY[i] + dy))
                enemyY[i] += dy;
        }

        if (enemyType[i] == 'C')
        {
            if (isWall(enemyX[i] + enemyVX[i], enemyY[i]))
                enemyVX[i] *= -1;
            else
                enemyX[i] += enemyVX[i];
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
void attack(char dir)
{
    int tx = playerX;
    int ty = playerY;

    if (dir == 'i') ty--;
    if (dir == 'k') ty++;
    if (dir == 'j') tx--;
    if (dir == 'l') tx++;

    attackX = tx;
    attackY = ty;
    attackChar = '*';
    attackTime = clock();

    for (int i = 0; i < enemyCount; i++)
    {
        if (enemyAlive[i] &&
            enemyX[i] == tx &&
            enemyY[i] == ty)
        {
            gotoXY(0, HEIGHT + 3);
            cout << "HIT ENEMY " << i << "   ";
            enemyAlive[i] = false;
            return;
        }
    }
}

void checkPlayerDamage()
{
    for (int i = 0; i < enemyCount; i++)
    {
        if (enemyAlive[i] && enemyX[i] == playerX && enemyY[i] == playerY)
        {
            if (clock() - lastHit > 500)
            {
                playerHP--;
                lastHit = clock();
            }
        }
    }

    if (bossActive &&
        playerX >= bossX && playerX < bossX + 3 &&
        playerY >= bossY && playerY < bossY + 3)
    {
        if (clock() - lastHit > 500)
        {
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
void input()
{
    char c = _getch(); // БЛОКИРА – чака клавиш

    system("cls");
    cout << "KEY CODE: " << (int)c << endl;

    if (c == 27) exit(0); // ESC за изход
}
void render()
{
    gotoXY(0, 0);
    cout << "HP: " << playerHP << endl;

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            bool drawn = false;
            if (clock() - attackTime < 300 && x == attackX && y == attackY)
            {
                cout << attackChar;
                continue;
            }

            bool drawn = false;

            // Играч
            if (x == playerX && y == playerY)
            {
                cout << '@';
                continue;
            }

            // Boss
            if (bossActive &&
                x >= bossX && x < bossX + 3 &&
                y >= bossY && y < bossY + 3)
            {
                cout << 'B';
                continue;
            }

            // Врагове
            for (int i = 0; i < enemyCount; i++)
            {
                if (enemyAlive[i] && enemyX[i] == x && enemyY[i] == y)
                {
                    cout << enemyType[i];
                    drawn = true;
                    break;
                }
            }

            // АТАКА – РИСУВА СЕ НАЙ-ОТГОРЕ
            if (!drawn &&
                clock() - attackTime < 300 &&
                x == attackX && y == attackY)
            {
                cout << attackChar;
                continue;
            }

            // Arena
            if (!drawn)
                cout << arena[y][x];
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
        input();
        updatePhysics();
        updateEnemies();
        checkPlayerDamage();
        render();

        bool allDead = true;
        for (int i = 0; i < enemyCount; i++)
            if (enemyAlive[i]) allDead = false;

        if (allDead && !bossActive)
        {
            wave++;
            if (wave < 5)
            {
                delete[] enemyX;
                delete[] enemyY;
                delete[] enemyVX;
                delete[] enemyVY;
                delete[] enemyType;
                delete[] enemyAlive;
                initEnemies(enemyCount + (2 + rand() % 3));
            }
            else
            {
                bossActive = true;
                bossX = WIDTH / 2 - 1;
                bossY = 3;
                bossHP = 15;
            }
        }

        if (playerHP <= 0)
        {
            system("cls");
            cout << "GAME OVER\n";
            break;
        }

        if (!bossActive && wave >= 5)
        {
            system("cls");
            cout << "YOU WIN!\n";
            break;
        }

        Sleep(60);
    }
}