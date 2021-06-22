/*
 * Created by zxh on 2017/08/13 . All rights reserved.
 */

#ifndef YEQU_INCLUDE_DEBUG_H
#define YEQU_INCLUDE_DEBUG_H

#include <args.h>
#include <vga.h>

void panic(const char *str);

/* kernel print function */
void printk(const char *format, ...);

void printk_color(vga_color_t back, vga_color_t fore, const char *format, ...);

#endif	/* YEQU_INCLUDE_DEBUG_H */
