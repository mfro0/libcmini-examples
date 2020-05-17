#ifndef FONTWINDOW_H
#define FONTWINDOW_H

#include "window.h"
#include <time.h>

struct window *create_fontwindow(short wi_kind, char *title);

#define FONTWINDOW_WINELEMENTS SIZER|MOVER|HSLIDE|VSLIDE|UPARROW|DNARROW|LFARROW|RTARROW|FULLER|CLOSER|NAME|INFO
#define FONTWINDOW_CLASS	0x47111237

struct finfo
{
    short font_index;
    char font_name[33];
};

struct fontwindow
{
    bool gdos_available;
    int add_fonts;
    struct finfo *font_info;
};


#endif // FONTWINDOW_H
