#include <gem.h>

#include "objutil.h"

short get_parent_object(OBJECT *tree, short index)
{
    do
    {
        index = tree[index].ob_next;
    } while (tree[tree[index].ob_next].ob_tail != index);

    return tree[index].ob_next;
}

int num_children(OBJECT *tree, short index)
{
    index = tree[index].ob_head;
    int num = 0;

    if (index == -1)
    {
        return 0;
    }

    num = 1;

    do
    {
        num++;
        index = tree[index].ob_next;
    } while (tree[tree[index].ob_next].ob_tail != index);

    return num;
}

int num_children_of_type(OBJECT *tree, short index, short type)
{
    index = tree[index].ob_head;
    int num = 0;

    if (index == -1)
    {
        return 0;
    }

    num = 1;

    do
    {
        if (tree[index].ob_type == type)
        {
            num++;
        }

        index = tree[index].ob_next;
    } while (tree[tree[index].ob_next].ob_tail != index);

    return num;
}
