#include "task.h"
#include "usr.h"

int task_count = 3, last_pid = 2; // 3 because of initial task count
unsigned int current = 0;

unsigned int stacks[PROC_NR][USR_STACK_SZ];
struct task tasks[PROC_NR] = {
	{.entry_point = print_lover},
	{.entry_point = lesser_print_lover},
	{.entry_point = fork_lover},
};

unsigned int *task_init(unsigned int *stack, void (*entry_point)(void)) {
	stack += USR_STACK_SZ - 16;

	stack[0] = 0x10;
	stack[1] = (unsigned int)entry_point;

	return stack;
}

int task_entry_available(int i) {

	if (i >= PROC_NR)
		return 0;

	if (tasks[i].state == TS_FREE)
		return 0;

	return 1;
}

int task_next_pid(void) {
	if (task_count >= PROC_NR)
		return -1;

	do
		last_pid = (last_pid + 1) % PROC_NR;
	while (task_entry_available(last_pid));

	return last_pid;
}

void tasks_init(void) {
	int i;

	for (i = 0; i < task_count; i++) {
		tasks[i].sp = task_init(stacks[i], tasks[i].entry_point);
		tasks[i].state = TS_RUNNABLE;
	}

	for (i = task_count; i < PROC_NR; i++) {
		tasks[i].state = TS_FREE;
		tasks[i].entry_point = 0;
		tasks[i].sp = 0;
	}
}
