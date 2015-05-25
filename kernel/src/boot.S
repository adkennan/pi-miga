
.section .text.boot

.globl Start
Start:
	ldr 	pc, 	_reset_handler
	ldr 	pc, 	_undefined_handler
	ldr 	pc, 	_swi_handler
	ldr 	pc, 	_prefetch_handler
	ldr 	pc, 	_data_handler
	ldr 	pc, 	_unused_handler
	ldr 	pc, 	_irq_handler
	ldr 	pc, 	_fiq_handler

_reset_handler:		.word	reset
_undefined_handler:	.word 	halt_undef
_swi_handler:		.word	halt_swi
_prefetch_handler:	.word	halt_pref
_data_handler:		.word	halt_data
_unused_handler:	.word	halt_unusued
_irq_handler:		.word   _HandleIrq
_fiq_handler:		.word	halt_fiq

_has_init:	.word	0

reset:
	ldr		r0,		=_has_init
	ldr		r1,		[r0]
	cmp		r1, 	#0
	bne		halt_reset
	mov		r1,		#1
	str		r1,	[r0]

	// Set up stack for IRQ and SYS modes.
   	orr		r3, 	r4, 	#0x12
   	msr		cpsr_c, 	r3
   	mov		sp, 	#0x4000
	cpsid   if,     0x1f
	mov 	sp, 	#0x8000
	mov		r10,	r2

	// Copy exception handler table to 0x0000
	ldr		r0, 	=Start
	mov 	r1, 	#0

	ldmia 	r0!, 	{r2-r9}
	stmia 	r1!, 	{r2-r9}
	ldmia 	r0!, 	{r2-r9}
	stmia 	r1!, 	{r2-r9}

	// Clear bss
	mov 	r5, 	#0
	mov 	r6, 	#0
	mov 	r7, 	#0
	mov 	r8, 	#0

	ldr		r4, 	=_bss_start
	ldr 	r9, 	=_bss_end

l1:
	stmia 	r4!, 	{r5-r8}
	cmp 	r4,		r9
	blo		l1

	mov		r0,		r10
	b 		main
	b		halt

halt_reset:
	mov 	sp, 	#0x8000
	bl		HandleReset
	b		halt
	
halt_undef:
	mov		sp,		#0x8000
	mov		r0,		r14
	bl		HandleUndefinedInstruction
	b		halt

halt_swi:
	mov		r0,		#49
	b		halt

halt_pref:
	mov		r0,		#50
	b		halt

halt_data:
	mov		r0,		#51
	b		halt

halt_unusued:
	mov		r0,		#52
	b		halt

halt_fiq:
	mov		r0,		#53

halt:
	bl		DebugPutChar
//	srsdb	0x1f!
//	cpsid	if,	0x1f
halted:
//	wfi
	b		halted

.globl Put32
Put32:
	str r1, [r0]
	bx lr

.globl Get32
Get32:
	ldr r0, [r0]
	bx lr

