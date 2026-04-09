#include "arch/versatilepb.h"
#include "kernel/lib.h"
#include "kernel/panic.h"
#include "kernel/sysent.h"
#include "kernel/task.h"
#include "kernel/timer.h"
#include "sched.h"
#include "uapi/syscall.h"
#include "uapi/types.h"

void start_kernel(void) {
	unsigned int *saved_stack, ret;

	// ============================ SETUP KERNEL SUBSYSTEMS ==========================
	tasks_init();
	timer_init(TIMER0, TIMER_PERIODIC, 1); // setup system timer

	kprint_string("Kernel started...\n");

	// ========================= ENTER KERNEL MAIN LOOP ==============================
	while (1) {

		kprint_string("Scheduling task: ");
		kprint_int(current);
		kprint_char('\n');

		if (!task_entry_allocated(current))
			panic(PANIC_ASSERT, "Scheduling unallocated task");

		if (task_waiting(current))
			panic(PANIC_ASSERT, "Scheduling waiting task");

		saved_stack = activate(tasks[current].sp);

		// ==================== NOTE: svc_entry() returns here  ======================

		tasks[current].sp = saved_stack;

		kprint_string("Back to kernel...\n");

		// 1. Handle syscall of current task
		switch (tasks[current].sp[R7]) {
				// =========================== SYSCALLS ================================
			case SYSCALL_YIELD:
				break;

			case SYSCALL_FORK:
				if ((ret = sys_fork())) {
					kprint_string("fork: Error during fork (");
					kprint_int(ret);
					kprint_string(")\n");
				}
				break;

			case SYSCALL_WRITE:
				if ((ret = sys_write())) {
					kprint_string("write: Error during write (");
					kprint_int(ret);
					kprint_string(")\n");
				}
				break;
			case SYSCALL_READ:
				if ((ret = sys_read())) {
					kprint_string("read: Error during read (");
					kprint_int(ret);
					kprint_string(")\n");
				}
				break;

			// ================================= IRQs ===================================
			case -PIC_IRQ_TIMER01:
				if (*(TIMER0 + TIMER_MIS)) { // timer went off, i.e., not spurious or bug
					kprint_string("system tick\n");
					*(TIMER0 + TIMER_INTCLR) = 1;
				}
				if (*(TIMER1 + TIMER_MIS)) {
					*(TIMER1 + TIMER_INTCLR) = 1;
					kprint_string("kernel tick\n");
				}
				break;

			default: {
				char buf[BUF_SIZE] = {};
				if (!kinttostr(tasks[current].sp[R7], buf))
					kprint_string("Couldn't convert string: number is likely negative!");

				panic(PANIC_SYSCALL, buf);
			}
		}

		//	wait_seconds(1);

		// 2. Select next process to run
		do {
			current = (current + 1) % PROC_NR;
		} while (!task_runnable(current));
	};

	kprint_string("After loop...should never reach here\n");
}
