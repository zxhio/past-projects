/*
 * Created by zxh on 2017/06/29 . All rights reserved.
 */

#include <vga.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <kbd.h>
#include <tty.h>
#include <string.h>
#include <mm.h>
#include <debug.h>
#include <istr.h>

void user_input(const char *input);


void init()
{
	//int a = 10;
	vga_init();
	printk("YEQU\n");
	gdt_init();
	idt_init();
	asm volatile ("sti");
	timer_init(200);
	//int b = 10;
	//printk(" %d, %x,%x\n", &a, &b, init);
	kbd_init();
	// mm_init();
	int c = tty_getc();
	printk("chin: %c\n", c);

	while (1);
}


void user_input(const char *input)
{
	if (strcmp(input, "shutdown") == 0) {
		printk("shutdown YEQU, bye!\n");
		asm volatile("hlt");
	} else if (strcmp(input, "help") == 0) {
		printk("  shutdown\n");
		printk("  help\n");
	}
}
