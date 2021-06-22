// File:       2048.c
// Author:     xianhui (definezxh@163.com)
// Date:       2018/08/20 12:17:26

#include "2048.h"
#include <stdlib.h>
#include <time.h>       // clock()
#include <math.h>       // log()

#define MV_LEFT         'h'
#define MV_UP           'k'
#define MV_RIGHT        'l'
#define MV_DOWN         'j'

// quit key
#define QUIT            'q'

// valid input character
#define DIR_KEY(d) ((d) == MV_LEFT || (d) == MV_UP || (d) == MV_RIGHT || (d) == MV_DOWN ? 1 : 0)
#define QUIT_KEY(d) ((d) == QUIT ? 1 : 0)
#define VALID_KEY(d) (DIR_KEY(d) || QUIT_KEY(d))

// \033 is octal, which is the terminal escape character ESC under linux(unix)
// ref: http://www.bitscn.com/os/linux/200904/158492.html
//      https://en.wikipedia.org/wiki/ANSI_escape_code
//      https://blog.csdn.net/haw_king/article/details/17390895
typedef enum {
        BLACK   = 0,            // #000000, black
        MAROON  = 1,            // #800000, maroon
        GREEN   = 2,            // #008000, green
        OLIVE   = 3,            // #808000, olive
        NAVY    = 4,            // #000080, navy
        PURPLE  = 5,            // #800080, purple
        TEAL    = 6,            // #008080, teal
        SILVER  = 7,            // #c0c0c0, silver
} colors;

static int fore_colors[7] = {
        30 + TEAL,
        30 + PURPLE,
        30 + NAVY,
        30 + OLIVE,
        30 + GREEN,
        30 + MAROON,
        30 + SILVER,
};

static int back_colors[7] = {
        40 + SILVER,
        40 + TEAL,
        40 + MAROON,
        40 + PURPLE,
        40 + NAVY,
        40 + OLIVE,
        40 + GREEN
};

// print block and return color
static void print_a_block(int score)
{
        int color;
        if (score == 0) {
                printf("  · ");
        } else {
                color = (int)(log(score) / log(2));     // low score corresponding light color
                printf("\033[%d;%dm%-5d\033[0m", fore_colors[color], back_colors[color], score);
        }
}

static void swap(int *a, int *b)
{
        int tmp = *a;
        *a = *b;
        *b = tmp;
}

static int get_random()
{
        int seed = clock();
        return seed % (BOARD_SIZE * BOARD_SIZE);
}

static int get_random_value()
{
        return get_random() % 2 ? 2 : 4;
}

// if have blank, return a random pos.
// else return -1.
static int get_next_value_pos(state *s)
{
        int pos;

        if (s->blank_num == 0)
                return -1;
        for (pos = get_random();
                        s->board[pos / BOARD_SIZE][pos % BOARD_SIZE]  != 0;
                        pos = get_random());
        s->blank_num--;
        return pos;
}


void board_init(state *s)
{
        int pos1, pos2;

        for (int i = 0; i < BOARD_SIZE; i++)
                for (int j = 0; j < BOARD_SIZE; j++)
                        s->board[i][j] = 0;
        s->blank_num = BOARD_SIZE * BOARD_SIZE;
        s->moved = 0;
        s->score = 0;

        // randomize the first two '2' position simply.
        pos1 = get_next_value_pos(s);
        s->board[pos1 / BOARD_SIZE][pos1 % BOARD_SIZE] = get_random_value();
        pos2 = get_next_value_pos(s);
        s->board[pos2 / BOARD_SIZE][pos2 % BOARD_SIZE] = get_random_value();
}

void board_draw(state *s)
{
        char c;
        char color[40], reset_color[] = "\033[m";

        printf("\033[2J\033[0;0H");     // clear screen, set cursor position (0, 0)
        printf("SCOREs %d  BLANK %lu\n", s->score, s->blank_num);
        for(int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                        print_a_block(s->board[i][j]);
                        if (j < BOARD_SIZE - 1)
                                printf("");
                        else
                                printf("\n");
                }
        }
        printf(" h k j l and q\n");
}

static void board_transpose(state *s)
{
        for (int i = 0; i < BOARD_SIZE; i++)
                for (int j = i + 1; j < BOARD_SIZE; j++)
                        swap(&s->board[i][j], &s->board[j][i]);
}

