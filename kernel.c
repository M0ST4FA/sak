#include "asm.h"
#include "lib.h"

#define USR_STACK_SZ 256
#define PROC_NR 2

unsigned int stacks[PROC_NR][USR_STACK_SZ];
unsigned int *tasks[PROC_NR];

void first(void) {
	print_string("In user mode: beginning\n");
	syscall();
	print_string("In user mode: second time\n");
	syscall();
	print_string("In user mode: end\n");
	syscall();

	// make sure if the task doesn't have anything to do, it keeps trapping into the kernel
	while (1)
		syscall(); // syscall in our case stores task state and restores kernel. It is like switching back to proc 0
}

void task(void) {
	print_string("In other task\n");
	while (1)
		syscall();
}

void setup_tasks(void) {
	tasks[0] = stacks[0] + USR_STACK_SZ - 16;
	tasks[0][0] = 0x10;
	tasks[0][1] = (unsigned int)&first;

	tasks[1] = stacks[1] + USR_STACK_SZ - 16;
	tasks[1][0] = 0x10;
	tasks[1][1] = (unsigned int)&task;
}

int start_kernel() {
	unsigned int *saved_stack;

	setup_tasks();

	print_string("Kernel started...\n");

	saved_stack = activate(tasks[0]);
	tasks[0] = saved_stack;

	print_string("Back to kernel...\n");

	saved_stack = activate(tasks[1]);
	tasks[1] = saved_stack;

	print_string("Back to kernel...\n");

	saved_stack = activate(tasks[0]);
	tasks[0] = saved_stack;

	print_string("Back to kernel...\n");

	while (1)
		;

	print_string("After loop...should never reach here\n");

	return 0;
}
