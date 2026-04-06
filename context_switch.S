.global activate
.global context_switch
.global save_context
.global restore_context

save_context:
	stmfd r0!, {r4-r11, lr} @ store callee-saved registers and LR
	bx lr

restore_context:
	ldmfd r0!, {r4-r11, lr} @ restore callee-saved registers an LR
	bx lr

/* High-level switch: save old, load new */
context_switch:
@ r0 = pointer to old task stack
@ r1 = pointer to new task stack

activate:
@ get SPSR and lr
ldmfd r0!, {ip, lr}
msr SPSR, ip

cps #0x1F @ switch to system mode
mov sp, r0
pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
cps #013 @ switch to back supervisor mode

movs pc, lr
