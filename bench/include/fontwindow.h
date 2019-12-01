#ifndef FONTWINDOW_H
#define FONTWINDOW_H

#include "window.h"
#include <time.h>

struct window *create_fontwindow(short wi_kind, char *title);

#define FONTWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define FONTWINDOW_CLASS	0x47111237

struct fontwindow
{
    void *bg_buffer;      /* buffer to store a bitmap of the clock face for optimized redraw */
    bool gdos_available;
    char (*fontnames)[32];
    int add_fonts;
};


#endif // FONTWINDOW_H
