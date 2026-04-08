#include "versatilepb.h"
#include "armv7.h"

.global activate
.global svc_entry
.global irq_entry

/* 
Switch to process whose stack is passed as first argument.
@param $r0 the top of the stack userspace process we want to switch to
@returns 

It is important to realize that:
1. In our kernel, proc 0 (kernel proc) handles all system calls
2. Our kernel is non-preemptive. This means that processes get back to
	the kernel voluntarily. The only way to do this is through a syscall.
3. Context switching is never done by a userspace proc. Only by the kernel.
	Thus, the state we save here is that of the kernel. When we return, it
	is to $lr of the kernel.
4. $lr of the kernel points to the instruction just after activate() call.
	The call is inside main kernel loop.
*/
activate:

	/* ===========================================================================
	1. Preserve kernel context for later.
	============================================================================ */
	mov ip, sp
	push {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}


	/* ===========================================================================
	2. Restore userspace process context. Process has already been chosen by 
	schedular (in main kernel loop)
	============================================================================ */

	/*
	Leaving userspace is:
		1. Voluntary thorugh svc_entry()
		2. Involuntary through interrupt
	
	In svc_entry() or irq_entry(), userspace context is preserved on top of userspace
	stack. We just pop in the same order from userspace stack.
	The top of userspace stack for a non-running process allways stores most of itself
	context. The rest of the context is stored in the task struct in the tasks table.
	All tables are indexed by proc pid.

	Bear in mind that context switching is just a function call. This is true for
	both syscalls and interrupts.
	Just like functions preserve state on the top of their stack frame, you can think
	about this in a similar fashion.

	In particular, think of each CPU as having a stack of contexts.
	There are two types of contexts:
		1. Process context, which consists of the CPU + memory state of a process
		2. Interrupt context, which consists of the CPU + memory state of an interrupt
			handler.
	In this stack of contexts, each context has its own stack, process stack or interrupt
	stack. It also has other metadata (like task struct of current task).
	A single context is analogous to the stack frame of a function in the stack of a proc.
	The stack frame itself is a stack within a stack.

	The big difference is management of the contexts. In a process, they are stacked.
	In the kernel, they are not necessarilly. In our case, ARM banking makes them easy
	to manage. In other architectures, it may be more complex.

	$r0 = top of userspace stack.
	*/
	@ get SPSR and lr
	ldmfd r0!, {ip, lr}
	msr SPSR, ip

	cps #MODE_SYS @ switch to system mode

	mov sp, r0												@ restore userspace stack
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}		@ restore userspace registers
	pop {r7}												@ r7 saved by interrupt or svc entry
	cps #MODE_SVC @ switch to back supervisor mode, $lr = next location in userspace proc

	/* ===========================================================================
	3. Move to userspace proc. The "s" means make CPSR = SPSR atomically.
	============================================================================ */
	movs pc, lr

/* because we're having a microkernel, system calls are a bit strange */
svc_entry:

	/* =========================================================================== 
	1. Preserve userspace context (cpu context -> registers, memory context -> stack, page tables, ...etc) 
	==============================================================================*/
	/* 
	switch to system mode (kernel mode with user space banks = usr_lr, usr_sp), preserve registers, them switch back
	usr_lr = the return address of the userspace wrapper. after we return to the wrapper, the wrapper will use it to
			return to its caller
	usr_sp = the userspace stack register. we use it to preserve userspace context. 

	right now, we don't have formal a task struct yet. in particular, we have an informal task struct.
	it doesn't have a fixed position and is always at the top of userspace stack.

	right now, we only have CPU context to preserve, i.e. registers. this is easy to do by just pushing them to userspace
	stack, so we can spare complex memory management for a dedicated struct for now.
	*/
	cps #MODE_SYS
	
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr} @ save task registers onto its stack
	mov r0, sp 													   @ save task sp into r0 to survive mode switch

	cps #MODE_SVC

	/* 
	just like push, but on r0 instead of sp, i.e. to userspace stack
	r0 is the sp of the task we saved before changing mode

	lr contains the address we will switch to later, after servicing the syscall
	this one is different from the one we saved before
	this one is set by the svc instruction, and points to the instruction (in user space)
	just after the svc instruction. We will use this to return back

	let's call this lr2 and the one we saved before lr1

	the flow is as follows:
	userspace function -> wrapper -> lr1 points to userspace function after wrapper -> 
	syscall -> points to instruction inside wrapper after syscall

	we have to preserve both and return in opposite direction. both are preserved
	in userspace stack
	*/
	mrs ip, SPSR        @ save the task's mode into ip
	stmfd r0!, {ip, lr}

	/*============================================================================
	2. Restore kernel context from kernel stack. This was saved in context_switch().
	============================================================================*/
	
	/* 
	Effectively, kernel runs as proc 0. In this step, we're switcing back to proc 0
	to service the calling processes need. 

	In other words: In our microkernel, systemcalls are not implemented as kernel
	services. Instead, they are implemented by proc 0, and the svc instruction +
	calling convention are used as an IPC mechanism.

	It is important to realize that this is possible only because we have a
	non-preemptive kernel. Otherwise, we lose track of which user process proc 0 is
	currently servicing. At this moment, it is whatever process is "current"
	*/

	pop {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}	
	mov sp, ip
	bx lr

/*
Generic tophalf of interrupt handling. Very similar to svc_entry.
*/
irq_entry:
	
	/*============================================================================
	1. Simulate svc entry. Calculate vic number and put it in place syscall nr.
	============================================================================*/
	/* 
	Simulate svc since: 
		1. Handler in the kernel is the same
		2. We return to user space via activate(). activate() will restore r7 for userspace proc.
	*/

	cps #MODE_SYS

	push {r7}
	ldr r7, =PIC_BASE
	ldr r7, [r7, #PIC_REG_IRQSTATUS] 		@ r7 now contains the vector number

	/* 
	Get most significant bits and negate them. 

	We use the same namespace for both irq vectors and syscall numbers.
	Basically, interrupts are handled by proc 0 (kernel proc). In particular,
	this irq_entry() function is the top half of interrupt handling. proc 0
	has the bottom half. This bottom half is shared between interrupts and
	syscalls.

	This is not a complete top-half. There's no ack here. Kernelspace
	part has the rest of this and the bottom-half.
	*/
	clz r7, r7							@ same as: 		r7 = r7 & 0x00FF
	sub r7, r7, #31						@ 2^16 = 32; 	r7 = r7 - 31 = -r7 (31 because of how 2's complement works)
	
	/*============================================================================
	2. Save user/kernel state
	============================================================================*/

	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}   @ this saves the state of whatever process was interrupted (kernel or user)
	mov r0, sp														 @ sp is banked
	
	cps #MODE_IRQ							@ exceptions set the mode to irq or fiq; here irq

	mrs ip, SPSR
	sub lr, lr, #0x4						@ because the stupid processor advances it two instructions; doesn't happen on svc
	stmfd r0!, {ip, lr}

	/*============================================================================
	2. Switch back to proc 0 to handle the interrupt. proc 0 has the bottom-half.
	============================================================================*/
	cps #MODE_SVC
	pop {r4, r5, r6, r7, r8, r9, r10, fp, ip, lr}
	mov sp, ip	
	bx lr
