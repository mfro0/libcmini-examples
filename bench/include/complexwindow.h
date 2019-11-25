#ifndef _COMPLEXWINDOW_H_
#define _COMPLEXWINDOW_H_

#include "window.h"

struct window *create_complexwindow(short wi_kind, char *title);

#define COMPLEXWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define COMPLEXWINDOW_CLASS	0x47111234
#endif /* _COMPLEXWINDOW_H_ */
