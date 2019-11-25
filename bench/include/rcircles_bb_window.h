#ifndef RCIRCLES_BB_WINDOW_H
#define RCIRCLES_BB_WINDOW_H

struct window *create_circles_rasterwindow(short wi_kind, char *title);

#define CIRCLES_RASTERWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define CIRCLES_RASTERWINDOW_CLASS	0x3346L



#endif // RCIRCLES_BB_WINDOW_H
