#pragma once
#include "uapi/types.h"

#define ADD_MAX 10
#define BUF_SIZE 20
#define ERANGE 2

extern unsigned int __bss_start;
extern unsigned int __bss_end;

void kprint_char(char c);

void kprint_string(const char *str);

void kprint_int(int i);

void *kmemcpy(char *dst, char *src, int n);

void *kmemset(char *dst, char val, int n);

void kreverse(char *buf, int size);

int kinttostr(int i, char *buf);

int kstrcmp(const char *a, const char *b);

size_t kstrlen(const char *s);
