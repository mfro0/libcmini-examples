|***************************************************************************
|
| reduced NKCC for the CF-Lib
| GNU-C compatible version
|
|***************************************************************************

|***************************************************************************
| ASSEMBLER CONTROL SECTION
|***************************************************************************

VERSION		=	0x0294		| NKCC's version

NKFf_FUNC	=	0x8000
NKFf_RESVD	=	0x4000
NKFf_NUM	=	0x2000
NKFf_CAPS	=	0x1000
NKFf_ALT	=	0x0800
NKFf_CTRL	=	0x0400
NKFf_SHIFT	=	0x0300		| both shift keys

NKFb_FUNC	=	15		| function
NKFb_RESVD	=	14		| reserved, ignore it!
NKFb_NUM	=	13		| numeric pad
NKFb_CAPS	=	12		| CapsLock
NKFb_ALT	=	11		| Alternate
NKFb_CTRL	=	10		| Control
NKFb_LSH	=	9		| left Shift key
NKFb_RSH	=	8		| right Shift key

NK_UP		=	0x01		| cursor up
NK_DOWN 	=	0x02		| cursor down
NK_RIGHT	=	0x03		| cursor right
NK_LEFT 	=	0x04		| cursor left
NK_M_PGUP	=	0x05		| Mac: page up
NK_M_PGDOWN	=	0x06		| Mac: page down
NK_M_END	=	0x07		| Mac: end
NK_INS		=	0x0b		| Insert
NK_CLRHOME	=	0x0c		| Clr/Home
NK_HELP 	=	0x0e		| Help
NK_UNDO 	=	0x0f		| Undo
NK_M_F11	=	0x1a		| Mac: function key #11
NK_M_F12	=	0x1c		| Mac: function key #12
NK_M_F14	=	0x1d		| Mac: function key #14
NK_ENTER	=	0x0a		| Enter
NK_DEL		=	0x1f		| Delete

|***************************************************************************
| EXPORT
|***************************************************************************

	.globl	_nkc_init		| init NKCC
	.globl	nkc_init
	.globl	_nkc_tos2n		| TOS key code converter
	.globl	nkc_tos2n
	.globl	_nkc_n2tos		| NKC to TOS key code converter
	.globl	nkc_n2tos
	.globl	_nkc_toupper		| convert character to upper case
	.globl	nkc_toupper
	.globl	_nkc_tolower		| convert character to lower case
	.globl	nkc_tolower

|***************************************************************************
| LOCAL TEXT SECTION
|***************************************************************************

|***************************************************************************
|
| nk_findscan: find scan code
|
|***************************************************************************

nk_findscan:
	btst	#NKFb_NUM,d0		| on numeric keypad?
	beqs	search			| no ->

	moveql	#0x4a,d1		| yes: try all numeric keypad
#ifdef __mcoldfire__
	cmpb	a0@(d1:l),d0		|	scan codes first
#else
	cmpb	a0@(d1:w),d0		|	scan codes first
#endif
	beqs	found1			| it matches ->

	moveql	#0x4e,d1
#ifdef __mcoldfire__
	cmpb	a0@(d1:l),d0
#else
	cmpb	a0@(d1:w),d0
#endif
	beqs	found1

	moveql	#0x63,d1		| block starts at 0x63

numsearch:
#ifdef __mcoldfire__
	cmpb	a0@(d1:l),d0		| match?
#else
	cmpb	a0@(d1:w),d0		| match?
#endif
	beqs	found1			| yes ->

#ifdef __mcoldfire__
	addql	#1,d1			| next scan code
#else
	addqw	#1,d1			| next scan code
#endif
	cmpw	#0x73,d1		| block end at 0x72
	bcss	numsearch		| continue search ->

search:
	moveql	#1,d1			| start with first valid scan code

mainsearch:
#ifdef __mcoldfire__
	cmpb	a0@(d1:l),d0		| match?
#else
	cmpb	a0@(d1:w),d0		| match?
#endif
	beqs	found1			| yes ->

#ifdef __mcoldfire__
	addql	#1,d1			| next scan code
#else
	addqb	#1,d1			| next scan code
#endif
	cmpb	#0x78,d1		| 0x78 = last valid scan code
	bcss	mainsearch		| continue search ->

	moveql	#0,d1			| not found
	rts

