#pragma once

#define USR_STACK_SZ 256
#define PROC_NR 2

extern unsigned int stacks[PROC_NR][USR_STACK_SZ];
extern void (*entry_points[PROC_NR])(void);
extern unsigned int *tasks[PROC_NR];

void setup_tasks(void);
