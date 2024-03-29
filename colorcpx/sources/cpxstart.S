// cpxstart.asm
//=============================================================================
// Startup file for CPX modules
//



// Globals
	.globl	cpxstart
	.globl	_saved
	.globl	_usr_vals
	.globl	_def_vals

	.text

cpxstart:
	move.l	4(sp),-(sp)
	jbsr	_cpx_preinit
	addq.l	#4,sp
	move.l	d0,4(sp)
	jbra	_cpx_init

	.data


_saved:
	.short	0			| 0: no user-preference saved yet
					| non-zero: user-preference saved
_usr_vals:				| saved user-preference
	.short	0x03E8, 0x03E8, 0x03E8	| Pen 0
	.short	0x0000, 0x0000, 0x0000	| Pen 1
	.short	0x03E8, 0x0000, 0x0000	| 2
	.short	0x0000, 0x03E8, 0x0000	| 3
	.short	0x0000, 0x0000, 0x03E8	| 4
	.short	0x0000, 0x03E8, 0x03E8	| 5
	.short	0x03E8, 0x03E8, 0x0000	| 6
	.short	0x03E8, 0x0000, 0x03E8	| 7
	.short	0x0320, 0x0320, 0x0320	| 8  - NEW
	.short	0x01F4, 0x01F4, 0x01F4	| 9  - NEW
	.short	0x01F4, 0x0000, 0x0000	| 10 - NEW
	.short	0x0000, 0x01F4, 0x0000	| 11 - NEW
	.short	0x0000, 0x0000, 0x01F4	| 12 - NEW
	.short	0x0000, 0x01F4, 0x01F4	| 13 - NEW
	.short	0x01F4, 0x01F4, 0x0000	| 14 - NEW
	.short	0x01F4, 0x0000, 0x01F4	| 15 - NEW
	.short	0x03E8, 0x03E8, 0x03E8
	.short	0x03A5, 0x03A5, 0x03A5
	.short	0x0363, 0x0363, 0x0363
	.short	0x0320, 0x0320, 0x0320
	.short	0x02DD, 0x02DD, 0x02DD
	.short	0x029B, 0x029B, 0x029B
	.short	0x0258, 0x0258, 0x0258
	.short	0x0215, 0x0215, 0x0215
	.short	0x01D3, 0x01D3, 0x01D3
	.short	0x0190, 0x0190, 0x0190
	.short	0x014D, 0x014D, 0x014D
	.short	0x010B, 0x010B, 0x010B
	.short	0x00C8, 0x00C8, 0x00C8
	.short	0x0085, 0x0085, 0x0085
	.short	0x0043, 0x0043, 0x0043
	.short	0x0000, 0x0000, 0x0000
	.short	0x03E8, 0x0000, 0x0000
	.short	0x03E8, 0x0000, 0x0043
	.short	0x03E8, 0x0000, 0x0085
	.short	0x03E8, 0x0000, 0x00C8
	.short	0x03E8, 0x0000, 0x010B
	.short	0x03E8, 0x0000, 0x014D
	.short	0x03E8, 0x0000, 0x0190
	.short	0x03E8, 0x0000, 0x01D3
	.short	0x03E8, 0x0000, 0x0215
	.short	0x03E8, 0x0000, 0x0258
	.short	0x03E8, 0x0000, 0x029B
	.short	0x03E8, 0x0000, 0x02DD
	.short	0x03E8, 0x0000, 0x0320
	.short	0x03E8, 0x0000, 0x0363
	.short	0x03E8, 0x0000, 0x03A5
	.short	0x03E8, 0x0000, 0x03E8
	.short	0x03A5, 0x0000, 0x03E8
	.short	0x0363, 0x0000, 0x03E8
	.short	0x0320, 0x0000, 0x03E8
	.short	0x02DD, 0x0000, 0x03E8
	.short	0x029B, 0x0000, 0x03E8
	.short	0x0258, 0x0000, 0x03E8
	.short	0x0215, 0x0000, 0x03E8
	.short	0x01D3, 0x0000, 0x03E8
	.short	0x0190, 0x0000, 0x03E8
	.short	0x014D, 0x0000, 0x03E8
	.short	0x010B, 0x0000, 0x03E8
	.short	0x00C8, 0x0000, 0x03E8
	.short	0x0085, 0x0000, 0x03E8
	.short	0x0043, 0x0000, 0x03E8
	.short	0x0000, 0x0000, 0x03E8
	.short	0x0000, 0x0043, 0x03E8
	.short	0x0000, 0x0085, 0x03E8
	.short	0x0000, 0x00C8, 0x03E8
	.short	0x0000, 0x010B, 0x03E8
	.short	0x0000, 0x014D, 0x03E8
	.short	0x0000, 0x0190, 0x03E8
	.short	0x0000, 0x01D3, 0x03E8
	.short	0x0000, 0x0215, 0x03E8
	.short	0x0000, 0x0258, 0x03E8
	.short	0x0000, 0x029B, 0x03E8
	.short	0x0000, 0x02DD, 0x03E8
	.short	0x0000, 0x0320, 0x03E8
	.short	0x0000, 0x0363, 0x03E8
	.short	0x0000, 0x03A5, 0x03E8
	.short	0x0000, 0x03E8, 0x03E8
	.short	0x0000, 0x03E8, 0x03A5
	.short	0x0000, 0x03E8, 0x0363
	.short	0x0000, 0x03E8, 0x0320
	.short	0x0000, 0x03E8, 0x02DD
	.short	0x0000, 0x03E8, 0x029B
	.short	0x0000, 0x03E8, 0x0258
	.short	0x0000, 0x03E8, 0x0215
	.short	0x0000, 0x03E8, 0x01D3
	.short	0x0000, 0x03E8, 0x0190
	.short	0x0000, 0x03E8, 0x014D
	.short	0x0000, 0x03E8, 0x010B
	.short	0x0000, 0x03E8, 0x00C8
	.short	0x0000, 0x03E8, 0x0085
	.short	0x0000, 0x03E8, 0x0043
	.short	0x0000, 0x03E8, 0x0000
	.short	0x0043, 0x03E8, 0x0000
	.short	0x0085, 0x03E8, 0x0000
	.short	0x00C8, 0x03E8, 0x0000
	.short	0x010B, 0x03E8, 0x0000
	.short	0x014D, 0x03E8, 0x0000
	.short	0x0190, 0x03E8, 0x0000
	.short	0x01D3, 0x03E8, 0x0000
	.short	0x0215, 0x03E8, 0x0000
	.short	0x0258, 0x03E8, 0x0000
	.short	0x029B, 0x03E8, 0x0000
	.short	0x02DD, 0x03E8, 0x0000
	.short	0x0320, 0x03E8, 0x0000
	.short	0x0363, 0x03E8, 0x0000
	.short	0x03A5, 0x03E8, 0x0000
	.short	0x03E8, 0x03E8, 0x0000
	.short	0x03E8, 0x03A5, 0x0000
	.short	0x03E8, 0x0363, 0x0000
	.short	0x03E8, 0x0320, 0x0000
	.short	0x03E8, 0x02DD, 0x0000
	.short	0x03E8, 0x029B, 0x0000
	.short	0x03E8, 0x0258, 0x0000
	.short	0x03E8, 0x0215, 0x0000
	.short	0x03E8, 0x01D3, 0x0000
	.short	0x03E8, 0x0190, 0x0000
	.short	0x03E8, 0x014D, 0x0000
	.short	0x03E8, 0x010B, 0x0000
	.short	0x03E8, 0x00C8, 0x0000
	.short	0x03E8, 0x0085, 0x0000
	.short	0x03E8, 0x0043, 0x0000
	.short	0x02DD, 0x0000, 0x0000
	.short	0x02DD, 0x0000, 0x0043
	.short	0x02DD, 0x0000, 0x0085
	.short	0x02DD, 0x0000, 0x00C8
	.short	0x02DD, 0x0000, 0x010B
	.short	0x02DD, 0x0000, 0x014D
	.short	0x02DD, 0x0000, 0x0190
	.short	0x02DD, 0x0000, 0x01D3
	.short	0x02DD, 0x0000, 0x0215
	.short	0x02DD, 0x0000, 0x0258
	.short	0x02DD, 0x0000, 0x029B
	.short	0x02DD, 0x0000, 0x02DD
	.short	0x029B, 0x0000, 0x02DD
	.short	0x0258, 0x0000, 0x02DD
	.short	0x0215, 0x0000, 0x02DD
	.short	0x01D3, 0x0000, 0x02DD
	.short	0x0190, 0x0000, 0x02DD
	.short	0x014D, 0x0000, 0x02DD
	.short	0x010B, 0x0000, 0x02DD
	.short	0x00C8, 0x0000, 0x02DD
	.short	0x0085, 0x0000, 0x02DD
	.short	0x0043, 0x0000, 0x02DD
	.short	0x0000, 0x0000, 0x02DD
	.short	0x0000, 0x0043, 0x02DD
	.short	0x0000, 0x0085, 0x02DD
	.short	0x0000, 0x00C8, 0x02DD
	.short	0x0000, 0x010B, 0x02DD
	.short	0x0000, 0x014D, 0x02DD
	.short	0x0000, 0x0190, 0x02DD
	.short	0x0000, 0x01D3, 0x02DD
	.short	0x0000, 0x0215, 0x02DD
	.short	0x0000, 0x0258, 0x02DD
	.short	0x0000, 0x029B, 0x02DD
	.short	0x0000, 0x02DD, 0x02DD
	.short	0x0000, 0x02DD, 0x029B
	.short	0x0000, 0x02DD, 0x0258
	.short	0x0000, 0x02DD, 0x0215
	.short	0x0000, 0x02DD, 0x01D3
	.short	0x0000, 0x02DD, 0x0190
	.short	0x0000, 0x02DD, 0x014D
	.short	0x0000, 0x02DD, 0x010B
	.short	0x0000, 0x02DD, 0x00C8
	.short	0x0000, 0x02DD, 0x0085
	.short	0x0000, 0x02DD, 0x0043
	.short	0x0000, 0x02DD, 0x0000
	.short	0x0043, 0x02DD, 0x0000
	.short	0x0085, 0x02DD, 0x0000
	.short	0x00C8, 0x02DD, 0x0000
	.short	0x010B, 0x02DD, 0x0000
	.short	0x014D, 0x02DD, 0x0000
	.short	0x0190, 0x02DD, 0x0000
	.short	0x01D3, 0x02DD, 0x0000
	.short	0x0215, 0x02DD, 0x0000
	.short	0x0258, 0x02DD, 0x0000
	.short	0x029B, 0x02DD, 0x0000
	.short	0x02DD, 0x02DD, 0x0000
	.short	0x02DD, 0x029B, 0x0000
	.short	0x02DD, 0x0258, 0x0000
	.short	0x02DD, 0x0215, 0x0000
	.short	0x02DD, 0x01D3, 0x0000
	.short	0x02DD, 0x0190, 0x0000
	.short	0x02DD, 0x014D, 0x0000
	.short	0x02DD, 0x010B, 0x0000
	.short	0x02DD, 0x00C8, 0x0000
	.short	0x02DD, 0x0085, 0x0000
	.short	0x02DD, 0x0043, 0x0000
	.short	0x01D3, 0x0000, 0x0000
	.short	0x01D3, 0x0000, 0x0043
	.short	0x01D3, 0x0000, 0x0085
	.short	0x01D3, 0x0000, 0x00C8
	.short	0x01D3, 0x0000, 0x010B
	.short	0x01D3, 0x0000, 0x014D
	.short	0x01D3, 0x0000, 0x0190
	.short	0x01D3, 0x0000, 0x01D3
	.short	0x0190, 0x0000, 0x01D3
	.short	0x014D, 0x0000, 0x01D3
	.short	0x010B, 0x0000, 0x01D3
	.short	0x00C8, 0x0000, 0x01D3
	.short	0x0085, 0x0000, 0x01D3
	.short	0x0043, 0x0000, 0x01D3
	.short	0x0000, 0x0000, 0x01D3
	.short	0x0000, 0x0043, 0x01D3
	.short	0x0000, 0x0085, 0x01D3
	.short	0x0000, 0x00C8, 0x01D3
	.short	0x0000, 0x010B, 0x01D3
	.short	0x0000, 0x014D, 0x01D3
	.short	0x0000, 0x0190, 0x01D3
	.short	0x0000, 0x01D3, 0x01D3
	.short	0x0000, 0x01D3, 0x0190
	.short	0x0000, 0x01D3, 0x014D
	.short	0x0000, 0x01D3, 0x010B
	.short	0x0000, 0x01D3, 0x00C8
	.short	0x0000, 0x01D3, 0x0085
	.short	0x0000, 0x01D3, 0x0043
	.short	0x0000, 0x01D3, 0x0000
	.short	0x0043, 0x01D3, 0x0000
	.short	0x0085, 0x01D3, 0x0000
	.short	0x00C8, 0x01D3, 0x0000
	.short	0x010B, 0x01D3, 0x0000
	.short	0x014D, 0x01D3, 0x0000
	.short	0x0190, 0x01D3, 0x0000
	.short	0x01D3, 0x01D3, 0x0000
	.short	0x01D3, 0x0190, 0x0000
	.short	0x01D3, 0x014D, 0x0000
	.short	0x01D3, 0x010B, 0x0000
	.short	0x01D3, 0x00C8, 0x0000
	.short	0x01D3, 0x0085, 0x0000
	.short	0x01D3, 0x0043, 0x0000
	.short	0x010B, 0x0000, 0x0000
	.short	0x010B, 0x0000, 0x0043
	.short	0x010B, 0x0000, 0x0085
	.short	0x010B, 0x0000, 0x00C8
	.short	0x010B, 0x0000, 0x010B
	.short	0x00C8, 0x0000, 0x010B
	.short	0x0085, 0x0000, 0x010B
	.short	0x0043, 0x0000, 0x010B
	.short	0x0000, 0x0000, 0x010B
	.short	0x0000, 0x0043, 0x010B
	.short	0x0000, 0x0085, 0x010B
	.short	0x0000, 0x00C8, 0x010B
	.short	0x0000, 0x010B, 0x010B
	.short	0x0000, 0x010B, 0x00C8
	.short	0x0000, 0x010B, 0x0085
	.short	0x0000, 0x010B, 0x0043
	.short	0x0000, 0x010B, 0x0000
	.short	0x0043, 0x010B, 0x0000
	.short	0x0085, 0x010B, 0x0000
	.short	0x00C8, 0x010B, 0x0000
	.short	0x010B, 0x010B, 0x0000
	.short	0x010B, 0x00C8, 0x0000
	.short	0x010B, 0x0085, 0x0000
	.short	0x010B, 0x0043, 0x0000
	.short	0x03E8, 0x03E8, 0x03E8
	.short	0x0000, 0x0000, 0x0000

