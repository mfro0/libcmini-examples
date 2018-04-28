#ifndef _TEXTWINDOW_H_
#define _TEXTWINDOW_H_

#include <gem.h>
#include <osbind.h>
#include "portab.h"

#define KIND_TEXTWINDOW		1
#define TEXTWINDOW_WINELEMENTS	SIZER|MOVER|FULLER|CLOSER|NAME|INFO|LFARROW|RTARROW|HSLIDE|VSLIDE|DNARROW|UPARROW

#define TEXTWINDOW_CLASS	0x74787477	/* 'txtw' */

extern struct window *create_textwindow(short wi_kind, char *title);
extern void create_textwindow_with_file(char *filename);
extern void draw_textwindow(struct window *wi, short x, short y, short w, short h);
extern int printf_textwindow(struct window *wi, const char* format, ...);
char *textwindow_load(struct window *wi, char *filename);
void delete_textwindow(struct window *wi);

#endif /* _TEXTWINDOW_H_ */

