#include <iostream>
#include <ctime>
#include <conio.h>
#include <cstdlib>
#include <windows.h>

using namespace std;

const int WIDTH=80;
const int HEIGHT=20;

char arena[HEIGHT][WIDTH];

int playerX, playerY;
int playerHP = 5;
int velocityY = 0;
bool onGround = false;
int jumpCount = 0;

int enemyCount;
int *enemyX;
int *enemyY;
char *enemyType;
bool *enemyAlive;

clock_t lastHit = 0;

void clearScreen()
{
    system("cls");
}
void initArena()
{
    for(int i=0; i<HEIGHT; i++)
    {
        for(int j=0; j<WIDTH; j++)
        {
            if(i==0 || i==HEIGHT-1 || j==0 || j==WIDTH-1)
            {
                arena[i][j] = '#';
            }
            else
            {
                arena[i][j] = ' ';
            }
        }
    }
    int platformStart = WIDTH/4;
    int platformEnd = 2*(WIDTH/4);
    int platformHeight = HEIGHT-6;

    for(int i=platformStart; i<platformEnd; i++)
    {
        arena[platformHeight][i] = '=';
    }
}
void renderArena()
{
    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            cout << arena[i][j];
        }
        cout << endl;
    }
}
int playerX;
int playerY;
int playerHP = 5;

void initPlayer()
{
    playerX = WIDTH / 2;
    playerY = HEIGHT / 2;
}
void render()
{
    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            if(i == playerY && j == playerX)
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
    clearScreen();
    initArena();

    initPlayer();
    render();

}
