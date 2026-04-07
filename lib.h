#pragma once

#define ADD_MAX 10
#define BUF_SIZE 20
#define ERANGE 2

extern unsigned int __bss_start;
extern unsigned int __bss_end;

void print_char(char c);

void print_string(const char *str);

void print_int(int i);

int add(int a, int b);

void *memcpy(char *dst, char *src, int n);

void *memset(char *dst, char val, int n);

void reverse(char *buf, int size);

int inttostr(int i, char *buf);
