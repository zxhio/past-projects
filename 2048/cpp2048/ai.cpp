// File:       ai.cpp
// Author:     xianhui (definezxh@163.com)
// Date:       2018/09/28 15:35:20

#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include "2048.h"

ttick::_2048_& max(ttick::_2048_& lhs, ttick::_2048_& rhs) {
    return lhs.score() >= rhs.score() ? lhs : rhs;
}

ttick::_2048_& nextStep(ttick::_2048_& game, char nextDirect,
                        ttick::RandomPos& randPos) {
    ttick::move(game, nextDirect, randPos);
    return game;
}

ttick::_2048_& nextMove(ttick::_2048_& game, char nextDirect,
                        ttick::RandomPos& randPos, int depth) {
    if (depth == 0)
        return game;
    nextStep(game, nextDirect, randPos);

    ttick::_2048_ left  = game;
    ttick::_2048_ right = game;
    ttick::_2048_ up    = game;
    ttick::_2048_ down  = game;

    nextMove(left, MV_LEFT, randPos, depth - 1);
    nextMove(up, MV_UP, randPos, depth - 1);
    nextMove(right, MV_RIGHT, randPos, depth - 1);
    nextMove(down, MV_DOWN, randPos, depth - 1);

    return max(max(left, up), max(right, down));
}

int bestMove(ttick::_2048_& game, ttick::RandomPos& randPos, int depth) {
    ttick::_2048_ nextGame[4] = {game, game, game, game};

    nextGame[0] = nextMove(nextGame[0], MV_LEFT, randPos, depth -1);
    nextGame[1] = nextMove(nextGame[1], MV_UP, randPos, depth-1);
    nextGame[2] = nextMove(nextGame[2], MV_RIGHT, randPos, depth-1);
    nextGame[3] = nextMove(nextGame[3], MV_DOWN, randPos, depth-1);

    int max = 0;
    for (int i = 1; i < 4; ++i) {
        if (nextGame[max].score() < nextGame[i].score())
            max = i;
    }

    if (nextGame[0].score() == nextGame[1].score() && nextGame[2].score() &&
        nextGame[3].score() && nextGame[0].score() == nextGame[3].score())
        for (int i = 0; i < 4; ++i)
            if (nextGame[i].isMoved())
                max = i;

    return max;
}

void ai(ttick::_2048_& game, ttick::RandomPos& randPos) {
    int  d    = 0;
    char a[4] = {MV_LEFT, MV_UP, MV_RIGHT, MV_DOWN};
    do {
        d = bestMove(game, randPos, 2);
        move(game, a[d], randPos);
        std::cout << "direct: " << d << '\n';
        game.draw();  // here call draw() is optional for printing in ai
        sleep(1);
    } while (game.movable());
    std::cout << "GAME OVER. YOUR SCOREs [" << game.score() << "]\n";
    return;
}

int main(int argc, char* argv[]) {
    unsigned int usec = 0;
    if (argc < 2 || (*argv[1] != '0' && *argv[1] != '1')) {
        std::cout << "Usage: play(0), ai(1)\n";
        return -1;
    }

    usec = argc > 2 ? atoi(argv[2]) : 0;

    ttick::RandomPos randPos;
    ttick::_2048_    game(randPos);
    if (*argv[1] == '0') {
        game.draw();
        ttick::play(game, randPos);
    } else {
        ai(game, randPos);
    }
}
