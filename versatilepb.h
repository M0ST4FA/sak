#pragma once

// ==================== UART'S =====================
#define UART0 ((volatile unsigned int *)0x101f1000)
#define UARTFR 0x06		 // UART flag register (offset from UART0)
#define UARTFR_TXFF 0x20 // Mask to get the bit representing if transmission buffer is full

// ==================== TIMERS =====================
// Timer addresses
#define TIMER0 ((volatile unsigned int *)0x101e2000)
#define TIMER1 ((volatile unsigned int *)0x101e2020)
#define TIMER2 ((volatile unsigned int *)0x101e3000)
#define TIMER3 ((volatile unsigned int *)0x101e3020)

// Timer register offsets in !!!UNSIGNED INT!!!
#define TIMER_VALUE 0x1
#define TIMER_CONTROL 0x2
#define TIMER_INTCLR 0x3
#define TIMER_MIS 0x5

// Timer interesting register values
#define TIMER_EN 0x80
#define TIMER_PERIODIC 0x40
#define TIMER_INTEN 0x20
#define TIMER_32BIT 0x02
#define TIMER_ONESHOT 0x01

// ==================== VIC'S =====================
// PIC: https://developer.arm.com/documentation/dui0225/d/programmer-s-reference/interrupt-controllers/primary-interrupt-controller?lang=en
// SIC: https://developer.arm.com/documentation/dui0225/d/programmer-s-reference/interrupt-controllers/secondary-interrupt-controller

// Base addresses of the VIC's.
#define PIC ((volatile unsigned int *)0x10140000)
#define PIC_BASE 0x10140000

#define SIC ((volatile unsigned int *)0x10003000)
#define SIC_BASE 0x10003000

// Note: Register offsets in !!!UNSIGNED INT!!!. Only the ones I use.
#define PIC_REG_IRQSTATUS 0x0
#define PIC_REG_FIQSTATUS 0x1
#define PIC_REG_RAWINTR 0x2
#define PIC_REG_INTEN 0x4
#define PIC_REG_INTENCLR 0x14
#define PIC_REG_SOFTINT 0x18
#define PIC_REG_SOFTINTCLR 0x1c
#define PIC_REG_VECTADDR 0x30
#define PIC_REG_DEFVECTADDR 0x34 // The address of the default fector handler
#define PIC_REG_VECTADDR0 0x100	 // The address of handler of vect 0
#define PIC_REG_VECTADDR15 0x13c // The address of the handler of vect 15

// IRQs for different events
#define PIC_IRQ_TIMER01 4
#define PIC_MASK_TIMER01 0x10
#define PIC_IRQ_TIMER23 5
#define PIC_IRQ_SIC 31
