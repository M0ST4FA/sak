.global _start

_start:
	ldr sp, =__stack_top
	bl start_kernel

