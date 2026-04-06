#define UART0 ((volatile unsigned int *)0x101f1000)
#define UARTFR 0x06		 // UART flag register (offset from UART0)
#define UARTFR_TXFF 0x20 // Mask to get the bit representing if transmission buffer is full
