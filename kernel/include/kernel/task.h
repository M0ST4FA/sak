#pragma once
#include "panic.h"
#include "uapi/types.h"

#define USR_STACK_SZ 1024
#define PROC_NR 10

// Indicies into top of userspace stack (task struct fields)
#define SPSR 0
#define SVC_WRAPPER_LR 1 // Address we return to into svc wrapper next time proc is scheduled
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

enum task_state {
	TS_FREE = 0x0,
	TS_RUNNABLE = 0x2,
	TS_WAIT_READ = 0x4,
	TS_WAIT_WRITE = 0x8,
};

struct task {
	enum task_state state;
	void (*entry_point)(void);
	unsigned int *sp;
};

extern size_t task_count;

extern unsigned int stacks[PROC_NR][USR_STACK_SZ];
extern struct task tasks[PROC_NR];
extern unsigned int current;

unsigned int *task_init(unsigned int *stack, void (*entry_point)(void));

static inline int task_entry_allocated(int i) {
	if (i >= PROC_NR)
		panic(PANIC_ASSERT, "task_entry_allocated(i): i > PROC_NR");

	return tasks[i].state != TS_FREE;
}

static inline int task_waiting(int i) {
	if (!task_entry_allocated(i)) // assertion
		panic(PANIC_ASSERT, "task_waiting(i): entry not allocated!");

	return tasks[i].state & (TS_WAIT_READ | TS_WAIT_WRITE);
}

int task_runnable(int i);
int task_next_pid(void);
void tasks_init(void);
