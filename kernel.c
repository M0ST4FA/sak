#include "asm.h"
#include "lib.h"
#include "task.h"

int start_kernel() {
	unsigned int *saved_stack, current = 0;
	char buf[BUF_SIZE] = {0};

	setup_tasks();

	print_string("Kernel started...\n");

	// scheduling loop
	while (1) {

		print_string("Scheduling task: ");
		if (!inttostr(current, buf))
			print_string(buf);

		print_char('\n');

		saved_stack = activate(tasks[current]);
		tasks[current] = saved_stack;

		print_string("Back to kernel...\n");

		// add some delay
		for (volatile int i = 0; i < 1000000000; i++)
			;

		// current++;
		// if (current >= PROC_NR)
		// 	current = 0;

		current = (current + 1) % PROC_NR;
	};

	print_string("After loop...should never reach here\n");

	return 0;
}
