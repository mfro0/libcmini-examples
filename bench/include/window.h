#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <stdbool.h>
#include <gem.h>
#include <osbind.h>

struct window
{
    unsigned long wclass;
    short handle;
    long kind;
    bool word_aligned;

    GRECT rect;         /* current WF_SIZE */
    GRECT work;         /* = WF_WORKAREA */
    GRECT old;          /* previous rect */

    long left;          /* start of viewing rect in document */
    long top;

    long doc_width;
    long doc_height;

    long x_fac;         /* conversion factor document units <-> pixels in x */
    long y_fac;         /* conversion factor document units <-> pixels in y */

    int open;
    int topped;
    int fulled;

    char name[200];		/* this is the XAAES max length, TOS allows only 80 characters */
    char info[200];		/* the window's info line */

    void *priv;		/* pointer to private data available to "subclasses" */

    void (*full)(struct window *wi);
    void (*size)(struct window *wi, short x, short y, short w, short h);
    void (*draw)(struct window *wi, short x, short y, short w, short h);
    void (*del)(struct window *wi);
    void (*opn)(struct window *wi, short x, short y, short w, short h);
    void (*clear)(struct window *wi, short x, short y, short w, short h);
    void (*scroll)(struct window *wi);
    void (*timer)(struct window *wi);

    short work_in[10];
    short work_out[57];
    VdiHdl vdi_handle;
};

/*
 * global variables
 */
void init_windows(void);
void free_windows(void);

struct window *create_window(short wi_kind, char *title);
void delete_window(struct window *wi);
void open_window(struct window *wi, short x, short y, short w, short h);
void size_window(struct window *wi, short x, short y, short w, short h);
void clear_window(struct window *wi, short x, short y, short w, short h);
void full_window(struct window *wi);
void scroll_window(struct window *wi);

struct window *first_window();
struct window *next_window();

void send_redraw(struct window *wi, short x, short y, short w, short h);
void do_redraw(struct window *wi, short xc, short yc, short wc, short hc);
struct window *from_handle(short handle);
struct window *top_window(void);

typedef int (*wi_cb)(struct window *wi);
int foreach_window(wi_cb cb);

#define MIN_WIDTH  (10 * gl_wbox)
#define MIN_HEIGHT (10 * gl_hbox)

#endif /* _WINDOW_H_ */

