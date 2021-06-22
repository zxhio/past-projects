/*
 * Created by zxh on 2018/05/11 . All rights reserved.
 */

#include <tty.h>
#include <vga.h>
#include <string.h>

inline static uint32_t prev(uint32_t i)
{
	if (i == 0)
		return TTY_BUF_LEN  - 1;
	return i - 1;
}

inline static uint32_t next(int32_t i)
{
	if (i == TTY_BUF_LEN  - 1)
		return 0;
	return i + 1;
}

void tty_init()
{
	memset(tty_dev.buf, 0, TTY_BUF_LEN);
	tty_dev.head = 0;
	tty_dev.tail = 0;
	tty_dev.count = 0;
}

/*
 * if C == '\n', we should flush the tty buf,
 * but function TTY_PUTC only display,
 * function TTY_FLUSH is used for flush the tty buf.
 */
int tty_putc(int c)
{
	vga_putc(c);
	if (c == '\b') {			/* Backspace */
		vga_putc(' ');			/* fill with ' ' */
		vga_putc('\b');
		return tty_pop();
	} 
	/* 
	 * Carriage Enter
	 * else if (c == 0x0d) {
	 *	return tty_flush();
	 * }
	 */
	tty_push(c);
	return c;
}

int tty_getc()
{
	while (1) {
		if (tty_dev.buf[tty_dev.tail - 1] == '\n')
			return tty_dev.buf[0];
	}
	return 0;
}

int tty_push(int c)
{
	uint32_t tail = tty_dev.tail;
	if (tty_dev.count == TTY_BUF_LEN)
		return -1;
	else {
		tty_dev.buf[tail] = c;		/* implicit type conversion. */
		tty_dev.tail = next(tail);
		tty_dev.count++;
	}

	return 0;
}

int tty_pop()
{
	uint32_t tail = tty_dev.tail;
	char ch = tty_dev.buf[tail];

	if (tty_dev.count == 0)
		return -1;
	else {
		tty_dev.tail = prev(tail);
		tty_dev.count--;
	}
	
	return ch;
}


int tty_flush()
{
	int count = tty_dev.count;
	tty_init();
	return count;
}

char *tty_buf()
{
	tty_dev.buf[tty_dev.count] = '\0';
	return tty_dev.buf;
}
