/*
 * GEM resource C output of prgflags
 *
 * created by ORCS 2.18
 */

#ifndef _LONG_PTR
#  define _LONG_PTR LONG
#endif

#ifndef OS_NORMAL
#  define OS_NORMAL 0x0000
#endif
#ifndef OS_SELECTED
#  define OS_SELECTED 0x0001
#endif
#ifndef OS_CROSSED
#  define OS_CROSSED 0x0002
#endif
#ifndef OS_CHECKED
#  define OS_CHECKED 0x0004
#endif
#ifndef OS_DISABLED
#  define OS_DISABLED 0x0008
#endif
#ifndef OS_OUTLINED
#  define OS_OUTLINED 0x0010
#endif
#ifndef OS_SHADOWED
#  define OS_SHADOWED 0x0020
#endif
#ifndef OS_WHITEBAK
#  define OS_WHITEBAK 0x0040
#endif
#ifndef OS_DRAW3D
#  define OS_DRAW3D 0x0080
#endif

#ifndef OF_NONE
#  define OF_NONE 0x0000
#endif
#ifndef OF_SELECTABLE
#  define OF_SELECTABLE 0x0001
#endif
#ifndef OF_DEFAULT
#  define OF_DEFAULT 0x0002
#endif
#ifndef OF_EXIT
#  define OF_EXIT 0x0004
#endif
#ifndef OF_EDITABLE
#  define OF_EDITABLE 0x0008
#endif
#ifndef OF_RBUTTON
#  define OF_RBUTTON 0x0010
#endif
#ifndef OF_LASTOB
#  define OF_LASTOB 0x0020
#endif
#ifndef OF_TOUCHEXIT
#  define OF_TOUCHEXIT 0x0040
#endif
#ifndef OF_HIDETREE
#  define OF_HIDETREE 0x0080
#endif
#ifndef OF_INDIRECT
#  define OF_INDIRECT 0x0100
#endif
#ifndef OF_FL3DIND
#  define OF_FL3DIND 0x0200
#endif
#ifndef OF_FL3DBAK
#  define OF_FL3DBAK 0x0400
#endif
#ifndef OF_FL3DACT
#  define OF_FL3DACT 0x0600
#endif
#ifndef OF_MOVEABLE
#  define OF_MOVEABLE 0x0800
#endif
#ifndef OF_POPUP
#  define OF_POPUP 0x1000
#endif

#ifndef G_SWBUTTON
#  define G_SWBUTTON 34
#endif
#ifndef G_POPUP
#  define G_POPUP 35
#endif
#ifndef G_EDIT
#  define G_EDIT 37
#endif
#ifndef G_SHORTCUT
#  define G_SHORTCUT 38
#endif
#ifndef G_SLIST
#  define G_SLIST 39
#endif
#ifndef G_EXTBOX
#  define G_EXTBOX 40
#endif
#ifndef G_OBLINK
#  define G_OBLINK 41
#endif

#ifndef WHITEBAK
#  define WHITEBAK OS_WHITEBAK
#endif
#ifndef DRAW3D
#  define DRAW3D OS_DRAW3D
#endif
#ifndef FL3DIND
#  define FL3DIND OF_FL3DIND
#endif
#ifndef FL3DBAK
#  define FL3DBAK OF_FL3DBAK
#endif
#ifndef FL3DACT
#  define FL3DACT OF_FL3DACT
#endif

#ifndef C_UNION
#ifdef __PORTAES_H__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __GEMLIB__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __PUREC__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __ALCYON__
#  define C_UNION(x) x
#endif
#endif
#ifndef C_UNION
#  define C_UNION(x) (_LONG_PTR)(x)
#endif

#define T0OBJ 0
#define FREEBB 1
#define FREEIMG 1
#define FREESTR 15

BYTE *rs_strings[] = {
	(BYTE *)"OK",
	(BYTE *)"Cancel",
	(BYTE *)"Save",
	(BYTE *)"Load program in Alt-RAM",
	(BYTE *)"Allocate memory in Alt-RAM",
	(BYTE *)"Fastload",
	(BYTE *)"File: ________.___",
	(BYTE *)"",
	(BYTE *)"",
	(BYTE *)"Public domain software by Roger Burrows",
	(BYTE *)"",
	(BYTE *)"",
	(BYTE *)"Set program flags",
	(BYTE *)"",
	(BYTE *)""
};

