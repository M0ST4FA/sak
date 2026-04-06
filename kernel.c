#include "asm.h"
#include "lib.h"
#include "task.h"

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
