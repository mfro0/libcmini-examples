
#include "window.h"
#include "global.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "util.h"
#include "rasterwindow.h"


//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

/* include this after definition of dbg() */

#include "rcircles_bb_window.h"
#include "rasterdraw.h"

struct rcircles_rasterwindow
{
    bool new_turn;
    short color;
    struct raster backbuffer;
};

static void timer_rasterwindow(struct window *wi);
static void delete_rasterwindow(struct window *wi);
static void draw_rasterwindow(struct window *wi, short wx, short wy, short ww, short wh);

/*
 * create a new window and add it to the window list.
 */
struct window *create_circles_rasterwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct rcircles_rasterwindow *rw;

    dbg("start\r\n");

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = RASTERWINDOW_CLASS;
        wi->draw = draw_rasterwindow;
        wi->del = delete_rasterwindow;
        wi->timer = timer_rasterwindow;

        wi->word_aligned = false;

        rw = malloc(sizeof(struct rcircles_rasterwindow));

        if (rw != NULL)
        {
            wi->priv = rw;
            memset(rw, 0, sizeof(struct rcircles_rasterwindow));
        }
        else
        {
            fprintf(stderr, "%s: could not allocate rasterwindow private data\r\n", __FUNCTION__);
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
    dbg("finished\r\n");

    return wi;
}

static void delete_rasterwindow(struct window *wi)
{
    /* free window-private memory */
    if (wi && wi->priv)
    {
        struct rcircles_rasterwindow *rw = (struct rcircles_rasterwindow *) wi->priv;

        if (rw->backbuffer.address != NULL)
            free(rw->backbuffer.address);
        free(wi->priv);
    }
    /* let the generic window code do the rest */
    delete_window(wi);
}

/*
 * clear a buffer containing count shorts
 */
static inline void clear_buffer(short *start, const int count)
{
    memset(start, 0, count * 2);
}

/*
 * draw window
 */
static void draw_rasterwindow(struct window *wi, short wx, short wy, short ww, short wh)
{
    short x;
    short y;
    short w;
    short h;
    short start_angle;
    short end_angle;

    long backbuffer_size;

    struct rcircles_rasterwindow *rw = (struct rcircles_rasterwindow *) wi->priv;

    MFDB dst =
    {
        .fd_addr = NULL /* rest doesn't need to be filled if copy target is screen */
    };

    short pxy[8];


    /* get size of window's work area */
    wind_get(wi->handle, WF_WORKXYWH, &x, &y, &w, &h);

    /*
     * check if window backbuffer is available and if yes, if it has the correct size
     */
    if (rw->backbuffer.address == NULL ||
        rw->backbuffer.width != w || rw->backbuffer.height != h)
    {
        dbg("number of planes=%d\r\n", gl_nplanes);


        rw->backbuffer.width = w;
        rw->backbuffer.height = h;
        rw->backbuffer.num_planes = gl_nplanes;
        rw->backbuffer.wd_width = (w + 15) / 16;

        backbuffer_size = rw->backbuffer.wd_width * h * gl_nplanes * sizeof(short);

        /* need to realloc backbuffer */
        if (rw->backbuffer.address != NULL)
        {
            free(rw->backbuffer.address);
        }
        rw->backbuffer.address = malloc(backbuffer_size);

        if (rw->backbuffer.address == NULL)
        {
            /* if still no memory, return */
            fprintf(stderr, "could not allocate memory for back buffer\r\n");
            exit(1);
        }
        memset(rw->backbuffer.address, 0, backbuffer_size);
    }

    MFDB src =
    {
        .fd_addr = rw->backbuffer.address,
        .fd_w = rw->backbuffer.width,
        .fd_h = rw->backbuffer.height,
        .fd_wdwidth = (w + 15) / 16,
        .fd_stand = 0,  /* device specific format */
        .fd_nplanes = rw->backbuffer.num_planes,
        .fd_r1 = 0,
        .fd_r2 = 0,
        .fd_r3 = 0
    };


    dbg("height = %d, width = %d, wd_width = %d, num_planes=%d\r\n",
        rw->backbuffer.height, rw->backbuffer.width,
        rw->backbuffer.wd_width, rw->backbuffer.num_planes);
    /*
     * preset pxy array for raster copy
     */
    pxy[0] = wx - x;
    pxy[1] = wy - y;
    pxy[2] = pxy[0] + ww - 1;
    pxy[3] = pxy[1] + wh - 1;

    pxy[4] = wx;
    pxy[5] = wy;
    pxy[6] = wx + ww - 1;
    pxy[7] = wy + wh - 1;


    /*
     * only do an internal redraw if requested to do so, otherwise we assume we're
     * called during a rectangle list traversal and just need to copy buffers
     */
    if (rw->new_turn)
    {
        /* now paint something to the back buffer */

        /* draw some filled circles to backbuffer */
        int xc = rw->backbuffer.width / 2;
        int yc = rw->backbuffer.height / 2;
        int r = min(xc, yc);
        int i;

        set_clip(0, 0, rw->backbuffer.width, rw->backbuffer.height, true);

        /* draw 16 random, thick line clipped circles to backbuffer */
        for (i = 0; i < 16; i++)
        {
            xc = random(0, rw->backbuffer.width);
            yc = random(0, rw->backbuffer.height);
            r = random(0, min(rw->backbuffer.width, rw->backbuffer.height));
            start_angle = 100;
            end_angle = 350;

            set_linewidth(random(1, 15));

            if (gl_nplanes <= 8)
                draw_arc256(&rw->backbuffer, xc, yc, r, start_angle, end_angle, i + rw->color);
            else
                draw_circle32k(&rw->backbuffer, xc, yc, r, i + rw->color);
        }
        /* copy the back buffer to the front buffer */
        vro_cpyfm(wi->vdi_handle, S_ONLY, pxy, &src, &dst);

        rw->new_turn = false;   /* reset flag to avoid unnecessary buffer redraws */
    }
    else
    {
        /* copy the back buffer to the front buffer */
        vro_cpyfm(wi->vdi_handle, S_ONLY, pxy, &src, &dst);
    }
}


/*
 * react on timer events
 */
static void timer_rasterwindow(struct window *wi)
{
    struct rcircles_rasterwindow *rw = wi->priv;

    if (rw != NULL)
    {
        rw->new_turn = true;    /* signal drawing code that this is a new draw */
        do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
        rw->color += 1;
        rw->color &= 15;    /* flip through the first 16 color indices */
    }
}


