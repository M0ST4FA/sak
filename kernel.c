#include "asm.h"
#include "lib.h"

#define USR_STACK_SZ 256

unsigned int first_stack[USR_STACK_SZ] = {0};
unsigned int *first_stack_start = first_stack + USR_STACK_SZ - 16;

void first(void) {
	print_string("In user mode\n");
	while (1)
		;
}

int start_kernel() {

	print_string("Kernel started...\n");

	first_stack_start[0] = 0x10; // user mode
	first_stack_start[1] = (unsigned int)&first;
	activate(first_stack_start);

	return 0;
}
