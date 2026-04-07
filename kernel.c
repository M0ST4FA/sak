#include "asm.h"
#include "lib.h"
#include "panic.h"
#include "syscall.h"
#include "task.h"

int panic_cause;

void start_kernel() {
	unsigned int *saved_stack, current = 0;
	char buf[BUF_SIZE] = {0};

	setup_tasks();

	print_string("Kernel started...\n");

	// main kernel loop
	while (1) {

		print_string("Scheduling task: ");
		if (!inttostr(current, buf))
			print_string(buf);

		print_char('\n');

		saved_stack = activate(tasks[current]);

		// ======================= SVC_ENTRY returns here  ===========================

		tasks[current] = saved_stack;

		print_string("Back to kernel...\n");

		switch (tasks[current][R7]) {
		case SYSCALL_YIELD:
			break;

		default:
			goto panic;
		}

		// add some delay
		for (volatile int i = 0; i < 100000000; i++)
			;

		// current++;
		// if (current >= PROC_NR)
		// 	current = 0;

		current = (current + 1) % PROC_NR;
	};
	print_string("After loop...should never reach here\n");

panic:

	switch (panic_cause) {
	case PANIC_SYSCALL:
		print_string("panic: Invalid syscall!\n");
		break;

	default:
		print_string("panic: Unknown reason!\n");
	}

	asm volatile(
		"wfi");
	print_string("After wfi...should never reach here\n");
}