found1:
	tstw	d1			| found set CCR
	rts

|***************************************************************************
| GLOBAL TEXT SECTION
|***************************************************************************

|***************************************************************************
|
| nkc_init: initialize NKCC
|
|***************************************************************************

_nkc_init:
nkc_init:
|------------- fetch addresses of TOS' key scan code translation tables

	movel	a2,sp@-			| backup registers
	movel	d2,sp@-
	moveql	#-1,d0			| the function is also used to
	movel	d0,sp@- 		| change the addresses| values
	movel	d0,sp@- 		| of -1 as new addresses tell
	movel	d0,sp@- 		| XBIOS not to change them
	movew	#0x10,sp@-		| Keytbl
	trap	#14			| XBIOS
	lea	sp@(0xe),sp		| clean stack

	movel	d0,a0			| ^key table structure
	movel	a0@+,pkey_unshift	| get ^unshifted table
	movel	a0@+,pkey_shift 	| get ^shifted table
	movel	a0@,pkey_caps		| get ^CapsLock table

	movew	#VERSION,d0		| load version #
	movel	sp@+,d2			| restore registers
	movel	sp@+,a2
	rts				| bye

|***************************************************************************
|
| nkc_tconv: TOS key code converter
|
|***************************************************************************

_nkc_tos2n:
nkc_tos2n:
	movel	sp@(4), d0		| Parameter via Stack!
#ifdef __mcoldfire__
	lea	sp@(-12),sp
	movem.l	d2-d4,sp@		| store registers
#else
	movem.l	d2-d4,sp@- 		| store registers
#endif

|------------- separate TOS key code

	movel	d0,d1			| TOS key code
	swap	d1			| .W = scan code and flags
	movew	d1,d2			| copy
#ifdef __mcoldfire__
	movel	#0xff,d3		| and-mask
	andl	d3,d0			| .B = ASCII code
	andl	d3,d1			| .B = scan code
#else
	movew	#0xff,d3		| and-mask
	andw	d3,d0			| .B = ASCII code
	andw	d3,d1			| .B = scan code
#endif
	beq	tos306			| scancode=zero (key code created
						| by ASCII input of TOS 3.06)? ->
#ifdef __mcoldfire__
	andl	#0x1f00,d2		| .W = key flags (in high byte)
#else
	andw	#0x1f00,d2		| .W = key flags (in high byte)
#endif

|------------- decide which translation table to use

	movew	d2,d3			| key flags
#ifdef __mcoldfire__
	andl	#NKFf_SHIFT,d3		| isolate bits for shift keys
#else
	andw	#NKFf_SHIFT,d3		| isolate bits for shift keys
#endif
	beqs	ktab11			| shift key pressed? no->

	movel	pkey_shift,a0		| yes: use shift table
	bras	ktab13			| ->

ktab11:
	btst	#NKFb_CAPS,d2		| CapsLock?
	beqs	ktab12			| no->

	movel	pkey_caps,a0		| yes: use CapsLock table
	bras	ktab13			| ->

ktab12:
	movel	pkey_unshift,a0 	| use unshifted table

|------------- check if scan code is out of range
|
| Illegal scancodes can be used to produce 'macro key codes'. Their format is:
|
| - the scancode must be 0x84 or larger (should be 0xff to work properly with old
|   versions of Mag!x)
| - the ASCII code must be in the range 0x20...0xff (values below are set to 0x20
|   by NKCC)
| - Alternate and Control are not used for the normalized key code. However,
|   if at least one of them is non-zero, then the numeric keypad flag will be
|   set in the resulting key code.
|

ktab13:
	cmpb	#0x84,d1		| illegal scan code?
	bcss	ktab14			| no ->

	movew	d2,d1			| flags
|	andw	#NKFf_ALT|NKFf_CTRL,d1	| Alternate or Control?
#ifdef __mcoldfire__
	andl	#0xc00,d1		| Alternate or Control?
#else
	andw	#0xc00,d1		| Alternate or Control?
#endif
	beqs	special 		| no ->

#ifdef __mcoldfire__
	orl	#NKFf_NUM,d0		| yes: set numeric keypad flag
#else
	orw	#NKFf_NUM,d0		| yes: set numeric keypad flag
#endif
|	and	#NKFf_CAPS|NKFf_SHIFT,d2	| mask off both flags
#ifdef __mcoldfire__
	andl	#0x1300,d2		| mask off both flags
