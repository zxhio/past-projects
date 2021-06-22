/*
 * Created by zxh on 2018/04/02 . All rights reserved.
 */

#include <kbd.h>
#include <tty.h>
#include <istr.h>
#include <idt.h>
#include <debug.h>
#include <string.h>
#include <types.h>


/*
 * the keyboard ordinary scode reference address.
 * http://www.win.tue.nl/~aeb/linux/kbd/scodes-1.html#ss1.2
 */

#define SHIFT_L		0x2a
#define SHIFT_R 	0x36
#define BACKSPACE	0x0E
#define ENTER		0x1C
#define CAPSLOCK	0x3A
#define TAB		0x0F


static uint8_t kbd_scodes[128] = {
	0,		/* Normal error code */
	0x1b,		/* Esc */
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	0x08, 		/* Backspce, 0x8 is ascii code, the same as '\b' */
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	'\n',		/* Enter */
	0,		/* Left Ctrl */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '"',
	'`', 		/* ', left of '1' position */
	0,		/* Lshift */
	'\\',		/* On a 102-key keyboard */
	'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	0,		/* Rshift */
};

static uint8_t kbd_scodes_upper[128] = {
	0,		/* Normal error code */
	0x1b,		/* Esc */
	'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	0x08, 		/* Backspce, 0x8 is ascii code. */
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
	'\n',		/* Enter */
	0,		/* Left Ctrl */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'',
	'~', 		/* ', left of '1' position */
	0,		/* Lshift */
	'|',		/* On a 102-key keyboard */
	'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
	0,		/* Rshift */
};


static int shift_flag = 0;

static void kbd_handler(struct pt_regs *reg)
{
	uint8_t ch;
	uint8_t code = inb(0x60);		/* read the data buffer */

	if(code == SHIFT_L || code == SHIFT_L)
		shift_flag = 1;
	else if(code == 0xaa || code == 0xb6 )
		shift_flag = 0;

	if (code & 0x80) {

	} else {
		if(code == SHIFT_L || code == SHIFT_R)
			return ;
		ch = shift_flag ? kbd_scodes_upper[code] : kbd_scodes[code];
		tty_putc(ch);
	}
}


void kbd_init()
{
	register_interrupt_handler(IRQ1, kbd_handler);
	tty_init();
}
