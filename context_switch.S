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
	Right now, leaving userspace is only voluntary. In particular, it only happens
	via svc_entry().

	In svc_entry(), userspace context is preserved on top of userspace stack.
	We just pop in the same order from userspace stack.
	The top of userspace stack is our mobile "task struct", for simplicity.

	Bear in mind that context switching is just a function call. Just like
	functions preserve state on the top of their stack frame, you can think of
	this in a similar fashion.
	In particular, think of each CPU as having a stack of contexts.
	The stack of contexts corresponds to the stack of a process.
	In this stack of contexts, each context has its own stack.
	This is analogous to the stack frame of a function in the stack of a proc.
	The stack frame itself is a stack within a stack.

	The big difference is management of the contexts. In a process, they are stacked.
	In the kernel, they are not. In our case, ARM banking makes them easy to manage.
	In other architectures, it may be more complex.

	$r0 = top of userspace stack.
	*/
	@ get SPSR and lr
	ldmfd r0!, {ip, lr}
	msr SPSR, ip

	cps #0x1F @ switch to system mode

	mov sp, r0												@ restore userspace stack
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}		@ restore userspace registers

	cps #0x13 @ switch to back supervisor mode, $lr = next location in userspace proc

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
	cps #0x1F
	
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, fp, ip, lr} @ save task registers onto its stack
	mov r0, sp 													   @ save task sp into r0 to survive mode switch

	cps #0x13

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

irq_entry
	
