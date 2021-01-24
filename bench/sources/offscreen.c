#include "window.h"
#include "global.h"
#include <stdlib.h>
#include "offscreen.h"
#include <gemx.h>

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

struct offscreenwindow
{
    short oh;           /* offscreen bitmap handle */
    short *bm;
    short ellipse_color;
};

static void draw_sample(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_offscreenwindow(struct window *wi);
static void delete_offscreenwindow(struct window *wi);

/*
 * create a new window and add it to the window list.
 */
struct window *create_offscreenwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct offscreenwindow *ow;

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = OFFWINDOW_CLASS;
        wi->draw = draw_sample;
        wi->del = delete_offscreenwindow;
        wi->timer = timer_offscreenwindow;

        ow = malloc(sizeof(struct offscreenwindow));
        if (ow != NULL)
        {
            wi->priv = ow;
            ow->oh = v_open_bm(wi->vdi_handle, NULL, 1, 0, 600, 400);
        }

        if (! ow->oh)
        {
            dbg("v_open_bm() failed.\r\n");
        }

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */
    }
    return wi;
}

static void delete_offscreenwindow(struct window *wi)
{
    if (wi && wi->priv) free(wi->priv);
    /* let the generic window code do the rest */
    delete_window(wi);
}

/*
 * Draw Filled Ellipse
 */
static void draw_sample(struct window *wi, short wx, short wy, short wh, short ww)
{
    struct offscreenwindow *ow = wi->priv;
    short vh = wi->vdi_handle;

    wi->clear(wi, wx, wy, wh, ww);
    vsf_style(vh, FIS_PATTERN);
    vsf_interior(vh, 1);
    vsf_color(vh, ow->ellipse_color);
    v_ellipse(vh, wi->work.g_x + wi->work.g_w / 2,
                      wi->work.g_y + wi->work.g_h / 2,
                      wi->work.g_w / 2, wi->work.g_h / 2);
}


/*
 * react on timer events
 */
static void timer_offscreenwindow(struct window *wi)
{
    struct offscreenwindow *gw = (struct offscreenwindow *) wi->priv;

    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
    gw->ellipse_color++;
    gw->ellipse_color &= (1 << gl_nplanes) - 1;
}

