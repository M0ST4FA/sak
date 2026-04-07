#pragma once

#define USR_STACK_SZ 256
#define PROC_NR 4

// Indicies into top of userspace stack (task struct fields)
#define SPSR 0
#define SVC_WRAPPER_LR 1
#define R0 2
#define R1 3
#define R2 4
#define R3 5
#define R4 6
#define R5 7
#define R6 8
#define R7 9
#define R8 10
#define R9 11
#define R10 12
#define FP 13
#define IP 14
#define SP 15
#define PROC_LR 16

extern unsigned int stacks[PROC_NR][USR_STACK_SZ];
extern void (*entry_points[PROC_NR])(void);
extern unsigned int *tasks[PROC_NR];

void setup_tasks(void);
