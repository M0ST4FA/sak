#include "task.h"
#include "usr.h"

unsigned int stacks[PROC_NR][USR_STACK_SZ];
void (*entry_points[PROC_NR])(void) = {print_lover, lesser_print_lover};
unsigned int *tasks[PROC_NR];

unsigned int *init_task(unsigned int *stack, void (*entry_point)(void)) {
	stack += USR_STACK_SZ - 16;

	stack[0] = 0x10;
	stack[1] = (unsigned int)entry_point;

	return stack;
}

void setup_tasks(void) {
	for (int i = 0; i < PROC_NR; i++)
		tasks[i] = init_task(stacks[i], entry_points[i]);
}
