/*
 * Created by zxh on 2018/05/11 . All rights reserved.
 */

#ifndef YEQU_INCLUDE_TTY_H
#define YEQU_INCLUDE_TTY_H

#include <types.h>

#define TTY_BUF_LEN	1024

struct tty {
	uint32_t head, tail;
	uint32_t count;
	char buf[TTY_BUF_LEN];
};

struct tty tty_dev;

void tty_init();
int tty_putc(int c);
int tty_getc();
int tty_push(int c);
int tty_pop();
int tty_flush();
char *tty_buf();

#endif /* YEQU_INCLUDE_TTY_H */
