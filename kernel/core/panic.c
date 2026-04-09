#include "kernel/panic.h"
#include "arch/versatilepb.h"
#include "lib.h"

void panic(unsigned int cause, const char *msg) {
	// TODO: We need some sort of context dump

	*(PIC + PIC_REG_INTEN) = 0; // Disable interrupts

	switch (cause) {
		case PANIC_SYSCALL:
			kprint_string("panic: Invalid syscall\n");
			break;

		case PANIC_ASSERT:
			kprint_string("panic: Assertion failed!\n");
			break;

		default:
			kprint_string("panic: Unknown reason!\n");
	}

	if (msg) {
		kprint_string("\tpanic message: ");
		kprint_string(msg);
		kprint_char('\n');
	}

	while (1)
		asm volatile(
			"wfi");

	kprint_string("After wfi...should never reach here\n");
}
