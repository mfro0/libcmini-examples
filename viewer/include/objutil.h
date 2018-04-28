#ifndef OBJUTIL_H
#define OBJUTIL_H

extern short get_parent_object(OBJECT *tree, short index);
extern int num_children(OBJECT *tree, short index);
extern int num_children_of_type(OBJECT *tree, short index, short type);

#endif // OBJUTIL_H
