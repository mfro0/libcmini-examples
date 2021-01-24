#ifndef IMGROTWINDOW_H
#define IMGROTWINDOW_H

struct window *create_imgrotwindow(short wi_kind, char *title);

#define IMGROTWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define IMGROTWINDOW_CLASS	0x3346L


#endif // IMGROTWINDOW_H
