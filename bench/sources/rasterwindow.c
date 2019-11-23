#include "window.h"
#include "global.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rasterwindow.h"


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
#define dbg(format, arg...) do { } while (0)
#endif /* DEBUG */

/* include this after definition of dbg() */

#include "rasterdraw.h"

struct rasterwindow
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
struct window *create_rasterwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct rasterwindow *rw;

    dbg("start");

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = RASTERWINDOW_CLASS;
        wi->draw = draw_rasterwindow;
        wi->del = delete_rasterwindow;
        wi->timer = timer_rasterwindow;

        wi->word_aligned = false;

        rw = malloc(sizeof(struct rasterwindow));

        if (rw != NULL)
        {
            memset(rw, 0, sizeof(struct rasterwindow));
            wi->priv = rw;
            rw->new_turn = true;
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
    dbg("finished");

    return wi;
}

static void delete_rasterwindow(struct window *wi)
{
    /* free window-private memory */
    if (wi && wi->priv)
    {
        struct rasterwindow *rw = (struct rasterwindow *) wi->priv;

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
    short vh = wi->vdi_handle;

    struct rasterwindow *rw = (struct rasterwindow *) wi->priv;

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
        rw->backbuffer.width = w;
        rw->backbuffer.height = h;
        rw->backbuffer.num_planes = gl_nplanes;
        rw->backbuffer.wd_width = (w + 15) / 16;  /* round up to next 16-bit boundary */

        /* need to realloc backbuffer */
        if (rw->backbuffer.address != NULL)
        {
            free(rw->backbuffer.address);
            rw->backbuffer.address = malloc((rw->backbuffer.height *
                                             rw->backbuffer.wd_width * sizeof(short) *
                                             rw->backbuffer.num_planes));
        }
        else
        {
            rw->backbuffer.address = malloc((rw->backbuffer.height * rw->backbuffer.wd_width * sizeof(short) *
                                             rw->backbuffer.num_planes));
        }
        if (rw->backbuffer.address == NULL)
        {
            /* if still no memory, return */
            fprintf(stderr, "could not allocate memory for back buffer\r\n");
            exit(1);
        }
    }

    MFDB src =
    {
        .fd_addr = rw->backbuffer.address,
        .fd_w = rw->backbuffer.width,
        .fd_h = rw->backbuffer.height,
        .fd_wdwidth = rw->backbuffer.wd_width,
        .fd_stand = 0,  /* device specific format */
        .fd_nplanes = rw->backbuffer.num_planes,
        .fd_r1 = 0,
        .fd_r2 = 0,
        .fd_r3 = 0
    };
    dbg("height = %d, width = %d, wd_width = %d",
                 rw->backbuffer.height, rw->backbuffer.width, rw->backbuffer.wd_width);

    /*
     * only do an internal redraw if requested to do so, otherwise we assume we're
     * called during a rectangle list traversal and just need to copy buffers
     */
    if (rw->new_turn)
    {
        /* now paint something to the back buffer */

        dbg("clear buffer (from %p, w=%d, h=%d, np=%d", rw->backbuffer.address, rw->backbuffer.width,
            rw->backbuffer.height, rw->backbuffer.num_planes);
        /* first, clear it (assuming setting all bits to zero really clears it) */
        clear_buffer(rw->backbuffer.address,
                     rw->backbuffer.wd_width * rw->backbuffer.height * rw->backbuffer.num_planes);


        /* draw some filled circles to backbuffer */
        int xc = rw->backbuffer.width / 2;
        int yc = rw->backbuffer.height / 2;
        int r = min(xc, yc);
        int i;

        pxy[0] = wx - x;
        pxy[1] = wy - y;
        pxy[2] = pxy[0] + ww - 1;
        pxy[3] = pxy[1] + wh - 1;

        pxy[4] = wx;
        pxy[5] = wy;
        pxy[6] = wx + ww - 1;
        pxy[7] = wy + wh - 1;


        set_clip(0, 0, rw->backbuffer.width, rw->backbuffer.height, true);

        for (i = 0; i < 16; i++)
        {
            dbg("draw_filled_circle p=%p x=%d y=%d, r=%d c=%d",
                rw->backbuffer.address, xc, yc, r, i + rw->color);
            if (rw->backbuffer.num_planes <= 8)
                draw_filled_circle256(&rw->backbuffer, xc, yc, r, i + rw->color);
            else
                draw_filled_circle32k(&rw->backbuffer, xc, yc, r, i + rw->color);

            xc += 2;
            yc -= 2;
            r -= 3;

        }
        /* copy the back buffer to the front buffer */
        vro_cpyfm(vh, S_ONLY, pxy, &src, &dst);

        rw->new_turn = false;   /* reset flag to avoid unnecessary buffer redraws */
    }
}


/*
 * react on timer events
 */
static void timer_rasterwindow(struct window *wi)
{
    struct rasterwindow *rw = wi->priv;

    if (rw != NULL)
    {
        rw->new_turn = true;    /* signal drawing code that this is a new draw */
        do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
        rw->color += 1;
        rw->color &= 15;    /* flip through the first 16 color indices */
    }
}


