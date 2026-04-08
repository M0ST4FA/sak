#include "asm.h"
#include "lib.h"
#include "panic.h"
#include "syscall.h"
#include "task.h"
#include "types.h"
#include "versatilepb.h"

int panic_cause = PANIC_NR; // we start with 3 tasks (static system tasks)

void timer_setup(volatile unsigned int *timer, int mode, int seconds) {
	// 1. Tell VIC to accept interrupts from timer0
	*(PIC + PIC_REG_INTEN) = PIC_MASK_TIMER01;

	// 2. Program the timer
	*timer = seconds * 1000000;
	*(timer + TIMER_CONTROL) = TIMER_EN | mode | TIMER_32BIT | TIMER_INTEN;
}

void wait_seconds(int seconds) { // kernel uses TIMER1
	timer_setup(TIMER1, TIMER_ONESHOT, seconds);

	asm volatile("wfi"); // irq fires -> fallthrow

	print_string("tick: kernel (which is to say, entire machine) was resting for some time...\n");
}

void start_kernel(void) {
	unsigned int *saved_stack, current = 0, ret;
	char buf[BUF_SIZE] = {0};

	tasks_setup();
	// setup system timer
	timer_setup(TIMER0, TIMER_PERIODIC, 2);

	print_string("Kernel started...\n");

	// main kernel loop
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
			case SYSCALL_YIELD:
				break;

			case SYSCALL_FORK: {
				int child_pid, n;
				unsigned int *parent_sp, *parent_stack_top, *child_stack_top, *child_sp;
				size_t stack_offset;

				parent_sp = (unsigned int *)tasks[current].sp;

				if (task_count >= PROC_NR) {
					parent_sp[R0] = -1; // set return value for parent proc ("current")
					break;
				}
				// ===== 1. Allocate child pid and proc entry
				// should never happen
				if ((child_pid = task_next_pid()) < 0) {
					panic_cause = PANIC_ASSERT;
					goto panic;
				}

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
			};
				break;
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
				panic_cause = PANIC_SYSCALL;
				goto panic;
		}

		wait_seconds(1);

		// 2. Select next process to run
		do {
			char buf[BUF_SIZE] = {0};
			current = (current + 1) % PROC_NR;

		} while (!task_entry_available(current));
	};

	print_string("After loop...should never reach here\n");

panic:
	// TODO: We need some sort of context dump
	// TODO: Convert into a tail function (eventually halts and never returns)

	switch (panic_cause) {
		case PANIC_SYSCALL:
			print_string("panic: Invalid syscall!\n");
			break;

		case PANIC_ASSERT:
			print_string("panic: Assertion failed!\n");
			break;

		default:
			print_string("panic: Unknown reason!\n");
	}

	asm volatile(
		"wfi");
	print_string("After wfi...should never reach here\n");
}
