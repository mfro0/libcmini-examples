#include <stdlib.h>
#include <string.h>

#include "window.h"
#include "global.h"
#include "clockwindow.h"
#include <limits.h>
#include "intmath.h"

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

struct clockwindow {
    short pxy_max_x;
    short pxy_max_y;
};

static void draw_clockwindow(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_clockwindow(struct window *wi);
static void delete_clockwindow(struct window *wi);

static (*old_timer_handler)(void) = NULL;
static short timer_handler()
{

    (* old_timer_handler)();
}

/*
 * create a new window and add it to the window list.
 */
struct window *create_clockwindow(short wi_kind, char *title)
{
    struct window *wi;
    struct clockwindow *cw;

    wi = create_window(wi_kind, title);
    if (wi)
    {
        wi->wclass = CLOCKWINDOW_CLASS;
        wi->draw = draw_clockwindow;
        wi->del = delete_clockwindow;
        wi->timer = timer_clockwindow;

        cw = malloc(sizeof(struct clockwindow));
        wi->priv = cw;

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = 1;
        wi->y_fac = 1;
        short vh = wi->vdi_handle;
        vex_timv(vh, timer_handler, &old_timer_handler, 50);
    }

    return wi;
}

static void delete_clockwindow(struct window *wi)
{
    if (wi->priv) free(wi->priv);
    /* let the generic window code do the rest */
    delete_window(wi);
}

static short min(short a, short b)
{
    return a < b ? a : b;
}

/*
 * Draw a wall clock
 */
static void draw_clockwindow(struct window *wi, short wx, short wy, short ww, short wh)
{
    struct clockwindow *cw = wi->priv;

    short vh = wi->vdi_handle;

    short rad = min(ww, wh) / 2;
    short minute;

    wi->clear(wi, wx, wy, ww, wh);

    /* an hour equals 60 minutes */
    for (minute = 0; minute < 60; minute++)
    {
        short ang = minute * 3600L / 60L;
        short pxy[4];
        /* rotate indicator into place */

        short min5 = minute % 5 == 0;
        pxy[0] = (min5 ? rad - 20 : rad - 10) * icos(ang) / SHRT_MAX;
        pxy[1] = (min5 ? rad - 20 : rad - 10) * isin(ang) / SHRT_MAX;
        pxy[2] = (rad - 2) * icos(ang) / SHRT_MAX;
        pxy[3] = (rad - 2) * isin(ang) / SHRT_MAX;

        /* translate to position */
        pxy[0] += wx + ww / 2;
        pxy[1] += wy + wh / 2;
        pxy[2] += wx + ww / 2;
        pxy[3] += wy + wh / 2;
        vsl_color(vh, 1);
        vsl_width(vh, 1);
        v_pline(vh, 2, pxy);
    }
}

/*
 * react on timer events
 */
static void timer_clockwindow(struct window *wi)
{
    struct clockwindow *cw = wi->priv;
    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
}

