#include <stdlib.h>
#include <string.h>

#include "window.h"
#include "global.h"
#include "bezierwindow.h"
#include <limits.h>
#include "intmath.h"
#include <gemx.h>

// #define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

struct bezierwindow {
    short bezarea_pxy[13 * 2];	/* 12 points to define a "bezier-shaped potato" */
    char bez[13];               /* the point-type flags array */
    short num_pts;
    short polygon_color;
    short rot_angle;
    short pxy_max_x;
    short pxy_max_y;
};

static void draw_bezier(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_bezierwindow(struct window *wi);
static void delete_bezierwindow(struct window *wi);

/*
 * create a new window and add it to the window list.
 */
struct window *create_bezierwindow(short wi_kind, char *title)
{
    struct window *wi;
    struct bezierwindow *bw;

    const short pxy[] = {
        40, 20,
        40, 35,
        35, 40,
        20, 40,
        5, 40,
        0, 35,
        0, 20,
        0, 5,
        5, 0,
        20, 0,
        35, 0,
        40, 10,
        40, 20
    };

    const char bez[] = {
        1, 0, 0, 1,
           0, 0, 1,
           0, 0, 1,
           0, 0, 1
    };


    wi = create_window(wi_kind, title);
    if (wi)
    {
        int i;

        wi->wclass = BEZIERWINDOW_CLASS;
        wi->draw = draw_bezier;
        wi->del = delete_bezierwindow;
        wi->timer = timer_bezierwindow;

        bw = malloc(sizeof(struct bezierwindow));
        wi->priv = bw;

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = 1;
        wi->y_fac = 1;

        bw->num_pts = sizeof(bez) / sizeof(bez[0]);
        memcpy(&bw->bezarea_pxy, pxy, bw->num_pts * 2 * sizeof(short));
        memcpy(&bw->bez, bez, bw->num_pts * sizeof(char));

        /*
         * calculate max extent
         */
        bw->pxy_max_x = 0;
        bw->pxy_max_y = 0;

        for (i = 0; i < 24; i++)
        {
            bw->pxy_max_x = bw->bezarea_pxy[i] > bw->pxy_max_x ? bw->bezarea_pxy[i] : bw->pxy_max_x;
            bw->pxy_max_y = bw->bezarea_pxy[i + 1] > bw->pxy_max_y ? bw->bezarea_pxy[i + 1] : bw->pxy_max_y;
        }
        bw->polygon_color = 1;
        bw->rot_angle = 0;
    }

    return wi;
}

static void delete_bezierwindow(struct window *wi)
{
    if (wi->priv) free(wi->priv);
    /* let the generic window code do the rest */
    delete_window(wi);
}

/*
 * Draw our "bezier shaped potato"
 */
static void draw_bezier(struct window *wi, short wx, short wy, short wh, short ww)
{
    struct bezierwindow *bw = wi->priv;
    short *coords = bw->bezarea_pxy;
    short pxy[bw->num_pts * 2];
    int i;
    short vh = wi->vdi_handle;

    wi->clear(wi, wx, wy, wh, ww);

    /*
     * rotate polygon coordinates
     */
    for (i = 0; i < bw->num_pts * 2; i += 2)
    {
        short xr;
        short yr;
        short ang = bw->rot_angle;
        short x = coords[i];
        short y = coords[i + 1];

        /*
         * translate to origin
         */
        x -= bw->pxy_max_x / 2;
        y -= bw->pxy_max_y / 2;

        /*
         * rotate
         */
        xr = ((long) x * icos(ang)) / SHRT_MAX - ((long) y * isin(ang)) / SHRT_MAX;
        yr = ((long) x * isin(ang)) / SHRT_MAX + ((long) y * icos(ang)) / SHRT_MAX;

        x = xr;
        y = yr;

        /* translate back */
        x += bw->pxy_max_x / 2 + 5;
        y += bw->pxy_max_y / 2 + 5;

        /* scale to window */
        x = x * wi->work.g_w / 50 + wi->work.g_x;
        y = y * wi->work.g_h / 50 + wi->work.g_y;

        pxy[i] = x;
        pxy[i + 1] = y;
    }

    vsf_perimeter(vh, 1);

    vsl_width(vh, 20);
    vsl_color(vh, bw->polygon_color);
    vsf_color(vh, bw->polygon_color + 3);

    short ext[4], totpts, totmvs;

    v_bez_fill(vh, bw->num_pts, pxy, bw->bez, ext, &totpts, &totmvs);
    dbg("ext=(%d, %d, %d, %d), totpts=%d, totmvs=%d\r\n",
        ext[0], ext[1], ext[2], ext[3],
        totpts, totmvs);
}

/*
 * react on timer events
 */
static void timer_bezierwindow(struct window *wi)
{
    struct bezierwindow *cw = wi->priv;

    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);

    cw->rot_angle = cw->rot_angle - 50;   /* 5 degree steps */
    cw->rot_angle = cw->rot_angle < 0 ? 3600 + cw->rot_angle : cw->rot_angle;
    cw->polygon_color++;
    cw->polygon_color %= 8;
}