#else
	andw	#0x1300,d2		| mask off both flags
#endif

special:
#ifdef __mcoldfire__
	orl	d2,d0			| combine with ASCII code
#else
	orw	d2,d0			| combine with ASCII code
#endif
|	orw	#NKFf_FUNC|NKFf_RESVD,d0	| set function and resvd
#ifdef __mcoldfire__
	orl	#0x1300,d0		| set function and resvd
#else
	orw	#0x1300,d0		| set function and resvd
#endif
	cmpb	#0x20,d0		| ASCII code in range?
	bcc	exit2			| yes ->

	moveb	#0x20,d0		| no: use minimum
	bra	exit2			| ->

|------------- check if Alternate + number: they have simulated scan codes

ktab14:
	cmpb	#0x78,d1		| scan code of Alt + number?
	bcss	scan1			| no->

#ifdef __mcoldfire__
	subl	#0x76,d1		| yes: calculate REAL scan code
	moveb	a0@(d1:l),d0		| fetch ASCII code
	orl	#NKFf_ALT,d2		| set Alternate flag
#else
	subb	#0x76,d1		| yes: calculate REAL scan code
	moveb	a0@(d1:w),d0		| fetch ASCII code
	orw	#NKFf_ALT,d2		| set Alternate flag
#endif
	bra	cat_codes		| -> add flag byte and exit

|------------- check if exception scan code from cursor keypad

scan1:
	lea	xscantab,a1		| ^exception scan code table

search_scan:
	movew	a1@+,d3 		| NKC and scan code
	bmis	tabend			| <0? end of table reached ->

	cmpb	d1,d3			| scan code found?
	bnes	search_scan		| no: continue search ->

#ifdef __mcoldfire__
	lsrl	#8,d3			| .B = NKC
#else
	lsrw	#8,d3			| .B = NKC
#endif
	moveql	#0,d0			| mark: key code found
	bras	scan2			| ->

tabend:
	moveql	#0,d3			| no NKC found yet

|------------- check if rubbish ASCII code and erase it, if so

scan2:
#ifdef __mcoldfire__
	moveb	a0@(d1:l),d4		| ASCII code from translation table
#else
	moveb	a0@(d1:w),d4		| ASCII code from translation table
#endif
	cmpb	#32,d0			| ASCII returned by TOS < 32?
	bccs	scan3			| no -> can't be rubbish

	cmpb	d4,d0			| yes: compare with table entry
	beqs	scan3			| equal: that's ok ->

	moveql	#0,d0			| not equal: rubbish! clear it

|------------- check if ASCII code could only be produced via Alternate key
|		combination

scan3:
	tstb	d0			| ASCII code valid?
	beqs	scan4			| no ->

	cmpb	d4,d0			| compare with table entry
	beqs	scan4			| equal: normal key ->

|	and	#!NKFf_ALT,d2		| no: clear Alternate flag
#ifdef __mcoldfire__
	andl	#0xF7FF,d2		| no: clear Alternate flag
#else
	andw	#0xF7FF,d2		| no: clear Alternate flag
#endif

|------------- check if ASCII code found yet, and set it, if not

scan4:
	tstb	d0			| found?
	bnes	scan5			| yes ->

	moveb	d3,d0			| no: use code from exception table
	bnes	scan5			| now valid? yes ->

	moveb	d4,d0			| no: use code from transl. table

|------------- check special case: delete key

scan5:
	cmpb	#127,d0 		| ASCII code of Delete?
	bnes	scan6			| no ->

	moveb	#NK_DEL,d0		| yes: set according NKC

|------------- check if key is on numeric keypad (via scan code)

scan6:
	cmpb	#0x4a,d1		| numeric pad scan code range?
	beqs	numeric 		| yes ->

	cmpb	#0x4e,d1
	beqs	numeric 		| yes ->

	cmpb	#0x63,d1
	bcss	scan7			| no ->

	cmpb	#0x72,d1
	bhis	scan7			| no ->

numeric:
#ifdef __mcoldfire__
	orl	#NKFf_NUM,d2		| yes: set numeric bit
#else
	orw	#NKFf_NUM,d2		| yes: set numeric bit
#endif

|------------- check if "function key" and set bit accordingly

