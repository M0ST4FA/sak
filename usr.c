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

void fork_child(void) {
	print_string("I'm a very cute child of fork\n");

	while (1)
		kernel_yield();
}

void fork_lover(void) {
	int child_pid = fork();

	print_string("I'm about to have a child...\n");

	if (!child_pid)
		fork_child();

	print_string("Having a child is easier than what I expected...\n");

	while (1)
		kernel_yield();
}
