#ifndef RCIRCLES_VDI_WINDOW_H
#define RCIRCLES_VDI_WINDOW_H

extern struct window *create_rc_vdiwindow(short wi_kind, char *title);

#define CIRCLES_VDIWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define CIRCLES_VDIWINDOW_CLASS	0x3347L

#endif // RCIRCLES_VDI_WINDOW_H
