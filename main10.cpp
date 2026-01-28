#include <iostream>
#include <conio.h>    // _kbhit, _getch
#include <windows.h>  // Sleep, console functions
#include <cstdlib>
#include <ctime>

using namespace std;

/* ================= CONFIG ================= */
const int width  = 40;
const int height = 20;
const int delayMs = 120;
/* ========================================== */

/* ================= GAME STATE ================ */
int x, y;
int foodX, foodY;
int score;
int tailX[100], tailY[100];
int tailLen;
bool gameOver;
bool paused;
enum eDir { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDir dir;
/* ============================================ */

/* ================ CONSOLE HELPERS =========== */
void GotoXY(int x, int y) {
    COORD pos = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void HideCursor() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info = {1, FALSE};
    SetConsoleCursorInfo(console, &info);
}
/* ============================================ */

void Setup() {
    gameOver = false;
    paused = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    srand((unsigned)time(0));
    foodX = rand() % (width - 2) + 1;
    foodY = rand() % (height - 2) + 1;
    score = 0;
    tailLen = 0;
}

void Draw() {
    GotoXY(0, 0);

    // Top wall
    for (int i = 0; i < width + 2; i++) cout << "=";
    cout << "\n";

    // Map content
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) cout << "=";
            else if (i == y && j == x) cout << "O";
            else if (i == foodY && j == foodX) cout << "$";
            else {
                bool printed = false;
                for (int k = 0; k < tailLen; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";
                        printed = true;
                        break;
                    }
                }
                if (!printed) cout << " ";
            }
            if (j == width - 1) cout << "=";
        }
        cout << "\n";
    }

    // Bottom wall
    for (int i = 0; i < width + 2; i++) cout << "=";
    cout << "\n";

    cout << "Score: " << score << "\n";
    if (paused) cout << "PAUSED - Press P to Resume\n";
    cout << "Controls: W A S D | P=Pause | Q=Quit\n";
}

void Input() {
    if (_kbhit()) {
        char ch = _getch();

        if (paused) {
            if (ch == 'p' || ch == 'P') paused = false;
            return;
        }

        switch (ch) {
        case 'a': if (dir != RIGHT) dir = LEFT; break;
        case 'd': if (dir != LEFT)  dir = RIGHT; break;
        case 'w': if (dir != DOWN)  dir = UP; break;
        case 's': if (dir != UP)    dir = DOWN; break;
        case 'p': case 'P': paused = true; break;
        case 'q': case 'Q': gameOver = true; break;
        default: break;
        }
    }
}

void Logic() {
    if (paused) return;

    // Move tail
    int prevX = x, prevY = y;
    for (int i = 0; i < tailLen; i++) {
        int tempX = tailX[i];
        int tempY = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = tempX;
        prevY = tempY;
    }

    // Move head
    switch (dir) {
    case LEFT:  x--; break;
    case RIGHT: x++; break;
    case UP:    y--; break;
    case DOWN:  y++; break;
    default: break;
    }

    // Wall collision
    if (x <= 0 || x >= width - 1 || y < 0 || y >= height) gameOver = true;

    // Self collision
    for (int i = 0; i < tailLen; i++)
        if (tailX[i] == x && tailY[i] == y) gameOver = true;

    // Food collision
    if (x == foodX && y == foodY) {
        score += 10;
        foodX = rand() % (width - 2) + 1;
        foodY = rand() % (height - 2) + 1;
        tailLen++;
    }
}

void StartMenu() {
    system("cls");
    cout << "====================\n";
    cout << "      SNAKE GAME\n";
    cout << "====================\n";
    cout << "1. Start Game\n";
    cout << "2. Exit\n";
    cout << "Select option (1 or 2): ";

    char choice;
    cin >> choice;
    if (choice == '1') Setup();
    else exit(0);
}

int main() {
    HideCursor();
    StartMenu();

    while (true) {
        while (!gameOver) {
            Draw();
            Input();
            Logic();
            Sleep(delayMs);
        }

        // Game over screen
        GotoXY(0, 0);
        cout << "\nGAME OVER! Final Score: " << score << "\n";
        cout << "Press R to Restart or Q to Quit.\n";

        char choice;
        while (true) {
            if (_kbhit()) {
                choice = _getch();
                if (choice == 'r' || choice == 'R') {
                    Setup();
                    break;
                }
                else if (choice == 'q' || choice == 'Q') exit(0);
            }
            Sleep(50);
        }
    }

    return 0;
}