scan7:
	cmpb	#32,d0			| ASCII code less than 32?
	bccs	scan8			| no ->

#ifdef __mcoldfire__
	orl	#NKFf_FUNC,d2		| yes: set function bit
#else
	orw	#NKFf_FUNC,d2		| yes: set function bit
#endif

|------------- check special case: Return or Enter key

	cmpb	#13,d0			| Return or Enter key?
	bnes	scan8			| no ->

	btst	#NKFb_NUM,d2		| yes: from the numeric pad?
	beqs	scan8			| no -> it's Return, keep code

	moveql	#NK_ENTER,d0		| yes: it's Enter| new code

|------------- check if function key (F1-F10) via scan code

scan8:
	cmpb	#0x54,d1		| shift + function key?
	bcss	scan9			| no ->

	cmpb	#0x5d,d1
	bhis	scan9			| no ->

#ifdef __mcoldfire__
	subl	#0x54-0x3b,d1		| yes: scan code for unshifted key
#else
	subb	#0x54-0x3b,d1		| yes: scan code for unshifted key
#endif
	movew	d2,d3			| shift flags
#ifdef __mcoldfire__
	andl	#NKFf_SHIFT,d3		| any shift key flag set?
#else
	andw	#NKFf_SHIFT,d3		| any shift key flag set?
#endif
	bnes	scan9			| yes ->
#ifdef __mcoldfire__
	orl	#NKFf_SHIFT,d2		| no: set both flags
#else
	orw	#NKFf_SHIFT,d2		| no: set both flags
#endif

scan9:
	cmpb	#0x3b,d1		| (unshifted) function key?
	bcss	cat_codes		| no ->

	cmpb	#0x44,d1
	bhis	cat_codes		| no ->

	moveb	d1,d0			| yes: calc NKC
#ifdef __mcoldfire__
	subl	#0x2b,d0
#else
	subb	#0x2b,d0
#endif

|------------- final flag handling| mix key code (low byte) and flag byte

cat_codes:
	movel	pkey_shift,a0		| ^shifted table
#ifdef __mcoldfire__
	moveb	a0@(d1:l),d3		| get shifted ASCII code
	orl	d2,d0			| mix flags with key code
	tst.w	d0
#else
	moveb	a0@(d1:w),d3		| get shifted ASCII code
	orw	d2,d0			| mix flags with key code
#endif
	bmis	scan10			| result is "function key"? ->

#ifdef __mcoldfire__
	andl	#NKFf_CTRL+NKFf_ALT,d2	| Control or Alternate pressed?
#else
	andw	#NKFf_CTRL+NKFf_ALT,d2	| Control or Alternate pressed?
#endif
	bnes	scan11			| yes ->

scan10:
	movel	pkey_unshift,a0 	| ^unshifted table
#ifdef __mcoldfire__
	cmpb	a0@(d1:l),d3		| shifted ASCII = unshifted ASCII?
#else
	cmpb	a0@(d1:w),d3		| shifted ASCII = unshifted ASCII?
#endif
	beqs	scan12			| yes ->

	bras	exit2			| no ->

scan11:
#ifdef __mcoldfire__
	orl	#NKFf_FUNC,d0		| Alt/Ctrl + char: set function bit
#else
	orw	#NKFf_FUNC,d0		| Alt/Ctrl + char: set function bit
#endif
	movel	pkey_caps,a0		| ^CapsLock table
#ifdef __mcoldfire__
	cmpb	a0@(d1:l),d3		| shifted ASCII = CapsLocked ASCII?
#else
	cmpb	a0@(d1:w),d3		| shifted ASCII = CapsLocked ASCII?
#endif
	bnes	exit2			| no ->

	moveb	d3,d0			| yes: use shifted ASCII code

scan12:
#ifdef __mcoldfire__
	orl	#NKFf_RESVD,d0		| yes: nkc_cmp() has to check
#else
	orw	#NKFf_RESVD,d0		| yes: nkc_cmp() has to check
#endif

|------------- restore registers and exit

exit2:
	tstw	d0			| set CCR
#ifdef __mcoldfire__
	movem.l	sp@,d2-d4 		| restore registers
	lea	sp@(12),sp
#else
	movem.l	sp@+,d2-d4 		| restore registers
#endif
	rts				| bye

|------------- special handling for key codes created by TOS' 3.06 ASCII input

