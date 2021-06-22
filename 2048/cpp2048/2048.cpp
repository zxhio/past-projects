// File:       2048.cpp
// Author:     xianhui (definezxh@163.com)
// Date:       2018/09/26 17:50:14

#include <iomanip>
#include <iostream>
#include <random>

#include "2048.h"

// #include <cstdio>

namespace ttick {

#define RANDOM_VALUE(pos) ((pos) % 2 ? 2 : 4)

static int foreColors[7] = {30 + TEAL,  30 + PURPLE, 30 + NAVY,  30 + OLIVE,
                            30 + GREEN, 30 + MAROON, 30 + SILVER};

static int backColors[7] = {40 + MAROON, 40 + SILVER, 40 + OLIVE, 40 + GREEN,
                            40 + PURPLE, 40 + NAVY,   40 + TEAL};

void drawChunk(int rowNo, int foreColor, int backColor, int score) {
    std::cout << "\033[" << foreColor << ";" << backColor << "m"
              << std::setw(5);
    if (rowNo == 1 || rowNo == 3)
        std::cout << ' ';
    else if (score == 0)
        std::cout << '-';
    else
        std::cout << score;
    std::cout << "    \033[0m";

    /* compare iostream with stdio, it looks a little different.
    if (rowNo == 1 || rowNo == 3)
        printf("\033[%d;%dm%5c    \033[0m", foreColor, backColor, ' ');
    else if (score == 0)
        printf("\033[%d;%dm%5c    \033[0m", foreColor, backColor, '-');
    else
        printf("\033[%d;%dm%5d    \033[0m", foreColor, backColor, score);
    */
}

void drawRow(int rowNo, const std::array<int, boardSize>& row) {
    int color;
    for (auto& chunk : row) {
        color = static_cast<int>(log(chunk) / log(2)) % 7;
        // all chunk foreground color use BLACK.
        drawChunk(rowNo, /* foreColors[color] */ 30, backColors[color], chunk);
    }
    std::cout << '\n';
}

char directionInput() {
    char d;
    system("stty raw -echo");
    d = getchar();
    system("stty cooked echo");
    return d;
}

_2048_::_2048_(RandomPos& randPos)
    :  // randomPos_(std::make_unique<RandomPos>()),
      blankNum_(boardSize * boardSize),
      score_(0),
      moved_(false) {
    for (auto& row : board_)
        row.fill(0);
    generateNextChunk(randPos);
    generateNextChunk(randPos);
}

_2048_::_2048_(const _2048_& other) {
    score_    = other.score_;
    moved_    = other.moved_;
    blankNum_ = other.blankNum_;
    for (std::size_t i = 0; i < boardSize; ++i)
        for (std::size_t j = 0; j < boardSize; ++j)
            board_[i][j]   = other.getChunk(i, j);
}

_2048_& _2048_::operator=(const _2048_& other) {
    score_    = other.score_;
    moved_    = other.moved_;
    blankNum_ = other.blankNum_;
    for (std::size_t i = 0; i < boardSize; ++i)
        for (std::size_t j = 0; j < boardSize; ++j)
            board_[i][j]   = other.getChunk(i, j);
    return *this;
}

void _2048_::draw() const {
    // std::cout << "\033[2J\033[0;0H";
    std::cout << "[score " << std::setw(6) << score_ << "]" << std::setw(12)
              << " "
              << "[blank " << std::setw(2) << blankNum_ << "]\n";

    for (auto& row : board_)
        for (int i = 1; i <= 3; ++i)
            drawRow(i, row);  // print in screen so slow with 3 terminal lines.

    std::cout << std::setw(8) << ' ' << MV_LEFT << ' ' << MV_UP << ' '
              << MV_RIGHT << ' ' << MV_DOWN << "  and " << QUIT << "(quit)\n";
}

void _2048_::transposeBoard() {
    for (int i = 0; i < boardSize; ++i)
        for (int j = i + 1; j < boardSize; ++j)
            std::swap(board_[i][j], board_[j][i]);
}

void _2048_::exchangeColumn() {
    for (int i = 0; i < boardSize; ++i)
        for (int j = 0; j < boardSize / 2; ++j)
            std::swap(board_[i][j], board_[i][boardSize - j - 1]);
}

void _2048_::moveLeft() {
    moved_ = false;
    for (auto& row : board_) {
        for (int i = 0, j = 1; j < boardSize; ++j) {
            if (row[j] == 0) {  // skip 0, find next non-0 chunk
                continue;
            }
            if (row[i] == 0) {
                row[i] = row[j];
                row[j] = 0;
                moved_ = true;
            } else if (row[j] == row[i]) {  // combine
                row[i] += row[j];
                row[j] = 0;
                score_ += row[i];
                blankNum_++;
                moved_ = true;
                ++i;
            } else if (row[++i] == 0) {
                row[i] = row[j];
                row[j] = 0;
                moved_ = true;
            }
        }
    }
}

void _2048_::moveUp() {
    transposeBoard();
    moveLeft();
    transposeBoard();
}

void _2048_::moveRight() {
    exchangeColumn();
    moveLeft();
    exchangeColumn();
}

void _2048_::moveDown() {
    transposeBoard();
    moveRight();
    transposeBoard();
}

bool _2048_::generateNextChunk(RandomPos& randPos) {
    std::size_t pos;

    if (blankNum_ == 0)
        return false;
    do {
        pos = randPos.position();
    } while (board_[pos / boardSize][pos % boardSize] != 0);
    blankNum_--;
    board_[pos / boardSize][pos % boardSize] = RANDOM_VALUE(pos);

    return true;
}

bool _2048_::movable() const {
    // if have blank chunk
    for (int i = 0; i < boardSize; ++i)
        for (int j = 0; j < boardSize; ++j)
            if (getChunk(i, j) == 0)
                return true;

    // inside
    for (int i = 1; i < boardSize - 1; ++i) {
        for (int j = 1; j < boardSize - 1; ++j) {
            if (getChunk(i, j) == getChunk(i - 1, j))
                return true;
            if (getChunk(i, j) == getChunk(i + 1, j))
                return true;
            if (getChunk(i, j) == getChunk(i, j - 1))
                return true;
            if (getChunk(i, j) == getChunk(i, j + 1))
                return true;
        }
    }

    // around horizontal
    for (int i = 0; i < boardSize; i += boardSize - 1)
        for (int j = 1; j < boardSize; ++j)
            if (getChunk(i, j) == getChunk(i, j - 1))
                return true;
    // around vertical
    for (int j = 0; j < boardSize; j += boardSize - 1)
        for (int i = 1; i < boardSize; ++i)
            if (getChunk(i, j) == getChunk(i - 1, j))
                return true;

    return false;
}

bool move(_2048_& game, char d, RandomPos& randPos) {
    std::size_t pos;
    switch (d) {
        case MV_LEFT:
            game.moveLeft();
            break;
        case MV_UP:
            game.moveUp();
            break;
        case MV_RIGHT:
            game.moveRight();
            break;
        case MV_DOWN:
            game.moveDown();
            break;
    }

    if (game.isMoved()) {
        game.generateNextChunk(randPos);
        // game.draw();
    }

    if (!game.movable())  // must after generateNextChunk()
        return false;

    return true;
}

void play(_2048_& game, RandomPos& randPos) {
    char d;
    do {
        do {
            d = directionInput();
        } while (!VALID_KEY(d));
        if (DIR_KEY(d)) {
            if (move(game, d, randPos)) {
                game.draw();  // here call draw() is optional for printing in ai
            } else {
                std::cout << "GAME OVER. YOUR SCOREs [" << game.score()
                          << "]\n";
                return;
            }
        }
    } while (!QUIT_KEY(d));
}

}  // namespace ttick
