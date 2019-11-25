#ifndef CLOCKWINDOW_H
#define CLOCKWINDOW_H


#include "window.h"
#include <time.h>

extern struct window *create_clockwindow(short wi_kind, char *title);

#define CLOCKWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define CLOCKWINDOW_CLASS	0x47111236

struct clockwindow
{
    short pxy_max_x;
    short pxy_max_y;
    void *face_buffer;      /* buffer to store a bitmap of the clock face for optimized redraw */
    struct tm prev;
};

#endif // CLOCKWINDOW_H
