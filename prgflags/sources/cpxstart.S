// cpxstart.asm
//=============================================================================
// Startup file for CPX modules
//



// Globals
	.globl	cpxstart
	.globl	_saved

// Externs
	.globl	_cpx_preinit		// adapt to gcc calling conventions
	.globl	_cpx_init

	.text

cpxstart:
	move.l	4(sp),-(sp)
	jbsr	_cpx_preinit
	addq.l	#4,sp
	move.l	d0,4(sp)
	jbra	_cpx_init

	.data

_saved:	.short	0			// 0: no user-preference saved yet
					// non-zero: user-preference saved
	.end
