#include "window.h"
#include "global.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "imgrotwindow.h"
#include "bench.h"
#include <math.h>

// #define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

/* private data for this window type */
struct imgrotwindow
{
    CICONBLK *iconblk;
    CICON *icon;
    short angle;
    short *bitmap;
    MFDB icon_mfdb;
};

static void timer_imgrotwindow(struct window *wi);
static void delete_imgrotwindow(struct window *wi);
static void draw_imgrotwindow(struct window *wi, short wx, short wy, short ww, short wh);

/*
 * create a new window and add it to the window list.
 */
struct window *create_imgrotwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct imgrotwindow *iw;

    wi = create_window(wi_kind, title);

    short vh = wi->vdi_handle;

    if (wi != NULL)
    {
        OBJECT *dlg;

        wi->wclass = IMGROTWINDOW_CLASS;
        wi->draw = draw_imgrotwindow;
        wi->del = delete_imgrotwindow;
        wi->timer = timer_imgrotwindow;
        wi->word_aligned = true;

        iw = malloc(sizeof(struct imgrotwindow));

        if (iw != NULL)
        {
            wi->priv = iw;
            iw->angle = 0;
            iw->bitmap = NULL;
        }
        else
        {
            fprintf(stderr, "%s: could not allocate vdiwindow private data\r\n", __FUNCTION__);
            return NULL;
        }

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */

        if (!rsrc_gaddr(R_TREE, ICNS, &dlg))
        {
            form_alert(1, "[2][Could not get RASTER resource][CANCEL]");
            exit(1);
        }

        CICONBLK *iconblk = dlg[COLICON].ob_spec.ciconblk;
        CICON *icon = iconblk->mainlist;

        MFDB src_mfdb =
        {
            .fd_w = iconblk->monoblk.ib_wicon,
            .fd_h = iconblk->monoblk.ib_hicon,
            .fd_wdwidth = (iconblk->monoblk.ib_wicon + 15) / sizeof(short) / 8,
            .fd_stand = 1,
            .fd_r1 = 0,
            .fd_r2 = 0,
            .fd_r3 = 0
        };

        /*
         * find the icon data from the available color depths in the CICONBLK that fits best
         */
        iw->icon = NULL;
        do {
            dbg("found depth %d\r\n", icon->num_planes);
            if (icon->num_planes <= gl_nplanes
                    && icon->num_planes > src_mfdb.fd_nplanes)
            {
                src_mfdb.fd_addr = icon->col_data;
                src_mfdb.fd_nplanes = icon->num_planes;
                iw->iconblk = iconblk;
                iw->icon = icon;
            }
            icon = icon->next_res;
        } while (icon != NULL);

        dbg("selected icon of depth %d\r\n", src_mfdb.fd_nplanes);

        if (iw->iconblk == NULL)
        {
            form_alert(1, "[1][No suitable color icon found][CANCEL]");
            exit(1);
        }


        MFDB dst =
        {
            .fd_addr = iw->icon->col_data,
            .fd_w = iconblk->monoblk.ib_wicon,
            .fd_h = iconblk->monoblk.ib_hicon,
            .fd_wdwidth = (iconblk->monoblk.ib_wicon + 15) / sizeof(short) / 8,
            .fd_stand = 0,
            .fd_nplanes = gl_nplanes,
            .fd_r1 = 0,
            .fd_r2 = 0,
            .fd_r3 = 0
        };
        iw->icon_mfdb = dst;


    }

    return wi;
}

static void delete_imgrotwindow(struct window *wi)
{
    /* free window-private memory */
    if (wi && wi->priv)
    {
        struct imgrotwindow *iw = (struct imgrotwindow *) wi->priv;

        if (iw->bitmap != NULL)
            free(iw->bitmap);

        free(wi->priv);
    }
    /* let the generic window code do the rest */
    delete_window(wi);
}

/*
 * draw window
 */
static void draw_imgrotwindow(struct window *wi, short wx, short wy, short ww, short wh)
{
    short x;
    short y;
    short w;
    short h;

    short vh = wi->vdi_handle;
    struct imgrotwindow *iw = (struct imgrotwindow *) wi->priv;

    (void) wx; (void) wy; (void) ww; (void) wh;

    /* get size of window's work area */
    wind_get(wi->handle, WF_WORKXYWH, &x, &y, &w, &h);

    /* first, clear it */
    if (wi->clear) wi->clear(wi, x, y, w, h);

    /* draw our (possibly rotated) icon */
    MFDB dst = { 0 };

    short wicon = iw->iconblk->monoblk.ib_wicon;
    short hicon = iw->iconblk->monoblk.ib_hicon;

    /*
     * center icon into window's work area
     */
    short pxy[8] =
    {
        0, 0, wicon - 1, hicon - 1,
        x + w / 2 - wicon / 2,
        y + h / 2 - hicon / 2,
        x + w / 2 + wicon / 2 - 1,
        y + h / 2 + hicon / 2 - 1
    };

    vro_cpyfm(vh, S_ONLY,
              pxy,
              &iw->icon_mfdb, &dst);
}


/*
 * react on timer events
 */
static void timer_imgrotwindow(struct window *wi)
{
    struct imgrotwindow *iw = wi->priv;

    if (iw != NULL)
    {
        do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
        iw->angle += 1;
    }
}

double vectors_dot_prod(const double *x, const double *y, int n)
{
    double res = 0.0;
    int i;

    for (i = 0; i < n; i++)
    {
        res += x[i] * y[i];
    }
    return res;
}

void matrix_vector_mult(const double **mat, const double *vec, double *result, int rows, int cols)
{
    // in matrix form: result = mat * vec;
    int i;
    for (i = 0; i < rows; i++)
    {
        result[i] = vectors_dot_prod(mat[i], vec, cols);
    }
}

/*
 * shearing of image src to dst around angle in x-direction
 * center point at (x, y)
 * angle is in 10ths of degrees (0-3600)
 */
void xshear(MFDB *src, MFDB *dst, short x, short y, short angle)
{
}

/*
 * shearing of image src to dst around angle in y-direction
 * center point at (x, y)
 * angle is in 10ths of degrees (0-3600)
 */
void yshear(MFDB *src, MFDB *dst, short x, short y, short angle)
{

}

