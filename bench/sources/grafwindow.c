#include "window.h"
#include "global.h"
#include <stdlib.h>
#include "grafwindow.h"

//#define DEBUG
#ifdef DEBUG
/*
 * Since writing directly somewhere to the screen would distort GEM, the escape sequences in debug_printf()
 * position the cursor on line 30, column 0, clear to end of line and write the debug message.
 * Make sure you don't add a newline after the message or the screen will be clobbered.
 * This way we have at least one single line to display diagnostic output.
 */
#define debug_printf(format, arg...) do { printf("\033Y\36 \33lDEBUG (%s): " format, __FUNCTION__, ##arg); } while (0)
#else
#define debug_printf(format, arg...) do { ; } while (0)
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

    wi->clear(wi, wx, wy, wh, ww);
    vsf_style(vdi_handle, 8);
    vsf_interior(vdi_handle, 1);
    vsf_color(vdi_handle, gw->ellipse_color);
    v_ellipse(vdi_handle, wi->work.g_x + wi->work.g_w / 2,
                      wi->work.g_y + wi->work.g_h / 2,
                      wi->work.g_w / 2, wi->work.g_h / 2);
    gw->ellipse_color++;
    if (gw->ellipse_color > 8) gw->ellipse_color = 0;
}


/*
 * react on timer events
 */
static void timer_grafwindow(struct window *wi)
{
    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
}