tos306:
#ifdef __mcoldfire__
	andl	#NKFf_CAPS,d2		| isolate CapsLock flag
	orl	d2,d0			| merge with ASCII code
	movem.l	sp@,d2-d4 		| restore registers
	lea	sp@(12),sp
#else
	andw	#NKFf_CAPS,d2		| isolate CapsLock flag
	orw	d2,d0			| merge with ASCII code
	movem.l	sp@+,d2-d4 		| restore registers
#endif
	rts				| bye

|***************************************************************************
|
| nkc_n2tos: convert normalized key codes back to TOS format
|
|***************************************************************************

_nkc_n2tos:  
nkc_n2tos:  
	movel	sp@(4),d0		| Parameter �ber Stack!

	movew	d0,d1			| normalized key code
|	and	#NKFf_FUNC|NKFf_ALT|NKFf_CTRL,d1| isolate flags
#ifdef __mcoldfire__
	andl	#0x8c00,d1		| isolate flags
#else
	andw	#0x8c00,d1		| isolate flags
#endif
	cmpw	#NKFf_FUNC,d1		| only function flag set?
	bnes	ktab20			| no ->

	cmpb	#0x20,d0		| ASCII code >= 0x20?
	bcss	ktab20			| no ->

|------------- macro key

	movew	d0,d1			| keep normalized key code
|	andl	#NKFf_CAPS|NKFf_SHIFT,d0	| isolate usable flags
	andl	#0x1300,d0		| mask off both flags
	btst	#NKFb_NUM,d1		| numeric keypad flag set?
	beqs	mackey			| no ->

|	or	#NKFf_ALT|NKFf_CTRL,d0	| yes: set Alternate + Control
#ifdef __mcoldfire__
	orl	#0xc00,d1		| yes: set Alternate + Control?
#else
	orw	#0xc00,d1		| yes: set Alternate + Control?
#endif

mackey:
#ifdef __mcoldfire__
	orl	#0xff,d0		| scan code always 0xff
#else
	orb	#0xff,d0		| scan code always 0xff
#endif
	swap	d0			| flags and scan code in upper word
	moveb	d1,d0			| ASCII code
	bra	exit3			| ->

|------------- select system key table to use

ktab20:
	movew	d0,d1			| normalized key code
#ifdef __mcoldfire__
	andl	#NKFf_SHIFT,d1		| isolate bits for shift keys
#else
	andw	#NKFf_SHIFT,d1		| isolate bits for shift keys
#endif
	beqs	ktab21			| shift key pressed? no->

	lea	n_to_scan_s,a1		| ^default translation table
	movel	pkey_shift,a0		| yes: use shift table
	bras	ktab23			| ->

ktab21:
	lea	n_to_scan_u,a1		| ^unshifted translation table
	btst	#NKFb_CAPS,d0		| CapsLock?
	beqs	ktab22			| no->

	movel	pkey_caps,a0		| yes: use CapsLock table
	bras	ktab23			| ->

ktab22:
	movel	pkey_unshift,a0 	| use unshifted table

|------------- handling for ASCII codes >= 32

ktab23:
	cmpb	#32,d0			| ASCII code < 32?
	bcss	lowascii		| yes ->

	bsr	nk_findscan		| find scan code
	bnes	found2			| found ->

	btst	#NKFb_FUNC,d0		| function flag set?
	beqs	notfound		| no ->

	movel	a0,d1			| save a0
	lea	tolower,a0		| ^upper->lower case table
	moveql	#0,d2			| clear for word operation
	moveb	d0,d2			| ASCII code
#ifdef __mcoldfire__
	moveb	a0@(d2:l),d0		| get lowercased ASCII code
#else
	moveb	a0@(d2:w),d0		| get lowercased ASCII code
#endif
	movel	d1,a0			| restore a0
	bsr	nk_findscan		| try to find scan code again
	bnes	found2			| found ->

|------------- unknown source: treat key code as it was entered using the
|		TOS 3.06 direct ASCII input

notfound:
	moveql	#0,d1			| not found: clear for word op.
	moveb	d0,d1			| unchanged ASCII code
#ifdef __mcoldfire__
	andl	#0x1f00,d0		| keep shift flags only
#else
	andw	#0x1f00,d0		| keep shift flags only