_def_vals:					| system defaults
	.short	0x03E8, 0x03E8, 0x03E8	| Pen 0
	.short	0x0000, 0x0000, 0x0000	| Pen 1
	.short	0x03E8, 0x0000, 0x0000	| 2
	.short	0x0000, 0x03E8, 0x0000	| 3
	.short	0x0000, 0x0000, 0x03E8	| 4
	.short	0x0000, 0x03E8, 0x03E8	| 5
	.short	0x03E8, 0x03E8, 0x0000	| 6
	.short	0x03E8, 0x0000, 0x03E8	| 7
	.short	0x0320, 0x0320, 0x0320	| 8  - NEW
	.short	0x01F4, 0x01F4, 0x01F4	| 9  - NEW
	.short	0x01F4, 0x0000, 0x0000	| 10 - NEW
	.short	0x0000, 0x01F4, 0x0000	| 11 - NEW
	.short	0x0000, 0x0000, 0x01F4	| 12 - NEW
	.short	0x0000, 0x01F4, 0x01F4	| 13 - NEW
	.short	0x01F4, 0x01F4, 0x0000	| 14 - NEW
	.short	0x01F4, 0x0000, 0x01F4	| 15 - NEW
	.short	0x03E8, 0x03E8, 0x03E8
	.short	0x03A5, 0x03A5, 0x03A5
	.short	0x0363, 0x0363, 0x0363
	.short	0x0320, 0x0320, 0x0320
	.short	0x02DD, 0x02DD, 0x02DD
	.short	0x029B, 0x029B, 0x029B
	.short	0x0258, 0x0258, 0x0258
	.short	0x0215, 0x0215, 0x0215
	.short	0x01D3, 0x01D3, 0x01D3
	.short	0x0190, 0x0190, 0x0190
	.short	0x014D, 0x014D, 0x014D
	.short	0x010B, 0x010B, 0x010B
	.short	0x00C8, 0x00C8, 0x00C8
	.short	0x0085, 0x0085, 0x0085
	.short	0x0043, 0x0043, 0x0043
	.short	0x0000, 0x0000, 0x0000
	.short	0x03E8, 0x0000, 0x0000
	.short	0x03E8, 0x0000, 0x0043
	.short	0x03E8, 0x0000, 0x0085
	.short	0x03E8, 0x0000, 0x00C8
	.short	0x03E8, 0x0000, 0x010B
	.short	0x03E8, 0x0000, 0x014D
	.short	0x03E8, 0x0000, 0x0190
	.short	0x03E8, 0x0000, 0x01D3
	.short	0x03E8, 0x0000, 0x0215
	.short	0x03E8, 0x0000, 0x0258
	.short	0x03E8, 0x0000, 0x029B
	.short	0x03E8, 0x0000, 0x02DD
	.short	0x03E8, 0x0000, 0x0320
	.short	0x03E8, 0x0000, 0x0363
	.short	0x03E8, 0x0000, 0x03A5
	.short	0x03E8, 0x0000, 0x03E8
	.short	0x03A5, 0x0000, 0x03E8
	.short	0x0363, 0x0000, 0x03E8
	.short	0x0320, 0x0000, 0x03E8
	.short	0x02DD, 0x0000, 0x03E8
	.short	0x029B, 0x0000, 0x03E8
	.short	0x0258, 0x0000, 0x03E8
	.short	0x0215, 0x0000, 0x03E8
	.short	0x01D3, 0x0000, 0x03E8
	.short	0x0190, 0x0000, 0x03E8
	.short	0x014D, 0x0000, 0x03E8
	.short	0x010B, 0x0000, 0x03E8
	.short	0x00C8, 0x0000, 0x03E8
	.short	0x0085, 0x0000, 0x03E8
	.short	0x0043, 0x0000, 0x03E8
	.short	0x0000, 0x0000, 0x03E8
	.short	0x0000, 0x0043, 0x03E8
	.short	0x0000, 0x0085, 0x03E8
	.short	0x0000, 0x00C8, 0x03E8
	.short	0x0000, 0x010B, 0x03E8
	.short	0x0000, 0x014D, 0x03E8
	.short	0x0000, 0x0190, 0x03E8
	.short	0x0000, 0x01D3, 0x03E8
	.short	0x0000, 0x0215, 0x03E8
	.short	0x0000, 0x0258, 0x03E8
	.short	0x0000, 0x029B, 0x03E8
	.short	0x0000, 0x02DD, 0x03E8
	.short	0x0000, 0x0320, 0x03E8
	.short	0x0000, 0x0363, 0x03E8
	.short	0x0000, 0x03A5, 0x03E8
	.short	0x0000, 0x03E8, 0x03E8
	.short	0x0000, 0x03E8, 0x03A5
	.short	0x0000, 0x03E8, 0x0363
	.short	0x0000, 0x03E8, 0x0320
	.short	0x0000, 0x03E8, 0x02DD
	.short	0x0000, 0x03E8, 0x029B
	.short	0x0000, 0x03E8, 0x0258
	.short	0x0000, 0x03E8, 0x0215
	.short	0x0000, 0x03E8, 0x01D3
	.short	0x0000, 0x03E8, 0x0190
	.short	0x0000, 0x03E8, 0x014D
	.short	0x0000, 0x03E8, 0x010B
	.short	0x0000, 0x03E8, 0x00C8
	.short	0x0000, 0x03E8, 0x0085
	.short	0x0000, 0x03E8, 0x0043
	.short	0x0000, 0x03E8, 0x0000
	.short	0x0043, 0x03E8, 0x0000
	.short	0x0085, 0x03E8, 0x0000
	.short	0x00C8, 0x03E8, 0x0000
	.short	0x010B, 0x03E8, 0x0000
	.short	0x014D, 0x03E8, 0x0000
	.short	0x0190, 0x03E8, 0x0000
	.short	0x01D3, 0x03E8, 0x0000
	.short	0x0215, 0x03E8, 0x0000
	.short	0x0258, 0x03E8, 0x0000
	.short	0x029B, 0x03E8, 0x0000
	.short	0x02DD, 0x03E8, 0x0000
	.short	0x0320, 0x03E8, 0x0000
	.short	0x0363, 0x03E8, 0x0000
	.short	0x03A5, 0x03E8, 0x0000
	.short	0x03E8, 0x03E8, 0x0000
	.short	0x03E8, 0x03A5, 0x0000
	.short	0x03E8, 0x0363, 0x0000
	.short	0x03E8, 0x0320, 0x0000
	.short	0x03E8, 0x02DD, 0x0000
	.short	0x03E8, 0x029B, 0x0000
	.short	0x03E8, 0x0258, 0x0000
	.short	0x03E8, 0x0215, 0x0000
	.short	0x03E8, 0x01D3, 0x0000
	.short	0x03E8, 0x0190, 0x0000
	.short	0x03E8, 0x014D, 0x0000
	.short	0x03E8, 0x010B, 0x0000
	.short	0x03E8, 0x00C8, 0x0000
	.short	0x03E8, 0x0085, 0x0000
	.short	0x03E8, 0x0043, 0x0000
	.short	0x02DD, 0x0000, 0x0000
	.short	0x02DD, 0x0000, 0x0043
	.short	0x02DD, 0x0000, 0x0085
	.short	0x02DD, 0x0000, 0x00C8
	.short	0x02DD, 0x0000, 0x010B
	.short	0x02DD, 0x0000, 0x014D
	.short	0x02DD, 0x0000, 0x0190
	.short	0x02DD, 0x0000, 0x01D3
	.short	0x02DD, 0x0000, 0x0215
	.short	0x02DD, 0x0000, 0x0258
	.short	0x02DD, 0x0000, 0x029B
	.short	0x02DD, 0x0000, 0x02DD
	.short	0x029B, 0x0000, 0x02DD
	.short	0x0258, 0x0000, 0x02DD
	.short	0x0215, 0x0000, 0x02DD
	.short	0x01D3, 0x0000, 0x02DD
	.short	0x0190, 0x0000, 0x02DD
	.short	0x014D, 0x0000, 0x02DD
	.short	0x010B, 0x0000, 0x02DD
	.short	0x00C8, 0x0000, 0x02DD
	.short	0x0085, 0x0000, 0x02DD
	.short	0x0043, 0x0000, 0x02DD
	.short	0x0000, 0x0000, 0x02DD
	.short	0x0000, 0x0043, 0x02DD
	.short	0x0000, 0x0085, 0x02DD
	.short	0x0000, 0x00C8, 0x02DD
	.short	0x0000, 0x010B, 0x02DD
	.short	0x0000, 0x014D, 0x02DD
	.short	0x0000, 0x0190, 0x02DD
	.short	0x0000, 0x01D3, 0x02DD
	.short	0x0000, 0x0215, 0x02DD
	.short	0x0000, 0x0258, 0x02DD
	.short	0x0000, 0x029B, 0x02DD
	.short	0x0000, 0x02DD, 0x02DD
	.short	0x0000, 0x02DD, 0x029B
	.short	0x0000, 0x02DD, 0x0258
	.short	0x0000, 0x02DD, 0x0215
	.short	0x0000, 0x02DD, 0x01D3
	.short	0x0000, 0x02DD, 0x0190
	.short	0x0000, 0x02DD, 0x014D
	.short	0x0000, 0x02DD, 0x010B
	.short	0x0000, 0x02DD, 0x00C8
	.short	0x0000, 0x02DD, 0x0085
	.short	0x0000, 0x02DD, 0x0043
	.short	0x0000, 0x02DD, 0x0000
	.short	0x0043, 0x02DD, 0x0000
	.short	0x0085, 0x02DD, 0x0000
	.short	0x00C8, 0x02DD, 0x0000
	.short	0x010B, 0x02DD, 0x0000
	.short	0x014D, 0x02DD, 0x0000
	.short	0x0190, 0x02DD, 0x0000
	.short	0x01D3, 0x02DD, 0x0000
	.short	0x0215, 0x02DD, 0x0000
	.short	0x0258, 0x02DD, 0x0000
	.short	0x029B, 0x02DD, 0x0000
	.short	0x02DD, 0x02DD, 0x0000
	.short	0x02DD, 0x029B, 0x0000
	.short	0x02DD, 0x0258, 0x0000
	.short	0x02DD, 0x0215, 0x0000
	.short	0x02DD, 0x01D3, 0x0000
	.short	0x02DD, 0x0190, 0x0000
	.short	0x02DD, 0x014D, 0x0000
	.short	0x02DD, 0x010B, 0x0000
	.short	0x02DD, 0x00C8, 0x0000
	.short	0x02DD, 0x0085, 0x0000
	.short	0x02DD, 0x0043, 0x0000
	.short	0x01D3, 0x0000, 0x0000
	.short	0x01D3, 0x0000, 0x0043
	.short	0x01D3, 0x0000, 0x0085
	.short	0x01D3, 0x0000, 0x00C8
	.short	0x01D3, 0x0000, 0x010B
	.short	0x01D3, 0x0000, 0x014D
	.short	0x01D3, 0x0000, 0x0190
	.short	0x01D3, 0x0000, 0x01D3
	.short	0x0190, 0x0000, 0x01D3
	.short	0x014D, 0x0000, 0x01D3
	.short	0x010B, 0x0000, 0x01D3
	.short	0x00C8, 0x0000, 0x01D3
	.short	0x0085, 0x0000, 0x01D3
	.short	0x0043, 0x0000, 0x01D3
	.short	0x0000, 0x0000, 0x01D3
	.short	0x0000, 0x0043, 0x01D3
	.short	0x0000, 0x0085, 0x01D3
	.short	0x0000, 0x00C8, 0x01D3
	.short	0x0000, 0x010B, 0x01D3
	.short	0x0000, 0x014D, 0x01D3
	.short	0x0000, 0x0190, 0x01D3
	.short	0x0000, 0x01D3, 0x01D3
	.short	0x0000, 0x01D3, 0x0190
	.short	0x0000, 0x01D3, 0x014D
	.short	0x0000, 0x01D3, 0x010B
	.short	0x0000, 0x01D3, 0x00C8
	.short	0x0000, 0x01D3, 0x0085
	.short	0x0000, 0x01D3, 0x0043
	.short	0x0000, 0x01D3, 0x0000
	.short	0x0043, 0x01D3, 0x0000
	.short	0x0085, 0x01D3, 0x0000
	.short	0x00C8, 0x01D3, 0x0000
	.short	0x010B, 0x01D3, 0x0000
	.short	0x014D, 0x01D3, 0x0000
	.short	0x0190, 0x01D3, 0x0000
	.short	0x01D3, 0x01D3, 0x0000
	.short	0x01D3, 0x0190, 0x0000
	.short	0x01D3, 0x014D, 0x0000
	.short	0x01D3, 0x010B, 0x0000
	.short	0x01D3, 0x00C8, 0x0000
	.short	0x01D3, 0x0085, 0x0000
	.short	0x01D3, 0x0043, 0x0000
	.short	0x010B, 0x0000, 0x0000
	.short	0x010B, 0x0000, 0x0043
	.short	0x010B, 0x0000, 0x0085
	.short	0x010B, 0x0000, 0x00C8
	.short	0x010B, 0x0000, 0x010B
	.short	0x00C8, 0x0000, 0x010B
	.short	0x0085, 0x0000, 0x010B
	.short	0x0043, 0x0000, 0x010B
	.short	0x0000, 0x0000, 0x010B
	.short	0x0000, 0x0043, 0x010B
	.short	0x0000, 0x0085, 0x010B
	.short	0x0000, 0x00C8, 0x010B
	.short	0x0000, 0x010B, 0x010B
	.short	0x0000, 0x010B, 0x00C8
	.short	0x0000, 0x010B, 0x0085
	.short	0x0000, 0x010B, 0x0043
	.short	0x0000, 0x010B, 0x0000
	.short	0x0043, 0x010B, 0x0000
	.short	0x0085, 0x010B, 0x0000
	.short	0x00C8, 0x010B, 0x0000
	.short	0x010B, 0x010B, 0x0000
	.short	0x010B, 0x00C8, 0x0000
	.short	0x010B, 0x0085, 0x0000
	.short	0x010B, 0x0043, 0x0000
	.short	0x03E8, 0x03E8, 0x03E8
	.short	0x0000, 0x0000, 0x0000

	.end
