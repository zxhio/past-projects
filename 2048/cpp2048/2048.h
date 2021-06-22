// File:       2048.h
// Author:     xianhui (definezxh@163.com)
// Date:       2018/09/26 17:33:59

#pragma once

#include <array>
#include <memory>
#include <random>

namespace ttick {

// move direction
#define MV_LEFT 'a'
#define MV_UP 'w'
#define MV_RIGHT 'd'
#define MV_DOWN 's'

// quit key
#define QUIT 'q'

// valid input character
#define DIR_KEY(d)                                                           \
    ((d) == MV_LEFT || (d) == MV_UP || (d) == MV_RIGHT || (d) == MV_DOWN ? 1 \
                                                                         : 0)
#define QUIT_KEY(d) ((d) == QUIT ? 1 : 0)
#define VALID_KEY(d) (DIR_KEY(d) || QUIT_KEY(d))

// \033 is octal, which is the terminal escape character ESC under linux(unix)
// ref: http://www.bitscn.com/os/linux/200904/158492.html
//      https://en.wikipedia.org/wiki/ANSI_escape_code
//      https://blog.csdn.net/haw_king/article/details/17390895
typedef enum {
    BLACK  = 0,  // #000000, black
    MAROON = 1,  // #800000, maroon
    GREEN  = 2,  // #008000, green
    OLIVE  = 3,  // #808000, olive
    NAVY   = 4,  // #000080, navy
    PURPLE = 5,  // #800080, purple
    TEAL   = 6,  // #008080, teal
    SILVER = 7   // #c0c0c0, silver
} colors;

constexpr const std::size_t boardSize = 4;

// Generate a random nunber with mt19937.
class RandomPos {
  public:
    RandomPos() : gen_(rd_()), dis_(0, boardSize * boardSize - 1) {}
    std::size_t position() {
        return dis_(gen_);
    }

  private:
    RandomPos(const RandomPos&) = delete;
    RandomPos& operator=(const RandomPos&) = delete;

    std::random_device              rd_;
    std::mt19937                    gen_;
    std::uniform_int_distribution<> dis_;
};

class _2048_ {
  public:
    // Create a empty _2048_ object, with score = 0 and
    // board generate randomly 2 or 4.
    explicit _2048_(RandomPos& randPos);

    _2048_(const _2048_& other);
    _2048_& operator=(const _2048_& other);

    void draw() const;

    // Addition in left direction, for late moveDirection
    void moveLeft();
    void moveUp();
    void moveRight();
    void moveDown();

    // Judge game can go on
    bool movable() const;

    // Generate next chunk, return false on blankNum_ == 0, return true true on
    // success and blankNum_ - 1.
    bool generateNextChunk(RandomPos& randPos);

    std::size_t blankNum() const {
        return blankNum_;
    }
    std::size_t score() const {
        return score_;
    }

    int getChunk(std::size_t x, std::size_t y) const {
        return board_[x][y];
    }

    bool isMoved() const {
        return moved_;
    }

    // Transpose board.
    void transposeBoard();
    // Symmetric exchange column.
    void exchangeColumn();

  private:
    // std::unique_ptr<RandomPos> randomPos_;
    std::array<std::array<int, boardSize>, boardSize> board_;
    std::size_t blankNum_;
    std::size_t score_;
    bool        moved_; // check board has changed
};

bool move(_2048_& game, char d, RandomPos& randPos);
void play(_2048_& game, RandomPos& randPos);

// A chunk include 2 case:
//  (0) |       |  (2) |       |    1th line
//      |   -   |      |   2   |    2th line
//      |       |,     |       |    3th line
// All lines print same color according to chunk value.
void drawChunk(int rowNo, int foreColor, int backColor, int score);

// Draw A 2048 row (3 terminal lines)
void drawRow(int rowNo, const std::array<int, boardSize>& row);

// Break cause interrupt signal, wait user input character and not print on
// screen.
char directionInput();

}  // namespace ttick
