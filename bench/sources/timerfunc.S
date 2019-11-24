	.extern	_old_timer_handler

	.globl	_timerfunc
	.globl	_vex_counter
	.text
	.even
timerfunc:
_timerfunc:
	move.l	d0,-(sp)
	move.l	_vex_counter,d0
	addq.l	#1,d0
	move.l	d0,_vex_counter
	move.l	(sp)+,d0
	move.l	_old_timer_handler,-(sp)
	rts

	.bss

	.even
_vex_counter:
	.long	0
