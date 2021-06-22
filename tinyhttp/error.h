//
// Created by zxh on 18-4-4.
//

#ifndef TINYHTTP_ERROR_H
#define TINYHTTP_ERROR_H


void err_ret(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_cont(int error, const char *fmt, ...);
void err_exit(int error, const char *fmt, ...);
void err_dump(const char *fmt, ...);
void err_msg(const char *fmt, ...);
void err_quit(const char *fmt, ...);

#endif //TINYHTTP_ERROR_H