#endif
	swap	d0			| -> high word (scan code = 0)
	movew	d1,d0			| low word: ASCII code
	bra	exit3			| ->

|------------- handling for ASCII codes < 32

lowascii:
	btst	#NKFb_FUNC,d0		| function key?
	bnes	func			| yes ->

#ifdef __mcoldfire__
	andl	#0xffff10ff,d0		| clear all flags except CapsLock
#else
	andw	#0x10ff,d0		| clear all flags except CapsLock
#endif
	bras	notfound		| ->

func:
	moveql	#0,d1			| clear for word operation
	moveb	d0,d1			| ASCII code (0...0x1f)
	movew	d1,d2			| copy
#ifdef __mcoldfire__
	moveb	a1@(d1:l),d1		| get scan code
#else
	moveb	a1@(d1:w),d1		| get scan code
#endif
	bnes	getascii		| valid? ->

	moveq	#0,d0			| invalid key code!! return 0
	bra	exit3			| ->

getascii:
	lea	n_to_scan_u,a1		| ^unshifted translation table
#ifdef __mcoldfire__
	moveb	a1@(d2:l),d2		| get scan code from unshifted tab.
	moveb	a0@(d2:l),d0		| get ASCII from system's table
#else
	moveb	a1@(d2:w),d2		| get scan code from unshifted tab.
	moveb	a0@(d2:w),d0		| get ASCII from system's table
#endif

| register contents:
|
| d0.b		ASCII code
| d1.b		scan code
| d0.hb 	NKCC flags
|

found2:
	movew	d0,d2			| flags and ASCII code
#ifdef __mcoldfire__
	andl	#0x1f00,d0		| isolate shift flags
#else
	andw	#0x1f00,d0		| isolate shift flags
#endif
	moveb	d1,d0			| merge with scan code
	swap	d0			| -> high byte
	clrw	d0			| erase low word
	moveb	d2,d0			| restore ASCII code

|------------- handling for Control key flag

	btst	#NKFb_CTRL,d2		| control key flag set?
	beqs	alternate		| no ->

	cmpb	#0x4b,d1		| scan code = "cursor left"?
	bnes	scanchk2		| no ->

	addl	#0x280000,d0		| change scan code to 0x73
	clrb	d0			| erase ASCII code
	bras	exit3			| ->

scanchk2:
	cmpb	#0x4d,d1		| scan code = "cursor right"?
	bnes	scanchk3		| no ->

	addl	#0x270000,d0		| change scan code to 0x74
	clrb	d0			| erase ASCII code
	bras	exit3			| ->

scanchk3:
	cmpb	#0x47,d1		| scan code = "ClrHome"?
	bnes	ascchk			| no ->

	addl	#0x300000,d0		| change scan code to 0x77
| keep ASCII code in this case! What a mess...
	bras	exit3			| ->

ascchk:
	lea	asc_trans,a0		| ^ASCII translation table

ascloop:
	movew	a0@+,d1 		| get next entry
	beqs	noctrlasc		| end of table ->

	cmpb	d0,d1			| ASCII code found?
	bnes	ascloop 		| no -> continue search

#ifdef __mcoldfire__
	lsrl	#8,d1			| yes: get translated code
#else
	lsrw	#8,d1			| yes: get translated code
#endif
	moveb	d1,d0			| use it
	bras	exit3			| ->

noctrlasc:
#ifdef __mcoldfire__
	andl	#0x1f,d0		| mask off upper 3 bits
#else
	andb	#0x1f,d0		| mask off upper 3 bits
#endif
	bras	exit3			| ->

|------------- handling for Alternate key flag

alternate:
	btst	#NKFb_ALT,d2		| alternate key flag set?
	beqs	exit3			| no ->

	cmpb	#2,d1			| top row on main keyboard?
	bcss	alphachk		| no ->

	cmpb	#0xd,d1
	bhis	alphachk		| no ->

	addl	#0x760000,d0		| yes: change scan code
	clrb	d0			| and erase ASCII code
	bras	exit3			| ->

alphachk:
	cmpb	#65,d0			| alpha-characters?
	bcss	exit3			| no ->

	cmpb	#122,d0
	bhis	exit3			| no ->

	cmpb	#90,d0
	blss	ascii0			| yes ->

	cmpb	#97,d0
	bcss	exit3			| no ->

ascii0:
	clrb	d0			| alpha-character: clear ASCII code

