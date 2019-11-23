#ifndef CLOCKWINDOW_H
#define CLOCKWINDOW_H


#include "window.h"

extern struct window *create_clockwindow(short wi_kind, char *title);

#define CLOCKWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define CLOCKWINDOW_CLASS	0x47111236

#endif // CLOCKWINDOW_H
