
.section .text

.globl _HandleIrq
_HandleIrq:

	sub		lr, 	lr, 	#4

	srsdb	0x1f!

	cpsid	if,		0x1f
	mrs		r12, spsr	
	push	{r0-r12, lr}

	and		r4, 	sp,		#4
	sub		sp,		sp,		r4
	
	push {r4,r5}

	mov		r0, sp
	bl		StoreStack

	//mov		r0, #65
	//bl		uart_putc

	bl		_DataMemoryBarrier
	
	bl		IdentifyIrq
	
	//mov		r0, #66
	//bl		uart_putc
	
	//cpsie	i
	
	bl		HandleIrq

	//cpsid	i

	//mov		r0, #67
	//bl		uart_putc
	
	bl		_DataMemoryBarrier
	
	bl		RestoreStack
	cmp		r0, #0
	movne	sp, r0

	pop {r4,r5}
	add	sp,		sp,		r4

	pop		{r0-r12, lr}
	msr		spsr, r12
	rfeia	sp!

.globl _EnableIrq
_EnableIrq:
	cpsie	i
	bx		lr

.globl _DisableIrq
_DisableIrq:
	cpsid	i
	bx		lr

.globl _DataMemoryBarrier
_DataMemoryBarrier:
	mov		r12, 	#0
	mcr		p15, 0, r12, c7, c10, 5
	mov		pc,		lr	

.globl _Idle
_Idle:
	wfi
	bx		lr

.globl _GetStack
_GetStack:
	mov		r0,		r13
	bx		lr

.globl _GetCpsr
_GetCpsr:
	mrs		r0,		cpsr
	bx		lr