exit3:
	tstw	d0			| set CCR
	rts				| bye


|***************************************************************************
|
| nkc_toupper: convert character to upper case
|
|***************************************************************************

_nkc_toupper:
nkc_toupper:
	movel	sp@(4),d1		| Parameter via Stack!
	lea	toupper,a0		| ^upper case translation table
#ifdef __mcoldfire__
	andl	#0xff,d1		| high byte = 0 for word operation
#else
	andw	#0xff,d1		| high byte = 0 for word operation
#endif
	moveb	a0@(d1:l),d0		| convert
	rts				| bye

|***************************************************************************
|
| nkc_tolower: convert character to lower case
|
|***************************************************************************

_nkc_tolower:
nkc_tolower:
	movel	sp@(4),d1		| Parameter via Stack!
	lea	tolower,a0		| ^lower case translation table
#ifdef __mcoldfire__
	andl	#0xff,d1		| high byte = 0 for word operation
#else
	andw	#0xff,d1		| high byte = 0 for word operation
#endif
	moveb	a0@(d1:l),d0		| convert
	rts				| bye

|***************************************************************************
| LOCAL DATA SECTION
|***************************************************************************

		.data

| exception scan code table for cursor block keys
|
| first entry.B:  NKCC key code
| second entry.B: scan code returned by TOS
|
| the table is terminated with both entries -1

xscantab:
	.byte	NK_UP		, 0x48	| cursor up
	.byte	NK_DOWN		, 0x50	| cursor down
	.byte	NK_LEFT		, 0x4b	| cursor left
	.byte	NK_LEFT		, 0x73	| Control cursor left
	.byte	NK_RIGHT	, 0x4d	| cursor right
	.byte	NK_RIGHT	, 0x74	| Control cursor right
	.byte	NK_M_PGUP	, 0x49	| Mac: page up
	.byte	NK_M_PGDOWN	, 0x51	| Mac: page down
	.byte	NK_M_END	, 0x4f	| Mac: end
	.byte	NK_INS		, 0x52	| Insert
	.byte	NK_CLRHOME	, 0x47	| ClrHome
	.byte	NK_CLRHOME	, 0x77	| Control ClrHome
	.byte	NK_HELP		, 0x62	| Help
	.byte	NK_UNDO		, 0x61	| Undo
	.byte	NK_M_F11	, 0x45	| Mac: F11
	.byte	NK_M_F12	, 0x46	| Mac: F12
	.byte	NK_M_F14	, 0x37	| Mac: F14
	.word	-1

| lower case to upper case conversion table
| (array of 256 unsigned bytes)
toupper:
	.byte 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07
	.byte 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
	.byte 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17
	.byte 0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f
	.byte 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27
	.byte 0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F
	.byte 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37
	.byte 0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F
	.byte 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47
	.byte 0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F
	.byte 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57
	.byte 0x58,0x59,0x5A,0x5B,0x5c,0x5D,0x5E,0x5F
	.byte 0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47
	.byte 0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F
	.byte 0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57
	.byte 0x58,0x59,0x5A,0x7B,0x7C,0x7D,0x7E,0x7F
	.byte 0x80,0x9A,0x90,0x83,0x8E,0xB6,0x8F,0x80
	.byte 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F
	.byte 0x90,0x92,0x92,0x93,0x99,0x95,0x96,0x97
	.byte 0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F
	.byte 0xA0,0xA1,0xA2,0xA3,0xA5,0xA5,0xA6,0xA7
	.byte 0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF
	.byte 0xB7,0xB8,0xB2,0xB2,0xB5,0xB5,0xB6,0xB7
	.byte 0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF
	.byte 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7
	.byte 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF
	.byte 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7
	.byte 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF
	.byte 0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7
	.byte 0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
	.byte 0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7
	.byte 0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF


