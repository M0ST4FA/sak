#include "error.h"
#include "kernel/ipc.h"
#include "kernel/lib.h"
#include "kernel/panic.h"
#include "kernel/task.h"
#include "lib.h"
#include "uapi/syscall.h"
#include "uapi/types.h"

// Forward declarations of private functions
ssize_t do_write(struct task *);
ssize_t do_read(struct task *);

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
	kmemcpy((char *)child_sp, (char *)parent_sp, n);

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

ssize_t do_read(struct task *task) {
	struct pipe_ringbuffer *pipe = 0;
	size_t pipe_sz = 0, i = 0;
	ssize_t ret = 0;
	struct task *t = 0;

	// 1. Read syscall arguments
	const int fd = task->sp[R0];
	char *user_buf = 0;
	const size_t count = task->sp[R2];

	// 2. Check limits
	if (fd > PIPE_NR)
		return -EBADF;
	if (count > PIPE_BUF_SZ)
		return -EMSGSIZE;

	// 3. Perform the actual read
	pipe = &pipes[fd];
	pipe_sz = pipe_len(pipe);

	if (count > pipe_sz) {
		task->state = TS_WAIT_READ;
		goto success;
	}

	user_buf = (char *)task->sp[R1];
	for (i = 0; i < count; i++)
		pipe_pop(pipe, user_buf++);
	ret = i; // how many elements did we (proc 0) give to user proc?

	// 4. Unblock any task blocking on write syscall
	for (i = 0; i < task_count; t = &(tasks[i++]))
		if (t->state == TS_WAIT_WRITE)
			do_write(t);

success:
	return ret;
}

ssize_t do_write(struct task *task) {
	struct pipe_ringbuffer *pipe = 0;
	size_t pipe_sz = 0, i = 0;
	ssize_t ret = 0;
	struct task *t = 0;

	// 1. Read syscall arguments
	const int fd = task->sp[R0];
	char *user_buf = 0; // read later; you may err before you need it
	const size_t count = task->sp[R2];

	// 2. Check limits
	if (fd > PIPE_NR)
		return -EBADF;
	if (count > PIPE_BUF_SZ)
		return -EMSGSIZE;

	// 3. Perform the actual write
	pipe = &pipes[fd];
	pipe_sz = pipe_len(pipe);

	if (pipe_sz < count) {
		task->state = TS_WAIT_WRITE;
		goto success;
	}

	user_buf = (char *)task->sp[R1];
	for (i = 0; i < count; i++)
		pipe_push(pipe, user_buf[i]);
	ret = i; // how many elements did we (proc 0) read from user proc?

	// 4. Unblock any task waiting on a read (i.e. in state TS_WAIT_READ)
	for (i = 0; i < task_count; t = &(tasks[i++]))
		if (t->state == TS_WAIT_READ)
			do_read(t); // Note: proc 0 performs reads & writes on behalf of the task

success: // we may need to free some things here in the future
	return ret;
}

int sys_getpid(void) {
	const struct task *t = &(tasks[current]);
	t->sp[R0] = current;

	return current;
}

ssize_t sys_write(void) {
	return do_write(&(tasks[current]));
}
ssize_t sys_read(void) {
	return do_read(&(tasks[current]));
}
