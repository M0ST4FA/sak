#include "syscall.h"

.global kernel_yield

/* function to be called from user mode to trap into the kernel */
kernel_yield:
	/* 
	preserve r7 as user space (the caller, written in C) doesn't expect it changed upon return; 
	r7 is calle saved according to the ARM calling convention, followed by C compiler 
	*/
	push {r7} 

	mov r7, #SYSCALL_YIELD 	@ syscall number for yielding to the kernel
	svc #0					@ trap into the kernel syscall handler; executed in the context of the current process
	@ pop {r7} not needed as activate() restores it
	/* move back to user space; do not store anything in link register (which preserves) */
	bx lr

.global fork
fork:
	push {r7}
	mov r7, #SYSCALL_FORK
	svc #0
	bx lr
