#pragma once
#include "kernel/uapi/types.h"

#define ADD_MAX 10
#define BUF_SIZE 20
#define ERANGE 2

extern unsigned int __bss_start;
extern unsigned int __bss_end;

void print_char(char c);

void print_string(const char *str);

void print_int(int i);

int add(int a, int b);

void *memcpy(char *dst, char *src, size_t n);

void *memset(char *dst, char val, size_t n);

void reverse(char *buf, int size);

int inttostr(int i, char *buf);

int strcmp(const char *a, const char *b);

size_t strlen(const char *s);
