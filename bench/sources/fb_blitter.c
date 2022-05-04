//org 	0xe8f900

    mvz.w	#0x100,d4			// rest von orginal routine
    lea	0xffff8007,a2		// sys register
    btst.b	#3,(a2)				// blitter vorhanden?
    beq	alte_routine		// nein alte routine
    cmpa.l	#0x40000000,a4		// src > screen adr?
    blt	alte_routine		// ja alte routine
    tst.l	a4					// src < screen adr?
    bmi	alte_routine		// ja alte routine
    cmpa.l	#0x40000000,a5		// dst > screen adr?
    blt	alte_routine		// ja alte routine
    tst.l	a5					// dst < screen adr?
    bmi	alte_routine		// ja alte routine
    lsr.l	#6,d6				// /64
    bne	blitter_routine		// wenn mehr als 64byt neue routine
alte_routinD6:
    move.l	d5,d6			// register restaurieren
alte_routine:
    jmp	0xfdc3f4			// in alter routine weitermachen
blitter_routine:
    cmpa.l	a5,a4
    bmi	alte_routinD6
normal:	lea	0xffff8a00,a2		// blitter register
    move.l	#0x20000,d2		// xinc = 2, yinc = 0
    move.l	d2,0x20(a2)		// src inc setzen
    move.l	d2,0x2e(a2)		// dst inc setzen
    moveq	#-1,d2			// endmasken auf 0xffff
    move.l	d2,0x28(a2)
    move.w	d2,0x2c(a2)
    move.l	a4,0x24(a2)		// src adr setzen
    move.l	a5,0x32(a2)		// dst adr setzen
    move.w	#0x0203,0x3a(a2)		// modus src->dst
    lsr.l	#1,d5			// /2 = anzahl worte
    lsr.l	#8,d6			// /256 = 2/256*64= 8192 = 0x2000 worte pro 1
    beq	nur_eine_zeile
    move.w	#0x2000,0x36(a2)		// zeilenlänge auf 0x2000
    move.w	d6,0x38(a2)		// anzahl zeilen
    bra	transfer
nur_eine_zeile:
    move.w	d5,0x36(a2)		// anzahl worte
    move.w	#1,0x38(a2)		// eine zeile
transfer:
    nop
    move.w	#0x8000,0x3c(a2)		// start blitter
// cache push über trap #10
    move.l	#0xe8fa80,d2		// sprungvector cache push
    move.l	d2,0x1fe0008a		// vector setzen (vbr+8a)
    trap	#10			// ausführen
wait_blitter1:
    nop								// sycn pipline
    tst.w	0x3c(a2)			// test fertig
    bmi	wait_blitter1
    tst.w	d6			// reste zu übertragen?
    beq	fertig 			// nein ->
    andi.l	#0x1fff,d5		// anzahl restwords
    beq	fertig			// wenn 0 dann fertig
    move.w	d5,0x36(a2)		// sonst restwordanzahl setzen
    move.w	#1,0x38(a2)		// eine zeile
    nop
    move.w	#0x8000,0x3c(a2)		// blitter starten
wait_blitter2:
    nop
    tst.w	0x3c(a2)			// fertig?
    bmi	wait_blitter2		// nein -> warten
fertig:
    movem.l	(sp),d2-d6/a2-a5		// register zurück
    lea	0x24(sp),sp		// stack korrigieren
    rts				// zurück ins FireTOS..........
