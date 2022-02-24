#ifndef TERRAINWINDOW_H
#define TERRAINWINDOW_H

struct window *create_terrainwindow(short wi_kind, char *title);

#define TERRAINWINDOW_WINELEMENTS SIZER|MOVER|FULLER|CLOSER|NAME|INFO
#define TERRAINWINDOW_CLASS	0x3349L

#endif // TERRAINWINDOW_H
