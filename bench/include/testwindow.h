#ifndef _TESTWINDOW_H_
#define _TESTWINDOW_H_

struct window *create_testwindow(short wi_kind, char *title);

#define TESTWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define TESTWINDOW_CLASS	0x2346L
#endif /* _GRAFWINDOW_H_ */
