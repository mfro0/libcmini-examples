#ifndef _DIALOG_H_
#define _DIALOG_H_

extern void init_dialogs();
extern void free_dialogs();

typedef short (init_callback_func)(OBJECT *dialog);
typedef short (exit_callback_func)(OBJECT *dialog, short exit_button);
typedef short (touchexit_callback_func)(OBJECT * dialog, short exit_obj);

struct dialog_handler
{
    short tree;
    OBJECT *dialog_object;                  /* the dialog object */
    short startobject;                  	/* for form_do(): start edit object */
    init_callback_func *init_callback;      /* called when initializing the dialog object */
    exit_callback_func *exit_callback;
    touchexit_callback_func *touchexit_callback;
    void (*dialog_do)(struct dialog_handler *dial);
};

extern void add_dialog(struct dialog_handler *dial);
extern void delete_dialog(struct dialog_handler *dial);
extern struct dialog_handler *from_treeindex(short tree);
extern struct dialog_handler *create_dialog(short tree, init_callback_func *ic, exit_callback_func *ec, touchexit_callback_func *te);

#endif /* _DIALOG_H_ */
