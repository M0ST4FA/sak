.global activate
.global kernel_return

activate:
mov ip, sp
push {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}

@ get SPSR and lr
ldmfd r0!, {ip, lr}
msr SPSR, ip

cps #0x1F @ switch to system mode
mov sp, r0
pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
cps #0x13 @ switch to back supervisor mode

movs pc, lr

kernel_return:
	cps #0x1F
	
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr} @ save task registers onto its stack
	mov r0, sp 													   @ save task sp into r0 to survive mode switch

	cps #0x13

	mrs ip, SPSR        @ save the task's mode into ip
	stmfd r0!, {ip, lr} @ just like push, but on r0 instead of sp; r0 is the sp of the task;

	pop {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}	
	mov sp, ip
	bx lr

