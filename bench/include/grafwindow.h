#ifndef _GRAFWINDOW_H_
#define _GRAFWINDOW_H_

struct window *create_grafwindow(short wi_kind, char *title);

#define GRAFWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define GRAFWINDOW_CLASS	0x2345L
#endif /* _GRAFWINDOW_H_ */
