#pragma once

enum {
	PANIC_SYSCALL,
	PANIC_ASSERT,
	PANIC_NR,
};

extern unsigned panic_cause;
extern void panic(unsigned int cause, const char *);
