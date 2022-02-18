#ifndef BENCHMENU_H
#define BENCHMENU_H

#include <gem.h>
#include <stdint.h>
#include <stdbool.h>

short init_menu();
short free_menu();
short handle_menu(OBJECT *menu, short title, short item);
bool is_menu_key(OBJECT *menu_tree, short keyreturn, short keystate, short *title, short *item);

#endif /* BENCHMENU_H */

