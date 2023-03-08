#include "aho_corasick.h"
#include "const.h"
#include "game.h"
#include <functional>
#include <ncurses.h>
#include <queue>
#include <vector>

using namespace std;

const int NORMAL = 0;
const int BOMB = 1;
const int WIN = 2;
const int FLAG = 3;
const int GODMODE_BOMB = 4;
const int BORDER = 5;
const int DIGIT = 6;
const int QUESTION_MARK = 7;

using AsyncTask = std::function<void()>;

int getGameLevel() {
    int level = 0;
    move(1, 0);
    printw("1 - easy (%d x %d, %d mines)\n"
           "2 - medium (%d x %d, %d mines)\n"
           "3 - hard (%d x %d, %d mines)",
           EASY_WIDTH, EASY_HEIGHT, EASY_MINES, MIDDLE_WIDTH, MIDDLE_HEIGHT,
           MIDDLE_MINES, HARD_WIDTH, HARD_HEIGHT, HARD_MINES);
    move(0, 0);
    printw("Choose difficulty level: ");
    do {
        int c = getch();
        if (c == 'q') {
            return -1;
        }
        if (c >= '1' && c <= '3') {
            level = c - '0';
        }
    } while (!level);
    return level;
}

int rows(int level) {
    switch (level) {
    case 1:
        return EASY_HEIGHT;
    case 2:
        return MIDDLE_HEIGHT;
    case 3:
        return HARD_HEIGHT;
    default:
        return 0;
    }
}

int cols(int level) {
    switch (level) {
    case 1:
        return EASY_WIDTH;
    case 2:
        return MIDDLE_WIDTH;
    case 3:
        return HARD_WIDTH;
    default:
        return 0;
    }
}

int mines(int level) {
    switch (level) {
    case 1:
        return EASY_MINES;
    case 2:
        return MIDDLE_MINES;
    case 3:
        return HARD_MINES;
    default:
        return 0;
    }
}

int abs(int x) { return x < 0 ? -x : x; }

bool isInArea(int x1, int y1, int x2, int y2) {
    int dx = abs(x1 - x2), dy = abs(y1 - y2);
    return max(dx, dy) == 1;
}

void solve(Game& game, int w, int h, int) {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            if (!game.isBomb(i, j)) {
                game.open(i, j);
            }
        }
    }
}

class ShortcutTask {
  public:
    ShortcutTask(int x, int y, int creationEpoch, int* epoch, Game* game, std::queue<AsyncTask>* taskQueue)
        : x0(x), y0(y), creationEpoch(creationEpoch), epoch(epoch), game(game), taskQueue(taskQueue) {}

    void operator()() const {
        if (creationEpoch != *epoch) {
            return;
        }

        int width = game->width();
        int height = game->height();

        int x = x0;
        int y = y0;

        int cellsLeft = width * height;
        bool hasChanges = false;

        while (cellsLeft > 0 && !hasChanges) {
            hasChanges |= game->shortcut(x, y);
            --cellsLeft;

            ++x;
            if (x == width) {
                ++y;
                x = 0;
            }
            if (y == height) {
                y = 0;
            }
        }

        if (hasChanges) {
            taskQueue->push(ShortcutTask(x, y, creationEpoch, epoch, game, taskQueue));
        }
    }

  private:
    int x0;
    int y0;
    int creationEpoch;
    int* epoch;
    Game* game;
    std::queue<AsyncTask>* taskQueue;
};

