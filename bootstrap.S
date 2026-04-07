.global _start

_start:
	/* setup kernel stack */
	ldr sp, =__stack_top

	/* setup interrupt vector table */
	mov r0, #0x08
	ldr r1, =interrupt_table @ address of branch instruction 
	ldr r3, =interrupt_table_end

keep_loading:
	ldr r2, [r1, #0x0] @ load branch instruction into r2 (a single word)
	str r2, [r0, #0x0] @ store branch instruction into address 0x08
	add r0, r0, #0x4
	add r1, r1, #0x4
	cmp r1, r3
	bne keep_loading

	/* jump to kernel C code */
	bl start_kernel

interrupt_table:
	ldr pc, svc_entry_address
	nop
	nop
	nop
	ldr pc, irq_entry_address
	svc_entry_address: .word svc_entry
	irq_entry_address: .word irq_entry
interrupt_table_end:
