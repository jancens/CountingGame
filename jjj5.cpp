#include <iostream>
#include <conio.h>
#include <vector>
#include <chrono>
#include <windows.h>
#include <cstdlib>
#include <ctime>

// area size
const int hSize = 20;
const int vSize = 20;
// colours
const int colourEntity = 10;
const int colourObjects = 14;
const int colourDefault = 7;
// randomness probalities in percent
const int probStone = 5;
const int probNumber = 10;
const int multiplierStoneAdjacent = 6;
const int multiplierNumberAdjacent = 2;


class Entity {
private:
    int hPosition;
    int vPosition;
    char symbol;
    void addS(char c, int & score) {
        if (c >= 48 && c <= 57) {
            symbol += (c-48);
            if (symbol > 57) {
                symbol -= 10;
                score += 10;
            }
        }
    }
public:
    Entity() {
        hPosition = 0;
        vPosition = 0;
        symbol = ' ';
    }
    Entity(int h, int v, char s) {
        hPosition = h;
        vPosition = v;
        symbol = s;
    }
    bool walk(int h, int v, char (& frame)[vSize][hSize], int & score) {
        int hNew = hPosition + h;
        int vNew = vPosition + v;
        if (!canWalk(hNew, vNew, frame)) {
            return false;
        } else {
            addS(frame[vNew][hNew], score);
            frame[vPosition][hPosition] = ' ';
            frame[vNew][hNew] = getS();
            hPosition = hNew;
            vPosition = vNew;
            return true;
        }
    }
    void reposition() {
        if (vPosition < vSize - 1) {
            vPosition++;
        }
    }
    bool canWalk(int hNew, int vNew, char (& frame)[vSize][hSize]){
        if (hNew >= hSize || hNew < 0 || vNew >= vSize || vNew < 0 || frame[vNew][hNew] == '*') {
            return false;
        }
        return true;
    }
    bool canWalk(char dir, char (& frame)[vSize][hSize]){
        int hNew = hPosition;
        int vNew = vPosition;
        if (dir == 'u') {
            vNew--;
        } else if (dir == 'r') {
            hNew++;
        } else if (dir == 'd') {
            vNew++;
        } else if (dir == 'l') {
            hNew--;
        } else {
            return false;
        }
        if (hNew >= hSize || hNew < 0 || vNew >= vSize || vNew < 0 || frame[vNew][hNew] == '*') {
            return false;
        }
        return true;
    }
    int getH() {
        return hPosition;
    }
    int getV() {
        return vPosition;
    }
    char getS() {
        return symbol;
    }
};

class Grid {
private:
public:
};

void clear() {
    COORD topLeft  = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}

void draw(const char (& fr)[vSize][hSize], const int & score, HANDLE & hConsole, int entV, int entH) {
    // clear screen
    clear();
    // draw
    SetConsoleTextAttribute(hConsole, colourDefault);
    std::cout << "Press ESC to quit" << std::endl;
    for (int h = 0; h < hSize+2; h++) {
        std::cout << '*';
    }
    int scoreLine = vSize / 2;
    std::cout << std::endl;
    for (int v = 0; v < vSize; v++) {
        std::cout << '*';
        for (int h = 0; h < hSize; h++) {
            if (entV == v && entH == h) {
                SetConsoleTextAttribute(hConsole, colourEntity);
                std::cout << fr[v][h];
                SetConsoleTextAttribute(hConsole, colourDefault);
            } else if (fr[v][h] != '*' && fr[v][h] != ' ') {
                SetConsoleTextAttribute(hConsole, colourObjects);
                std::cout << fr[v][h];
                SetConsoleTextAttribute(hConsole, colourDefault);
            } else {
                std::cout << fr[v][h];
            }
        }
        std::cout << '*' << v;
        if (v == scoreLine) {
            std::cout << "\t\tScore: " << score;
        }
        std::cout << std::endl;
    }
    SetConsoleTextAttribute(hConsole, colourDefault);
    for (int h = 0; h < hSize+2; h++) {
        std::cout << '*';
    }
    std::cout << std::endl << std::endl;
}

