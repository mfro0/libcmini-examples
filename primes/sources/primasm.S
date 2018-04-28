			.globl		_primes_asm


_primes_asm:
#ifdef __mcoldfire__
			lea			-3*4(sp),sp
			movem.l		d3-d5,(sp)
#else
			movem.l		d3-d5,-(sp)			// save register (3 * 4)
#endif /* __mcoldfire__ */
			
			move.l		4 * 4(sp),d5		// d5 = parameter = product to test
			move.l		#2,d4				// d4 = divider

loop:		cmp.l		d4,d5				// < product?
			ble			finished

			move.l		d5,d3
			divu.w		d4,d3				// d3 = d3 / d4, remainder in upper word of d3

			swap.w		d3
			tst.w		d3					// remainder?
			bne			next				// yes, not a factor

// print factor found

			move.l		d4,-(sp)			// push found divider
			pea			format				// push format string
			bsr			_printf
			lea			2 * 4(sp),sp		// fix stack

			swap.w		d3
			move.l		d3,-(sp)
			bsr			_primes_asm
addq.l		#4,sp
			bra			end

next:
			addq.l		#1,d4
			bra			loop				// yes

finished:		
			move.l		d5,-(sp)			// push rest
			pea			last_format			// push format
			bsr			_printf
			lea			2 * 4(sp),sp		// fix stack
end:
#ifdef __mcoldfire__
			movem.l		(sp),d3-d5
			lea			3*4(sp),sp
#else
			movem.l		(sp)+,d3-d5
#endif /* __mcoldfire__ */
			rts
			
			.data
format:		.asciz		" %d *"
last_format:
			.ascii		" %d"
			.dc.b		13,10,0
