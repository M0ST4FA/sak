#include "kernel/error.h"
#include "kernel/ipc.h"
#include "kernel/lib.h"
#include "kernel/panic.h"
#include "kernel/task.h"
#include "types.h"
#include "uapi/syscall.h"

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

int read(struct task *task) {
}

int write(struct task *task, int fd) {
}

int sys_write(void) {
	struct task *task = &tasks[current];
	struct pipe_ringbuffer *pipe = 0;
	size_t pipe_left = 0, i = 0;

	// 1. Read syscall arguments
	const int fd = task->sp[R0];
	char *user_buf = 0; // read later; you may err before you need it
	const int count = task->sp[R1];

	// 2. Check limits
	if (fd > PIPE_NR)
		return -EBADF;
	if (count > BUF_SIZE)
		return -EMSGSIZE;

	// 3. Perform the actual write
	pipe = &pipes[fd];
	pipe_left = pipe_len(pipe);

	if (pipe_left < count) {
		task->state = TS_WAIT_WRITE;
		goto success;
	}

	user_buf = (char *)task->sp[R1];
	for (i = 0; i < count; i++)
		pipe_push(pipe, user_buf[i]);

	// 4. Unblock any task waiting on a read (i.e. in state TS_WAIT_READ)
	for (i = 0; i < task_count; i++)
		if (tasks[i].state == TS_WAIT_READ)
			read(task);

success: // we may need to free some things here in the future
	return 0;
}
