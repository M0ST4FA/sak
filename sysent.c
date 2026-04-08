#include "syscall.h"
#include "lib.h"
#include "panic.h"
#include "task.h"
#include "types.h"

int sys_fork(void) {
	int child_pid, n;
	unsigned int *parent_sp, *parent_stack_top, *child_stack_top, *child_sp;
	size_t stack_offset;

	parent_sp = (unsigned int *)tasks[current].sp;

	if (task_count >= PROC_NR) {
		parent_sp[R0] = -1; // set return value for parent proc ("current")
		return -1;
	}
	// ===== 1. Allocate child pid and proc entry
	// should never happen
	if ((child_pid = task_next_pid()) < 0)
		return -PANIC_ASSERT;

	// stack pointer of child has to have the same offset as parent
	parent_stack_top = stacks[current] + USR_STACK_SZ;
	child_stack_top = stacks[child_pid] + USR_STACK_SZ;

	stack_offset = parent_stack_top - parent_sp;
	child_sp = child_stack_top - stack_offset;

	// ===== 2. Copy parent context to child (with some modifications)
	// Because of our decision to not have a task struct, copying context = copying stack
	// Notice how we share text; only stack (which include proc context) is copied
	n = stack_offset * sizeof(*stacks[current]);
	memcpy((char *)child_sp, (char *)parent_sp, n);

	// ===== 3. Set up child task struct
	tasks[child_pid].state = TS_RUNNABLE;
	tasks[child_pid].entry_point = tasks[current].entry_point;
	tasks[child_pid].sp = child_sp;
	task_count++;

	// ===== 4. Set return values for parent and child
	parent_sp[R0] = child_pid;
	child_sp[R0] = 0;

	return 0;
}
