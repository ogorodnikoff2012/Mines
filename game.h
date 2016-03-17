#ifndef __XENON_MINES_GAME_H__
#define __XENON_MINES_GAME_H__

#include <vector>

enum GameState
{
    CLOSED, MARKED, MAYBE, OPENED
};

class Game
{
    private:
        std::vector<std::vector<bool>> mines;
        std::vector<std::vector<GameState>> state;
        int neighbors(int x, int y) const;
        bool lost;
        int flags, opened, numOfMines;
        static GameState nextState(GameState st);
    public:
        Game(int rows, int cols, int mines);
//        Game(const char *filename);
//        void save(const char *filename) const;
        char cell(int x, int y) const;
        void open(int x, int y);
        void changeState(int x, int y);
        int flagsLeft() const;
        bool win() const;
        bool lose() const;
        bool isBomb(int x, int y) const;
};

#endif//__XENON_MINES_GAME_H__ 
