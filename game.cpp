#include "game.h"

#include <algorithm>
#include <random>
#include <vector>

using namespace std;

Game::Game(int rows, int cols, int minesCount) {
    mines.resize(cols, vector<bool>(rows, false));
    state.resize(cols, vector<GameState>(rows, CLOSED));

    lost = false;
    numOfMines = flags = minesCount;
    opened = 0;
    minesInstalled = false;
}

void Game::installMines(int forbidden_x, int forbidden_y) {
    vector<pair<int, int>> points;
    int cols = mines.size();
    int rows = mines[0].size();
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            if (x != forbidden_x || y != forbidden_y) {
                points.push_back(make_pair(x, y));
            }
        }
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(points.begin(), points.end(), g);
    for (int i = 0, n = min(numOfMines, rows * cols); i < n; i++) {
        mines[points[i].first][points[i].second] = true;
    }
    minesInstalled = true;
}

int Game::neighbors(int x, int y) const {
    int cnt = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int x0 = x + dx, y0 = y + dy;
            if (dx == 0 && dy == 0) {
                continue;
            }
            if (x0 >= 0 && y0 >= 0 && x0 < (int)mines.size() &&
                y0 < (int)mines[x0].size()) {
                if (mines[x0][y0]) {
                    cnt++;
                }
            }
        }
    }
    return cnt;
}

char Game::cell(int x, int y) const {
    GameState st = state[x][y];
    if (lost && mines[x][y]) {
        return 'B';
    }
    if (st == CLOSED) {
        return '*';
    } else if (st == MARKED) {
        return 'F';
    } else if (st == MAYBE) {
        return '?';
    } else {
        int n = neighbors(x, y);
        if (n == 0) {
            return ' ';
        } else {
            return (char)('0' + n);
        }
    }
}

void Game::open(int x, int y) {
    if (x < 0 || y < 0 || x >= (int)mines.size() || y >= (int)mines[0].size()) {
        return;
    }
    if (!minesInstalled) {
        installMines(x, y);
    }
    if (state[x][y] == OPENED) {
        return;
    }
    while (state[x][y] != CLOSED) {
        changeState(x, y);
    }
    state[x][y] = OPENED;
    opened++;
    if (mines[x][y]) {
        lost = true;
        return;
    }
    int n = neighbors(x, y);
    if (n == 0) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                open(x + dx, y + dy);
            }
        }
    }
}

void Game::changeState(int x, int y) {
    GameState curSt = state[x][y];
    GameState nextSt = nextState(curSt);
    if (nextSt == MARKED) {
        if (flags == 0) {
            return;
        } else {
            flags--;
        }
    }
    if (curSt == MARKED) {
        flags++;
    }
    state[x][y] = nextSt;
}

GameState Game::nextState(GameState st) {
    switch (st) {
    case CLOSED:
        return MARKED;
    case MARKED:
        return MAYBE;
    case MAYBE:
        return CLOSED;
    default:
        return OPENED;
    }
}

int Game::flagsLeft() const { return flags; }

bool Game::win() const {
    return !lost &&
           ((opened + numOfMines) == (int)mines.size() * (int)mines[0].size());
}

bool Game::lose() const { return lost; }

bool Game::isBomb(int x, int y) const { return mines[x][y]; }
