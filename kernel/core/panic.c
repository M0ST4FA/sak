#include "kernel/panic.h"
#include "lib.h"

void panic(unsigned int cause) {
	// TODO: We need some sort of context dump

	switch (cause) {
		case PANIC_SYSCALL:
			print_string("panic: Invalid syscall!\n");
			break;

		case PANIC_ASSERT:
			print_string("panic: Assertion failed!\n");
			break;

		default:
			print_string("panic: Unknown reason!\n");
	}

	asm volatile(
		"wfi");
	print_string("After wfi...should never reach here\n");
}
