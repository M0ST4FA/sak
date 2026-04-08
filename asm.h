#pragma once
#include "types.h"

extern unsigned int *activate(unsigned int *);
extern void kernel_yield(void);
extern int fork(void);
extern int getpid(void);
extern ssize_t write(int fd, void *buf, size_t count);
extern ssize_t read(int fd, void *buf, size_t count);
