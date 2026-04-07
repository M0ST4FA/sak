#pragma once

#define UART0 ((volatile unsigned int *)0x101f1000)
#define UARTFR 0x06		 // UART flag register (offset from UART0)
#define UARTFR_TXFF 0x20 // Mask to get the bit representing if transmission buffer is full

// Timer addresses
#define TIMER0 ((volatile unsigned int *)0x101e2000)
#define TIMER1 ((volatile unsigned int *)0x101e2020)
#define TIMER2 ((volatile unsigned int *)0x101e3000)
#define TIMER3 ((volatile unsigned int *)0x101e3020)

// Timer PICs
#define PIC_TIMER01 4
#define PIC_TIMER23 5

// Timer register offsets
#define TIMER_VALUE 0x1
#define TIMER_CONTROL 0x2
#define TIMER_INTCLR 0x3
#define TIMER_MIS 0x5

// Timer interesting register values
#define TIMER_EN 0X80
#define TIMER_PERIODIC 0x40
#define TIMER_INTEN 0x20
#define TIMER_32BIT 0x02
#define TIMER_ONESHOT 0x01