char randSymbol(int v, int h, const char (& frame)[vSize][hSize], const int & ticks) {
    //
    int roll, stoneLimit, numberLimit;
    stoneLimit = probStone + ticks / 15;
    numberLimit = probNumber;
    if (v == 0) {
        // top line
        if (frame[v+1][h] == '*') {
            stoneLimit *= multiplierStoneAdjacent - ticks / 60;
        }
        if (frame[v+1][h] >= '0' && frame[v+1][h] <= '9') {
            numberLimit *= multiplierNumberAdjacent;
        }
    } else if (v == vSize - 1) {
        // bottom line
        if (frame[v+1][h] == '*' || frame[v-1][h] == '*') {
            stoneLimit *= multiplierStoneAdjacent - ticks / 60;
        }
        if ((frame[v+1][h] >= '0' && frame[v+1][h] <= '9')
        || (frame[v-1][h] >= '0' && frame[v+1][h] <= '9')) {
            numberLimit *= multiplierNumberAdjacent;
        }
    } else {
        // everything else in the middle
        if (frame[v-1][h] == '*') {
            stoneLimit *= multiplierStoneAdjacent - ticks / 60;
        }
        if (frame[v-1][h] >= '0' && frame[v+1][h] <= '9') {
            numberLimit *= multiplierNumberAdjacent;
        }
    }
    roll = 1 + std::rand()/((RAND_MAX + 1u)/100);
    while(roll > 100) {
        roll = 1 + std::rand()/((RAND_MAX + 1u)/100);
    }
    if (roll >= 100 - numberLimit) {
        // it's a number! which one?
        roll = std::rand()/((RAND_MAX + 1u)/10);
        while(roll > 10) {
            roll = std::rand()/((RAND_MAX + 1u)/10);
        }
        return '0' + roll;
    } else if (roll <= stoneLimit) {
        // it's a stone!
        return '*';
    } else {
        // it's blank!
        return ' ';
    }
}

void genLine(int linePos, char (& frame)[vSize][hSize], const int & ticks) {
    // generate new line
    for (int h = 0; h < hSize; h++) {
        //frame[linePos][h] = ' ';
        frame[linePos][h] = randSymbol(linePos, h, frame, ticks);
    }
}

void gameOver(int score) {
    //
    exit(score);
}

void moveFrame(char (& frame)[vSize][hSize], std::vector<Entity> & entList, int & score, const int & ticks) {
    // check on game-ending situation (Entity is on bottom line and not able to move up)
    // if able to move up from bottom line, will do so, otherwise will stay in same place
    bool canWalkUp = entList[0].canWalk('u', frame);
    if (entList[0].getV() == vSize - 1) {
        if (!canWalkUp) {
            gameOver(score);
        }
    }

    // Entity stays in the same place, if able, otherwise gets moved down with the rest of the frame
    if (!canWalkUp && entList[0].getV() != 0) {
        entList[0].reposition();
    }

    // clear Entity from frame
    frame[entList[0].getV()][entList[0].getH()] = ' ';

    // move every line of frame down by one, Entity stays in the same place
    for (int v1, v = vSize - 1; v > 0; v--) {
        for (int h = 0; h < hSize; h++) {
            v1 = v - 1;
            frame[v][h] = frame[v1][h];
        }
    }

    // generate new line at the top
    genLine(0, frame, ticks);

    // if generated line has * in same spot as Entity, Entity gets pushed down
    if (entList[0].getV() == 0 && frame[0][entList[0].getH()] == '*') {
        entList[0].reposition();
    }

    // redraw Entity in frame
    frame[entList[0].getV()][entList[0].getH()] = entList[0].getS();
}

int main() {

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    std::vector<Entity> entList;
    int score = 0;
    int ch, ch2, x, y, ticks = 0;

    std::srand(std::time(nullptr)); // use current time as seed for random generator

    entList.emplace_back(1,2,'0');

    char frame[vSize][hSize];
    // initialize frame
    for (int v = 0; v < vSize; v++) {
        genLine(v, frame, ticks);
    }
    // set objects in frame
    for (auto& it : entList) {
        frame[it.getV()][it.getH()] = it.getS();
    }

    draw(frame, score, hConsole, entList[0].getV(), entList[0].getH());

    // main loop
    while (true) {
        ticks++;
        if ((ch = _getch()) == 27) /* 27 = Esc key */ {
            // stop process if ESC is pressed
            break;
        }
        //printf("%d", ch);
        if (ch == 0 || ch == 224) {
            //printf (", %d", _getch ());
            ch2 = _getch();
            switch (ch2) {
            case 72: // up
                x = 0;
                y = -1;
                break;
            case 75: // left
                x = -1;
                y = 0;
                break;
            case 77: // right
                x = 1;
                y = 0;
                break;
            case 80: // down
                x = 0;
                y = 1;
                break;
            default:
                // this shouldn't happen
                x = 0;
                y = 0;
                std::cout << ch2;
                break;
            }
            if (entList[0].walk(x, y, frame, score)) {
                draw(frame, score, hConsole, entList[0].getV(), entList[0].getH());
            }
        }
        //Sleep(1000);
        if (ticks % 3 == 0) {
            // move line down every 3 ticks
            moveFrame(frame, entList, score, ticks);
            draw(frame, score, hConsole, entList[0].getV(), entList[0].getH());
        }
    }

    return 0;
}
