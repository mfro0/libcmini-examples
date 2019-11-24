#include "window.h"
#include "global.h"
#include <stdlib.h>
#include "grafwindow.h"

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

struct grafwindow {
    short ellipse_color;
};

static void draw_sample(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_grafwindow(struct window *wi);
static void delete_grafwindow(struct window *wi);

/*
 * create a new window and add it to the window list.
 */
struct window *create_grafwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct grafwindow *gw;

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = GRAFWINDOW_CLASS;
        wi->draw = draw_sample;
        wi->del = delete_grafwindow;
        wi->timer = timer_grafwindow;

        gw = malloc(sizeof(struct grafwindow));
        wi->priv = gw;

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */
    }
    return wi;
}

static void delete_grafwindow(struct window *wi)
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
    struct grafwindow *gw = (struct grafwindow *) wi->priv;
    short vh = wi->vdi_handle;

    wi->clear(wi, wx, wy, wh, ww);
    vsf_style(vh, FIS_PATTERN);
    vsf_interior(vh, 1);
    vsf_color(vh, gw->ellipse_color);
    v_ellipse(vh, wi->work.g_x + wi->work.g_w / 2,
                      wi->work.g_y + wi->work.g_h / 2,
                      wi->work.g_w / 2, wi->work.g_h / 2);
}


/*
 * react on timer events
 */
static void timer_grafwindow(struct window *wi)
{
    struct grafwindow *gw = (struct grafwindow *) wi->priv;

    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
    gw->ellipse_color++;
    gw->ellipse_color &= (1 << gl_nplanes) - 1;
}

