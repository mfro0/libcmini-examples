#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "portab.h"
#include <gem.h>

#define RESOURCENAME "viewer.rsc"

extern OBJECT *menu;

extern short init_resource();
extern void free_resource();

#endif /* _RESOURCE_H_ */

