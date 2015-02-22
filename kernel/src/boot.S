
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
_undefined_handler:	.word 	halt
_swi_handler:		.word	halt
_prefetch_handler:	.word	halt
_data_handler:		.word	halt
_unused_handler:	.word	halt
_irq_handler:		.word   _HandleIrq
_fiq_handler:		.word	halt

reset:
	// Set up stack for IRQ and SYS modes.
   	orr		r3, 	r4, 	#0x12
   	msr		cpsr_c, 	r3
   	mov		sp, 	#0x4000
	cpsid   if,     0x1f	
	ldr		r0,		=exec_location
	ldr		sp,	 	[r0]

	// Copy exception handler table to 0x0000
	mov		r0, 	#0x8000
	mov 	r1, 	#0x0000

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

	ldr		r3,		=main
	blx		r3

halt:
	ldr		r0,		=halt_msg
	bl		uart_puts
really_halt:
	wfe
	b		really_halt


.globl Get32
Get32:
	ldr		r0,		[r0]
	bx		lr

.globl Put32
Put32:
	str		r1,		[r0]
	bx		lr

.section .data
halt_msg:	.ascii "Halting\n\0"

exec_location: .int 0x7ff8


