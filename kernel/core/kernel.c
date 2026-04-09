#include "arch/versatilepb.h"
#include "kernel/lib.h"
#include "kernel/panic.h"
#include "kernel/sysent.h"
#include "kernel/task.h"
#include "kernel/timer.h"
#include "types.h"
#include "uapi/syscall.h"

void start_kernel(void) {
	unsigned int *saved_stack, ret;

	// ============================ SETUP KERNEL SUBSYSTEMS ==========================
	tasks_init();
	timer_init(TIMER0, TIMER_PERIODIC, 5); // setup system timer

	print_string("Kernel started...\n");

	// ========================= ENTER KERNEL MAIN LOOP ==============================
	while (1) {

		print_string("Scheduling task: ");
		print_int(current);
		print_char('\n');

		saved_stack = activate(tasks[current].sp);

		// ==================== NOTE: svc_entry() returns here  ======================

		tasks[current].sp = saved_stack;

		print_string("Back to kernel...\n");

		// 1. Handle syscall of current task
		switch (tasks[current].sp[R7]) {
				// =========================== SYSCALLS ================================
			case SYSCALL_YIELD:
				break;

			case SYSCALL_FORK:
				if ((ret = sys_fork())) {
					print_string("fork: Error during fork (");
					print_int(ret);
					print_string(")\n");
				}
				break;

			case SYSCALL_WRITE:
				if ((ret = sys_write())) {
					print_string("write: Error during write (");
					print_int(ret);
					print_string(")\n");
				}
				break;
			case SYSCALL_READ:
				if ((ret = sys_read())) {
					print_string("read: Error during read (");
					print_int(ret);
					print_string(")\n");
				}
				break;

			// ================================= IRQs ===================================
			case -PIC_IRQ_TIMER01:
				if (*(TIMER0 + TIMER_MIS)) { // timer went off, i.e., not spurious or bug
					print_string("system tick\n");
					*(TIMER0 + TIMER_INTCLR) = 1;
				}
				if (*(TIMER1 + TIMER_MIS)) {
					*(TIMER1 + TIMER_INTCLR) = 1;
					print_string("kernel tick\n");
				}
				break;

			default:
				panic(PANIC_SYSCALL);
		}

		//	wait_seconds(1);

		// 2. Select next process to run
		do {
			current = (current + 1) % PROC_NR;

		} while (!task_entry_available(current));
	};

	print_string("After loop...should never reach here\n");
}
