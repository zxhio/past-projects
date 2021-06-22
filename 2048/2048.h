// File:       2048.h
// Author:     xianhui (definezxh@163.com)
// Date:       2018/08/20 12:12:29

#ifndef TTICK_2048_H
#define TTICK_2048_H

#include <stdio.h>
#include <stdint.h>

#define BOARD_SIZE      4

typedef struct {
        int board[BOARD_SIZE][BOARD_SIZE];
        size_t blank_num;
        int moved;
        int score;
} state;

void board_init(state *s);
void board_draw(state *s);
void move_up(state *s);
void move_down(state *s);
void move_left(state *s);
void move_right(state *s);
int move(state *s, int d);

#endif // TTICK_2048_H
