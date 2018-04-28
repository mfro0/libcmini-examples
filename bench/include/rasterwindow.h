#ifndef _RASTERWINDOW_H
#define _RASTERWINDOW_H

extern struct window *create_rasterwindow(short wi_kind, char *title);

#define RASTERWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define RASTERWINDOW_CLASS	0x3345L

#endif // RASTERWINDOW_H
