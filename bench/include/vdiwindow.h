#ifndef _VDIWINDOW_H_
#define _VDIWINDOW_H_


struct window *create_vdiwindow(short wi_kind, char *title);

#define VDIWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define VDIWINDOW_CLASS	0x3346L

#endif /* _VDIWINDOW_H_ */
