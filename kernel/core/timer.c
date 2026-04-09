#include "timer.h"
#include "arch/versatilepb.h"
#include "lib.h"

void timer_init(volatile unsigned int *timer, int mode, int seconds) {
	// 1. Tell VIC to accept interrupts from timer0
	*(PIC + PIC_REG_INTEN) = PIC_MASK_TIMER01;

	// 2. Program the timer
	*timer = seconds * 1000000;
	*(timer + TIMER_CONTROL) = TIMER_EN | mode | TIMER_32BIT | TIMER_INTEN;
}

void wait_seconds(int seconds) { // kernel uses TIMER1
	timer_init(TIMER1, TIMER_ONESHOT, seconds);

	asm volatile("wfi"); // irq fires -> fallthrow

	kprint_string("tick: kernel (which is to say, entire machine) was resting for some time...\n");
}
