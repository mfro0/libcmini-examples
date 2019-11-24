#include "window.h"
#include "global.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "vdiwindow.h"

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */


/*
 * return the smaller of two values
 */
static inline int min(int a, int b)
{
    return (a < b ? a : b);
}

/*
 * return the larger of two values
 */
static inline int max(int a, int b)
{
    return (a > b ? a : b);
}


/* private data for this window type */
struct vdiwindow
{
    bool new_turn;
    short color;
};

static void timer_vdiwindow(struct window *wi);
static void delete_vdiwindow(struct window *wi);
static void draw_vdiwindow(struct window *wi, short wx, short wy, short ww, short wh);

/*
 * create a new window and add it to the window list.
 */
struct window *create_vdiwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct vdiwindow *vw;

    dbg("start");

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = VDIWINDOW_CLASS;
        wi->draw = draw_vdiwindow;
        wi->del = delete_vdiwindow;
        wi->timer = timer_vdiwindow;

        wi->word_aligned = true;

        vw = malloc(sizeof(struct vdiwindow));

        if (vw != NULL)
        {
            wi->priv = vw;
            vw->color = 0;
            vw->new_turn = true;
        }
        else
        {
            fprintf(stderr, "%s: could not allocate vdiwindow private data\r\n", __FUNCTION__);
            return NULL;
        }

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */
    }
    dbg("finished");

    return wi;
}

static void delete_vdiwindow(struct window *wi)
{
    /* free window-private memory */
    if (wi && wi->priv)
    {
        struct vdiwindow *vw = (struct vdiwindow *) wi->priv;

        (void) vw; /* we might need it later */

        free(wi->priv);
    }
    /* let the generic window code do the rest */
    delete_window(wi);
}

/*
 * draw window
 */
static void draw_vdiwindow(struct window *wi, short wx, short wy, short ww, short wh)
{
    short x;
    short y;
    short w;
    short h;
    short vh = wi->vdi_handle;
    struct vdiwindow *vw = (struct vdiwindow *) wi->priv;

    /* get size of window's work area */
    wind_get(wi->handle, WF_WORKXYWH, &x, &y, &w, &h);

    /* first, clear it */
    if (wi->clear) wi->clear(wi, x, y, w, h);

    /* draw some filled circles */
    int xc = w / 2;
    int yc = h / 2;
    int r = min(xc, yc);
    int i;

    for (i = 0; i < 16; i++)
    {
        vsf_color(vh, i + vw->color);
        v_circle(vh, x + xc, y + yc, r);
        xc += 2;
        yc -= 2;
        r -= 3;
    }
}


/*
 * react on timer events
 */
static void timer_vdiwindow(struct window *wi)
{
    struct vdiwindow *vw = wi->priv;

    if (vw != NULL)
    {
        vw->new_turn = true;    /* signal drawing code that this is a new draw */
        do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
        vw->color += 1;
        vw->color &= 15;        /* flip through the first 16 color indices */
    }
}


