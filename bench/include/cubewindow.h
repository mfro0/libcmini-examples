#ifndef CUBEWINDOW_H
#define CUBEWINDOW_H

struct window *create_cubewindow(short wi_kind, char *title);

#define CUBEWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define CUBEWINDOW_CLASS	0x3347L

#endif // CUBEWINDOW_H
