#include <stdlib.h>
#include <string.h>

#include "window.h"
#include "global.h"
#include "complexwindow.h"

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

struct complexwindow {
    int fillarea_pxy[12 * 2];	/* 12 points to define a complex polygon */
    short polygon_color;
};

static void draw_complex(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_complexwindow(struct window *wi);
static void delete_complexwindow(struct window *wi);

/*
 * create a new window and add it to the window list.
 */
struct window *create_complexwindow(short wi_kind, char *title)
{
    struct window *wi;
    struct complexwindow *cw;
    const int pxy[24] = {
            5, 5,
            25, 20,
            45, 5,
            40, 25,
            50, 30,
            35, 35,
            40, 50,
            25, 40,
            10, 50,
            15, 35,
            0, 30,
            20, 25
    };

    wi = create_window(wi_kind, title);
    if (wi)
    {
        wi->wclass = COMPLEXWINDOW_CLASS;
        wi->draw = draw_complex;
        wi->del = delete_complexwindow;
        wi->timer = timer_complexwindow;

        cw = malloc(sizeof(struct complexwindow));
        wi->priv = cw;

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = 1;
        wi->y_fac = 1;

        memcpy(&cw->fillarea_pxy, pxy, 12 * 2 * sizeof(int));
        cw->polygon_color = 1;
    }

    return wi;
}

static void delete_complexwindow(struct window *wi)
{
    if (wi->priv) free(wi->priv);
    /* let the generic window code do the rest */
    delete_window(wi);
}

/*
 * Draw Filled Ellipse
 */
static void draw_complex(struct window *wi, short wx, short wy, short wh, short ww)
{
    struct complexwindow *cw = (struct complexwindow *) wi->priv;
    short pxy[(12 + 1) * 2];
    int i;
    short vh = wi->vdi_handle;

    wi->clear(wi, wx, wy, wh, ww);
    vsf_style(vh, 8);
    vsf_interior(vh, 1);
    vsf_color(vh, cw->polygon_color);
    v_ellipse(vh, wi->work.g_x + wi->work.g_w / 2,
                  wi->work.g_y + wi->work.g_h / 2,
                  wi->work.g_w / 2, wi->work.g_h / 2);
    vsf_color(vh, cw->polygon_color + 1);
    for (i = 0; i < 24; i += 2)
    {
        pxy[i] = cw->fillarea_pxy[i] * wi->work.g_w / 50 + wi->work.g_x;
        pxy[i + 1] = cw->fillarea_pxy[i + 1] * wi->work.g_h / 50 + wi->work.g_y;
    }
    pxy[24] = pxy[0];
    pxy[25] = pxy[1];

    vsf_perimeter(vh, 1);

    v_fillarea(vh, 12, pxy);

    vsf_interior(vh, 0);
    vsl_width(vh, 6);
    vsl_color(vh, 1);
    vsf_color(vh, cw->polygon_color + 3);
    v_pline(vh, 13, pxy);


    cw->polygon_color++;
    if (cw->polygon_color > 8) cw->polygon_color = 0;
}

/*
 * react on timer events
 */
static void timer_complexwindow(struct window *wi)
{
    // struct grafwindow *gw = (struct grafwindow *) wi->private;
    //gw->ellipse_color++;
    //gw->ellipse_color %= 1 << gl_planes;

    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
}

