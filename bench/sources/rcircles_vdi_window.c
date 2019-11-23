

#include "window.h"
#include "global.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rcircles_vdi_window.h"
#include "util.h"

//#define DEBUG
#ifdef DEBUG
/*
 * Since writing directly somewhere to the screen would distort GEM, the escape sequences in debug_printf()
 * position the cursor on line 30, column 0, clear to end of line and write the debug message.
 * Make sure you don't add a newline after the message or the screen will be clobbered.
 * This way we have at least one single line to display diagnostic output.
 */
#define dbg(format, arg...) do { printf("\033Y\36 \33lDEBUG (%s()): " format, __FUNCTION__, ##arg); (void) Cconin(); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

/* include this after definition of dbg() */

#include "rcircles_bb_window.h"

struct rcircles_rc_vdiwindow
{
    bool new_turn;
    short color;
};

static void timer_rc_vdiwindow(struct window *wi);
static void delete_rc_vdiwindow(struct window *wi);
static void draw_rc_vdiwindow(struct window *wi, short wx, short wy, short ww, short wh);

/*
 * create a new window and add it to the window list.
 */
struct window *create_rc_vdiwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct rcircles_rc_vdiwindow *vdiw;

    dbg("start");

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = CIRCLES_VDIWINDOW_CLASS;
        wi->draw = draw_rc_vdiwindow;
        wi->del = delete_rc_vdiwindow;
        wi->timer = timer_rc_vdiwindow;

        wi->word_aligned = false;

        vdiw = malloc(sizeof(struct rcircles_rc_vdiwindow));

        if (vdiw != NULL)
        {
            wi->priv = vdiw;
            vdiw->new_turn = true;
            vdiw->color = 0;
        }
        else
        {
            fprintf(stderr, "%s: could not allocate vdiwindow private data\r\n", __FUNCTION__);
            delete_window(wi);

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

static void delete_rc_vdiwindow(struct window *wi)
{
    /* free window-private memory */
    if (wi && wi->priv)
    {
        struct rcircles_rc_vdiwindow *vdiw = (struct rcircles_rc_vdiwindow *) wi->priv;
        (void) vdiw;        /* avoid warning about unused variable */

        free(wi->priv);
    }
    /* let the generic window code do the rest */
    delete_window(wi);
}

/*
 * draw window
 */
static void draw_rc_vdiwindow(struct window *wi, short wx, short wy, short ww, short wh)
{
    short x;
    short y;
    short w;
    short h;
    short pxy[4];
    short vh = wi->vdi_handle;

    struct rcircles_rc_vdiwindow *vdiw = (struct rcircles_rc_vdiwindow *) wi->priv;
    (void) vdiw;            /* avoid warning about unused variable */

    /* get size of window's work area */
    wind_get(wi->handle, WF_WORKXYWH, &x, &y, &w, &h);
    pxy[0] = wx;
    pxy[1] = wy;
    pxy[2] = wx + ww - 1;
    pxy[3] = wy + wh - 1;

    /* draw some filled circles */
    int xc;
    int yc;
    int i;
    short r;

    vs_clip(vh, 1, pxy);
    for (i = 0; i < 16; i++)
    {
        xc = random(0, w);
        yc = random(0, h);
        r = random(0, min(w, h));
        vsl_width(vh, random(1, 15));
        vsl_ends(vh, SQUARE, SQUARE);
        vsl_color(vh, random(0, 15));
        v_arc(vh, x + xc, y + yc, r, 300, 3300);
    }
}


/*
 * react on timer events
 */
static void timer_rc_vdiwindow(struct window *wi)
{
    struct rcircles_rc_vdiwindow *vdiw = wi->priv;

    if (vdiw != NULL)
    {
        vdiw->new_turn = true;    /* signal drawing code that this is a new draw */
        do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
        vdiw->color += 1;
        vdiw->color &= 15;    /* flip through the first 16 color indices */
    }
}


