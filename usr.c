#include "usr.h"
#include "asm.h"
#include "lib.h"

void print_lover(void) {
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

void lesser_print_lover(void) {
	print_string("In other task\n");
	while (1)
		syscall();
}