| upper case to lower case conversion table
| (array of 256 unsigned bytes)
tolower:
	.byte 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07
	.byte 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
	.byte 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17
	.byte 0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f
	.byte 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27
	.byte 0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F
	.byte 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37
	.byte 0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F
	.byte 0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67
	.byte 0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F
	.byte 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77
	.byte 0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F
	.byte 0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67
	.byte 0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F
	.byte 0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77
	.byte 0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F
	.byte 0x87,0x81,0x82,0x83,0x84,0x85,0x86,0x87
	.byte 0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x84,0x86
	.byte 0x82,0x91,0x91,0x93,0x94,0x95,0x96,0x97
	.byte 0x98,0x94,0x81,0x9B,0x9C,0x9D,0x9E,0x9F
	.byte 0xA0,0xA1,0xA2,0xA3,0xA4,0xA4,0xA6,0xA7
	.byte 0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF
	.byte 0xB0,0xB1,0xB3,0xB3,0xB4,0xB4,0x85,0xB0
	.byte 0xB1,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF
	.byte 0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7
	.byte 0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF
	.byte 0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7
	.byte 0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF
	.byte 0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7
	.byte 0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
	.byte 0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7
	.byte 0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF


|  ASCII code translation table for Control key
|
|  first entry.B:  modified ASCII code returned by TOS
|  second entry.B: original ASCII code as stored in key table
|
|  The table is terminated with both entries 0

asc_trans:
	.byte	0,0x32			| Control '2' becomes ASCII 0
	.byte	0x1e,0x36		| Control '6' becomes ASCII 0x1e
	.byte	0x1f,0x2d		| Control '-' becomes ASCII 0x1f
	.byte	0xa,0xd 		| Control Return/Enter: 0xd -> 0xa
	.word	0			| terminator


|  normalized key code -> scan code translation table
|  for unshifted key codes
|  indexed by function code (NK_...)

n_to_scan_u:
	.byte	0x00			| invalid key code
	.byte	0x48			| cursor up
	.byte	0x50			| cursor down
	.byte	0x4d			| cursor right
	.byte	0x4b			| cursor left
	.byte	0x49			| Mac: page up
	.byte	0x51			| Mac: page down
	.byte	0x4f			| Mac: end
	.byte	0x0e			| Backspace
	.byte	0x0f			| Tab
	.byte	0x72			| Enter
	.byte	0x52			| Insert
	.byte	0x47			| ClrHome
	.byte	0x1c			| Return
	.byte	0x62			| Help
	.byte	0x61			| Undo
	.byte	0x3b			| function key #1
	.byte	0x3c			| function key #2
	.byte	0x3d			| function key #3
	.byte	0x3e			| function key #4
	.byte	0x3f			| function key #5
	.byte	0x40			| function key #6
	.byte	0x41			| function key #7
	.byte	0x42			| function key #8
	.byte	0x43			| function key #9
	.byte	0x44			| function key #10
	.byte	0x45			| Mac: F11
	.byte	0x01			| Esc
	.byte	0x46			| Mac: F12
	.byte	0x37			| Mac: F14
	.byte	0x00			| reserved!
	.byte	0x53			| Delete

|  normalized key code -> scan code translation table
|  for shifted key codes
|  indexed by function code (NK_...)

n_to_scan_s:
	.byte	0x00			| invalid key code
	.byte	0x48			| cursor up
	.byte	0x50			| cursor down
	.byte	0x4d			| cursor right
	.byte	0x4b			| cursor left
	.byte	0x49			| Mac: page up
	.byte	0x51			| Mac: page down
	.byte	0x4f			| Mac: end
	.byte	0x0e			| Backspace
	.byte	0x0f			| Tab
	.byte	0x72			| Enter
	.byte	0x52			| Insert
	.byte	0x47			| ClrHome
	.byte	0x1c			| Return
	.byte	0x62			| Help
	.byte	0x61			| Undo
	.byte	0x54			| function key #1
	.byte	0x55			| function key #2
	.byte	0x56			| function key #3
	.byte	0x57			| function key #4
	.byte	0x58			| function key #5
	.byte	0x59			| function key #6
	.byte	0x5a			| function key #7
	.byte	0x5b			| function key #8
	.byte	0x5c			| function key #9
	.byte	0x5d			| function key #10
	.byte	0x45			| Mac: F11
	.byte	0x01			| Esc
	.byte	0x46			| Mac: F12
	.byte	0x37			| Mac: F14
	.byte	0x00			| reserved!
	.byte	0x53			| Delete

|***************************************************************************
| LOCAL BSS SECTION
|***************************************************************************

|	.bss

pkey_unshift:
	.long	1			| ^unshifted key table

pkey_shift:
	.long	1			| ^shifted key table

pkey_caps:
	.long	1			| ^CapsLock table