static WORD IMAG0[] = {
0x0000, 0x0000, 0x0000, 0x0000, 0x0018, 0x0004, 0x003C, 0x000E, 
0x0027, 0x000B, 0x0063, 0x8019, 0x80C1, 0xE030, 0xC080, 0x7060, 
0xE0C0, 0x30F0, 0xE060, 0x60D8, 0xB133, 0x408C, 0x1B1E, 0xC006, 
0x0E0C, 0x8003, 0x0E00, 0x0000, 0x0E00, 0x0000, 0x1B00, 0x0000, 
0x3100, 0x0080, 0x6000, 0x00C0, 0xC000, 0x0060, 0x8000, 0x0030, 
0x8001, 0x0010, 0x0003, 0x0018, 0x0006, 0x000C, 0x0006, 0x000C};

LONG rs_frstr[] = {
	0
};

BITBLK rs_bitblk[] = {
	{ (WORD *)0L, 4, 24, 0, 0, 1 }
};

LONG rs_frimg[] = {
	0L
};

ICONBLK rs_iconblk[] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

TEDINFO rs_tedinfo[] = {
	{ (BYTE *)6L, (BYTE *)7L, (BYTE *)8L, 3, 6, 0, 0x1100, 0x0, 0, 19,1 },
	{ (BYTE *)9L, (BYTE *)10L, (BYTE *)11L, 5, 0, 2, 0x1181, 0x0, 0, 40,1 },
	{ (BYTE *)12L, (BYTE *)13L, (BYTE *)14L, 3, 0, 2, 0x1181, 0x0, 0, 18,1 }
};

OBJECT rs_object[] = {
	{ -1, 1, 10, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 0x0000,0x0000, 0x0020,0x000b },
	{ 4, 2, 3, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1181L), 0x000a,0x0808, 0x0016,0x0802 },
	{ 3, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(0x0L), 0x0401,0x0d00, 0x0009,0x0101 },
	{ 1, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(0x1L), 0x040c,0x0d00, 0x0008,0x0101 },
	{ 6, 5, 5, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1181L), 0x0000,0x0808, 0x000a,0x0802 },
	{ 4, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(0x2L), 0x0401,0x0d00, 0x0007,0x0001 },
	{ 7, -1, -1, G_BUTTON, OF_SELECTABLE, OS_NORMAL, C_UNION(0x3L), 0x0001,0x0007, 0x001e,0x0001 },
	{ 8, -1, -1, G_BUTTON, OF_SELECTABLE, OS_NORMAL, C_UNION(0x4L), 0x0001,0x0006, 0x001e,0x0001 },
	{ 9, -1, -1, G_BUTTON, OF_SELECTABLE, OS_NORMAL, C_UNION(0x5L), 0x0001,0x0005, 0x001e,0x0001 },
	{ 10, -1, -1, G_BOXTEXT, OF_TOUCHEXIT, OS_NORMAL, C_UNION(0x0L), 0x0007,0x0003, 0x0012,0x0001 },
	{ 0, 11, 12, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x11100L), 0x0000,0x0000, 0x0020,0x0402 },
	{ 12, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(0x1L), 0x0000,0x0101, 0x0020,0x0301 },
	{ 10, -1, -1, G_TEXT, OF_LASTOB, OS_NORMAL, C_UNION(0x2L), 0x0406,0x0100, 0x0012,0x0001 }
};

_LONG_PTR rs_trindex[] = {
	0L
};

#ifndef __foobar_defined
#define __foobar_defined 1
struct foobar {
	WORD 	dummy;
	WORD 	*image;
};
#endif
struct foobar rs_imdope[] = {
	{ 0, &IMAG0[0] }
};



#define NUM_STRINGS 15
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 1
#define NUM_BB 1
#define NUM_FRIMG 1
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 3
#define NUM_OBS 13
#define NUM_TREE 1

BYTE pname[] = "PRGFLAGS.RSC";
