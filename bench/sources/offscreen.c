#include "window.h"
#include "global.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "offscreen.h"
#include <gemx.h>

#define DEBUG
#ifdef DEBUG
#ifdef __mcoldfire__
#define dbg(format, arg...) do { fprintf(stderr, "DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { fprintf(format, ##arg); } while (0)
#else
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#endif /* __mcoldfire__ */
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

struct offscreenwindow
{
    short bm_handle;           /* offscreen bitmap handle */
    MFDB bm;
    short ellipse_color;
    short ellipse_pattern;
};

static void draw_sample(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_offscreenwindow(struct window *wi);
static void delete_offscreenwindow(struct window *wi);
static void size_offscreenwindow(struct window *wi, short x, short y, short w, short h);
/*
 * create a new window and add it to the window list.
 */
struct window *create_offscreenwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct offscreenwindow *ow;
    short work_in[20] = { 0 };
    short work_out[57];

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = OFFWINDOW_CLASS;
        wi->draw = draw_sample;
        wi->del = delete_offscreenwindow;
        wi->timer = timer_offscreenwindow;
        wi->size = size_offscreenwindow;

        ow = malloc(sizeof(struct offscreenwindow));
        if (ow != NULL)
        {
            wi->priv = ow;
            ow->ellipse_color = 0;
            ow->ellipse_pattern = 0;
        }

        ow->bm_handle = wi->vdi_handle;
        memset(&ow->bm, 0, sizeof(MFDB));

        work_in[10] = 2;

        work_in[11] = 1279;
        work_in[12] = 959;
        work_in[13] = 1000;
        work_in[14] = 1000;

        dbg("VDI handle: %d\n", vdi_handle);
        v_opnbm(work_in, &ow->bm, &ow->bm_handle, work_out);

        dbg("bitmap handle: %d\n", ow->bm_handle);
        dbg("MFDB.fd_addr=%p\n", ow->bm.fd_addr);
        dbg("MFDB.fd_w=%d\n", ow->bm.fd_w);
        dbg("MFDB.fd_h=%d\n", ow->bm.fd_h);
        dbg("MFDB.fd_wdwidth=%d\n", ow->bm.fd_wdwidth);
        dbg("MFDB.fd_stand=%d\n", ow->bm.fd_stand);
        dbg("MFDB.fd_nplanes=%d\n", ow->bm.fd_nplanes);

        if (! ow->bm_handle)
        {
            form_alert(1, "[Could not get offscreen | bitmap handle (v_opnbm)][CANCEL]");

            wi->del(wi);
            return NULL;
        }

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = ow->bm.fd_w;
        wi->doc_height = ow->bm.fd_h;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */
    }
    return wi;
}

static void delete_offscreenwindow(struct window *wi)
{
    if (wi && wi->priv)
    {
        struct offscreenwindow *ow = (struct offscreenwindow *) wi->priv;

        if (ow->bm_handle)
        {
            v_clsbm(ow->bm_handle);
        }
        free(wi->priv);
    }
    /* let the generic window code do the rest */
    delete_window(wi);
}

static void size_offscreenwindow(struct window *wi, short x, short y, short w, short h)
{
    struct offscreenwindow *ow = wi->priv;

    wi->top = 0;
    wi->left = 0;

    size_window(wi, x, y, w, h);        /* call super "class"' size method */

}
/*
 * Draw Filled Ellipse
 */
static void draw_sample(struct window *wi, short wx, short wy, short ww, short wh)
{
    struct offscreenwindow *ow = wi->priv;
    short vh = wi->vdi_handle;

    MFDB screen = { 0 };
    short pxy[8] = { wi->left, wi->top, wi->left + ww - 1, wi->top + wh - 1,
                     wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h };

    vro_cpyfm(vh, S_ONLY, pxy, &ow->bm, &screen);
}


/*
 * react on timer events
 */
static void timer_offscreenwindow(struct window *wi)
{
    struct offscreenwindow *ow = (struct offscreenwindow *) wi->priv;
    short bh = ow->bm_handle;

    short pxy[8] = { 0, 0, ow->bm.fd_w - 1, ow->bm.fd_h - 1, 0, 0, ow->bm.fd_w - 1, ow->bm.fd_h - 1 };

    // vr_recfl(ow->bm_handle, pxy);
    vro_cpyfm(ow->bm_handle, ALL_WHITE, pxy, &ow->bm, &ow->bm);

    vsf_style(bh, FIS_PATTERN);
    vsf_interior(bh, ow->ellipse_pattern);
    vsf_interior(bh, 1);
    vsf_color(bh, ow->ellipse_color);
    v_ellipse(bh, wi->work.g_w / 2, wi->work.g_h / 2,
                  wi->work.g_w / 2, wi->work.g_h / 2);


    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);

    ow->ellipse_color++;
    ow->ellipse_color &= 15;

    if (ow->ellipse_color == 15)
    {
        ow->ellipse_pattern++;
        ow->ellipse_pattern &= 15;
    }

}

