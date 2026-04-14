#include "task.h"
#include "lib.h"
#include "panic.h"

size_t task_count = 0, runnable_count = 0, last_pid = 0; // 3 because of initial task count
unsigned int current = 0;

unsigned int stacks[PROC_NR][USR_STACK_SZ];
struct task tasks[PROC_NR];
unsigned int *task_init(unsigned int *stack, void (*entry_point)(void)) {
	stack += USR_STACK_SZ - 16;

	stack[0] = 0x10;
	stack[1] = (unsigned long)entry_point;

	return stack;
}

int task_runnable(int i) {
	return task_entry_allocated(i) && tasks[i].state == TS_RUNNABLE;
}

int task_next_pid(void) {
	if (task_count >= PROC_NR)
		return -1;

	do
		last_pid = (last_pid + 1) % PROC_NR;
	while (task_entry_allocated(last_pid));

	return last_pid;
}

extern void (*__init_tasks_start[])(void);
extern void (*__init_tasks_end[])(void);

void tasks_init(void) {
	size_t i;
	task_count = 0;
	void (**task)(void) = __init_tasks_start;

	for (task = __init_tasks_start; task < __init_tasks_end; task++) {
		tasks[task_count].entry_point = *task;
		tasks[task_count].state = TS_RUNNABLE;
		runnable_count++;
		tasks[task_count].sp = task_init(stacks[task_count], *task);

		task_count++;
	}
	last_pid = task_count - 1;

	for (i = task_count; i < PROC_NR; i++) {
		tasks[i].state = TS_FREE;
		tasks[i].entry_point = 0;
		tasks[i].sp = 0;
	}
}
