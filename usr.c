#include "usr.h"
#include "asm.h"
#include "lib.h"

void print_lover(void) {
	print_string("In user mode: beginning\n");
	kernel_yield();
	print_string("In user mode: second time\n");
	kernel_yield();
	print_string("In user mode: end\n");
	kernel_yield();

	// make sure if the task doesn't have anything to do, it keeps trapping into the kernel
	while (1)
		kernel_yield();
}

void lesser_print_lover(void) {
	print_string("In other task\n");
	while (1)
		kernel_yield();
}

void fork_lover(void) {
	int child_pid;
	char buf[BUF_SIZE];

	print_string("I'm about to have a child...\n");

	child_pid = fork();
	if (child_pid == 0) {
		print_string("Inside child. Never though I'm gonna make it, but here I'm!\n");

		while (1)
			kernel_yield();
	}

	inttostr(child_pid, buf);
	print_string("Having a child is easier than what I expected...PID: ");
	print_string(buf);
	print_char('\n');

	while (1)
		kernel_yield();
}