int main() {
    initscr();
    keypad(stdscr, true);
    noecho();
    start_color();
    halfdelay(1);
    curs_set(2);

    int level = getGameLevel();

    if (level < 0) {
        endwin();
        return 0;
    }

    int width = cols(level);
    int height = rows(level);
    int flags = mines(level);

    Game game(height, width, flags);

    int epoch = 0;
    int cur_x = 0, cur_y = 0;
    bool q_pressed = false;
    bool godmode = false;
    bool highlight_area = false;

    clear();

    init_pair(NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(BOMB, COLOR_RED, COLOR_RED);
    init_pair(WIN, COLOR_YELLOW, COLOR_BLACK);
    init_pair(FLAG, COLOR_GREEN, COLOR_BLACK);
    init_pair(GODMODE_BOMB, COLOR_RED, COLOR_BLACK);
    init_pair(BORDER, COLOR_CYAN, COLOR_BLACK);
    init_pair(DIGIT, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(QUESTION_MARK, COLOR_YELLOW, COLOR_BLACK);

    bool first_loop = true;

    Trie* root = new Trie;
    int godmode_idx = insert(root, "godmode");
    int area_idx = insert(root, "area");
    int autowin_idx = insert(root, "autowin");
    int shortcut_idx = insert(root, "shortcut");
    int hint_idx = insert(root, "hint");
    Trie* FSM = root;

    std::queue<AsyncTask> async_tasks;

    while (!game.win() && !game.lose() && !q_pressed) {
        if (!async_tasks.empty()) {
            AsyncTask task = async_tasks.front();
            async_tasks.pop();
            task();
        }

        int ch = first_loop ? 0 : getch();
        first_loop = false;
        switch (ch) {
        case KEY_LEFT:
            cur_x--;
            break;
        case KEY_RIGHT:
            cur_x++;
            break;
        case KEY_UP:
            cur_y--;
            break;
        case KEY_DOWN:
            cur_y++;
            break;
        case ' ':
            ++epoch;
            game.open(cur_x, cur_y);
            break;
        case '!':
            ++epoch;
            game.shortcut(cur_x, cur_y);
            break;
        case 'F':
        case 'f':
            ++epoch;
            game.changeState(cur_x, cur_y);
            break;
        case 'Q':
        case 'q':
            q_pressed = true;
            break;
        default:
            if ('a' <= ch && ch <= 'z') {
                FSM = go(FSM, ch);
                vector<int> codes = terminals(FSM);
                for (int code : codes) {
                    if (code == godmode_idx) {
                        godmode = !godmode;
                    } else if (code == area_idx) {
                        highlight_area = !highlight_area;
                    } else if (code == autowin_idx) {
                        solve(game, width, height, flags);
                    } else if (code == shortcut_idx) {
                        async_tasks.push(
                            ShortcutTask(cur_x, cur_y, epoch, &epoch, &game, &async_tasks));
                    } else if (code == hint_idx) {
                        if (game.isBomb(cur_x, cur_y)) {
                            while (game.cell(cur_x, cur_y) != 'F') {
                                game.changeState(cur_x, cur_y);
                            }
                        } else {
                            game.open(cur_x, cur_y);
                        }
                    }
                }
            }
            break;
        }
        cur_x = max(0, min(cur_x, width - 1));
        cur_y = max(0, min(cur_y, height - 1));

        // clear();

        attron(COLOR_PAIR(BORDER));
        move(0, 0);
        addch('+');
        for (int x = 0; x < width; x++) {
            addch('-');
        }
        addch('+');

        move(height + 1, 0);
        addch('+');
        for (int x = 0; x < width; x++) {
            addch('-');
        }
        addch('+');
        attroff(COLOR_PAIR(BORDER));

        for (int y = 0; y < height; y++) {
            move(y + 1, 0);
            attron(COLOR_PAIR(BORDER));
            addch('|');
            attroff(COLOR_PAIR(BORDER));
            for (int x = 0; x < width; x++) {
                bool selected = (x == cur_x && y == cur_y);
                int color_type = NORMAL;
                char c = game.cell(x, y);
                if (c == 'F') {
                    color_type = FLAG;
                }
                if (c == 'B') {
                    color_type = BOMB;
                }
                if (c == '?') {
                    color_type = QUESTION_MARK;
                }
                if (game.win()) {
                    color_type = WIN;
                }
                if (c <= '9' && '0' <= c) {
                    color_type = DIGIT;
                }
                if (selected && godmode && game.isBomb(x, y)) {
                    color_type = GODMODE_BOMB;
                }
                bool inArea = isInArea(x, y, cur_x, cur_y);
                attron(COLOR_PAIR(color_type));
                if (inArea && highlight_area) {
                    attron(A_BOLD);
                }
                addch(c);
                if (inArea && highlight_area) {
                    attroff(A_BOLD);
                }
                attroff(COLOR_PAIR(color_type));
            }
            attron(COLOR_PAIR(BORDER));
            addch('|');
            attroff(COLOR_PAIR(BORDER));
        }

        move(height + 2, 0);
        printw("Flags left: %d/%d     ", game.flagsLeft(), flags);

        move(cur_y + 1, cur_x + 1);

        refresh();
    }

    if (game.win()) {
        move(height + 2, 0);
        printw("                        ");
        move(height + 2, 0);
        printw("YOU WIN!!!");
    }

    if (game.lose()) {
        move(height + 2, 0);
        printw("                        ");
        move(height + 2, 0);
        printw("YOU LOSE!!!");
    }

    if (!q_pressed) {
        while (getch() < 0) {
        }
    }
    endwin();
    return 0;
}
