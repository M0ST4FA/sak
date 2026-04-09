#include "kernel/panic.h"
#include "lib.h"

void panic(unsigned int cause) {
	// TODO: We need some sort of context dump

	switch (cause) {
		case PANIC_SYSCALL:
			kprint_string("panic: Invalid syscall!\n");
			break;

		case PANIC_ASSERT:
			kprint_string("panic: Assertion failed!\n");
			break;

		default:
			kprint_string("panic: Unknown reason!\n");
	}

	asm volatile(
		"wfi");
	kprint_string("After wfi...should never reach here\n");
}