// board column symmetry transformation
static void board_reverse(state *s)
{
        for (int i = 0; i < BOARD_SIZE; i++)
                for (int j = 0; j < BOARD_SIZE / 2; j++)
                        swap(&s->board[i][j], &s->board[i][BOARD_SIZE - 1 - j]);

}

// the impletation is bullshit with duplicated code in 2 judgement
// \TODO refactoring code to enhance the universal property.
void move_left(state *s)
{
        int i, j, k;

        s->moved = 0;
        for (i = 0, j = 1, k = 0; k < BOARD_SIZE; i = 0, j = 1, k++) {
                for (j = 1; j < BOARD_SIZE; j++) {
                        while (j < BOARD_SIZE - 1 && s->board[k][j] == 0)
                                j++;
                        if (s->board[k][i] == 0) {      // move
                                if (s->board[k][i] != s->board[k][j])
                                        s->moved = 1;
                                s->board[k][i] = s->board[k][j];
                                s->board[k][j] = 0;
                        } else if (s->board[k][i] == s->board[k][j]) {  // move and combine
                                s->board[k][i] += s->board[k][j];
                                s->board[k][j] = 0;
                                s->blank_num++;
                                s->score += s->board[k][i];
                                s->moved = 1;
                                i++;
                        } else {                                // the bad code
                                if (s->board[k][++i] == 0) {    // the next fields could move only
                                        if (s->board[k][i] != s->board[k][j])
                                                s->moved = 1;
                                        s->board[k][i] = s->board[k][j];
                                        s->board[k][j] = 0;
                                }
                        }
                }
        }
}

void move_up(state *s)
{
        board_transpose(s);
        move_left(s);
        board_transpose(s);
}

void move_right(state *s)
{
        board_reverse(s);
        move_left(s);
        board_reverse(s);
}

void move_down(state *s)
{
        board_transpose(s);
        move_right(s);
        board_transpose(s);
}

static char direction_input()
{
        char d;
        system("stty raw -echo");
        d = getchar();
        system("stty cooked echo");
        return d;
}

static int moveable(state *s)
{
        // if have blank block
        for (int i = 0; i < BOARD_SIZE; i++)
                for (int j = 0; j < BOARD_SIZE; j++)
                        if (s->board[i][j] == 0)
                                return 1;
        // inside
        for (int i = 1; i < BOARD_SIZE - 1; i++) {
                for (int j = 1; j < BOARD_SIZE - 1; j++) {
                        if (s->board[i][j] == s->board[i - 1][j])
                                return 1;
                        if (s->board[i][j] == s->board[i + 1][j])
                                return 1;
                        if (s->board[i][j] == s->board[i][j - 1])
                                return 1;
                        if (s->board[i][j] == s->board[i][j + 1])
                                return 1;
                }
        }

        // around horizontal
        for (int i = 0; i < BOARD_SIZE; i += BOARD_SIZE - 1)
                for (int j = 1; j < BOARD_SIZE; j++)
                        if (s->board[i][j] == s->board[i][j - 1])
                                return 1;
        // around vertical
        for (int j = 0; j < BOARD_SIZE; j += BOARD_SIZE - 1)
                for (int i = 1; i < BOARD_SIZE; i++)
                        if (s->board[i][j] == s->board[i - 1][j])
                                return 1;

        return 0;
}

int move(state *s, int direction)
{
        int pos;
        switch (direction) {
        case MV_LEFT:
                move_left(s);
                break;
        case MV_UP:
                move_up(s);
                break;
        case MV_RIGHT:
                move_right(s);
                break;
        case MV_DOWN:
                move_down(s);
                break;
        }
        if (!moveable(s))
                return 0;
        if (s->moved) {
                pos = get_next_value_pos(s);
                s->board[pos / BOARD_SIZE][pos % BOARD_SIZE] = get_random_value();
                board_draw(s);
        }
        return 1;
}

void play(state *s)
{
        char d;
        do {
                do {
                        d = direction_input();  // input the operate direction
                } while (!VALID_KEY(d));        // must be special character
                if (DIR_KEY(d))
                        if (!move(s, d)) {
                                printf("GAME OVER. YOUR SCORE is [%d]\n", s->score);
                                return ;
                        }

        } while (!QUIT_KEY(d));
}

void test(state *s)
{
        char a[4] = {'h', 'k', 'j', 'l'};
        int i;
        do {
                i = get_random() % 4;
        } while (move(s, a[i % 4]));
}

int main()
{
        state s;
        board_init(&s);
        board_draw(&s);
//        play(&s);
         test(&s);
        return 0;
}
