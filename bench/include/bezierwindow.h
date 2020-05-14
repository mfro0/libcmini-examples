#ifndef BEZIERWINDOW_H
#define BEZIERWINDOW_H

#include "window.h"

struct window *create_bezierwindow(short wi_kind, char *title);

#define BEZIERWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define BEZIERWINDOW_CLASS	0x47211234
#endif /* BEZIERWINDOW_H */
