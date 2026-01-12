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

void spawnEnemies()
{
    enemyCount;
   for(int i = 0; i < enemyCount; i++)
   {
       if (enemyType[i] == 'E')
       {
           if (solid(enemyX[i], enemyY[i] + 1))
           {
               if (!solid(enemyX[i] + enemyVX[i], enemyY[i]))
               {
                   if (solid(enemyX[i] + enemyVX[i], enemyY[i] + 1))
                   {
                       enemyX[i] += enemyVX[i];
                   }
                   else
                   {
                       enemyVX[i] *= -1;
                   }
               }
               else
               {
                   enemyVX[i] *= -1;
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
        }
        if (c == 'd' && !solid(playerX + 1, playerY))
        {
            playerX++;
        }

        if (c == 'w' && jumpCount < 2) {
            jumpCount++;
            onGround = false;
            velocityY = -2;
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
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (i == playerY && j == playerX)
                cout << '@';
            else
                cout << arena[i][j];
        }
        cout << endl;
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

    while (true)
    {
        input();
        updatePhysics();
        render();
        Sleep(60);
    }
}