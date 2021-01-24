#include "window.h"
#include "global.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "imgrotwindow.h"
#include "bench.h"
#include <math.h>

#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

#ifdef NOT_USED
/*
 * return the smaller of two values
 */
static inline int min(int a, int b)
{
    return (a < b ? a : b);
}

/*
 * return the larger of two values
 */
static inline int max(int a, int b)
{
    return (a > b ? a : b);
}
#endif /* NOT_USED */

/* private data for this window type */
struct imgrotwindow
{
    CICONBLK *iconblk;
    CICON *icon;
    short angle;
    short *bitmap;
};

static void timer_imgrotwindow(struct window *wi);
static void delete_imgrotwindow(struct window *wi);
static void draw_imgrotwindow(struct window *wi, short wx, short wy, short ww, short wh);

static MFDB icon_mfdb;

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

        iw->icon = NULL;
        do {
            dbg("found depth %d\r\n", icon->num_planes);
            if (icon->num_planes <= gl_nplanes)
            {
                src_mfdb.fd_addr = icon->col_data;
                src_mfdb.fd_nplanes = icon->num_planes;
                iw->iconblk = iconblk;
                iw->icon = icon;
            }
            icon = icon->next_res;
        } while (icon != NULL);
        if (iw->icon == NULL)
        {
            form_alert(1, "[1][No suitable color icon found][CANCEL]");
            exit(1);
        }

        /*
         * now create a device dependent bitmap from raster
         */
        iw->bitmap = malloc((iconblk->monoblk.ib_wicon + 15) / sizeof(short) / 8 *
                             iconblk->monoblk.ib_hicon * gl_nplanes);
        if (iw->bitmap != NULL)
        {

            MFDB dst =
            {
                .fd_addr = iw->bitmap,        /* screen */
                .fd_w = iconblk->monoblk.ib_wicon,
                .fd_h = iconblk->monoblk.ib_hicon,
                .fd_wdwidth = (iconblk->monoblk.ib_wicon + 15) / sizeof(short) / 8,
                .fd_stand = 0,
                .fd_nplanes = gl_nplanes,
                .fd_r1 = 0,
                .fd_r2 = 0,
                .fd_r3 = 0
            };
            icon_mfdb = dst;

            //vr_trnfm(vh, &src_mfdb, &icon_mfdb);
        }
        else
        {
            form_alert(2, "[1][Could not allocate bitmap memory][CANCEL]");
            exit(1);
        }
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
    dbg("wicon=%d, hicon=%d\r\n", wicon, hicon);

    icon_mfdb.fd_addr = iw->icon->col_data;

    short pxy[8] =
    {
        0, 0, wicon - 1, hicon - 1,
        x + w / 2 - wicon / 2,
        y + h / 2 - hicon / 2,
        x + w / 2 + wicon / 2 - 1,
        y + h / 2 + hicon / 2 - 1
    };

    dbg("\r\npxy= %d, %d, %d, %d \r\n    %d, %d, %d, %d\r\n",
        pxy[0], pxy[1], pxy[2], pxy[3],
        pxy[4], pxy[5], pxy[6], pxy[7]);

    vs_clip(vh, 0, (short[4]) {0, 0, 0, 0});
    vro_cpyfm(vh, S_ONLY,
              pxy,
              &icon_mfdb, &dst);
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
 * calculates a shearing operation of the pixel at coordinates pos by angle
 */
void xshear(short *pos, double angle)
{
    double ret[2];
    const double mat[][2] = { { 1, -tan(angle / 2) }, { 0, 1 }};
    const double dpos[] = { (double) pos[0], (double) pos[1] };

    matrix_vector_mult(mat, dpos, ret, 2, 2);
}

/*
 * calculates a shearing operation of pixel at pos by angle
 */
void yshear(short *pos, double angle)
{

}


#ifdef NOT_USED
void xshear(short shear, short width, short height)
{
    short skew;
    short skewi;
    short skewf;
    short pixel;
    short x, y;
    short oleft, left;

    short P[20][20];

    for (y= 0; y < height; y++)
    {
        skew = shear * (y + 0.5);
        skewi = floor(skew);
        skewf = frac(skew);  /* = skew - skewi */
        oleft = 0;
        for (x = 0; x < width; x++)
        {
            pixel = P(width - x, y);
            left = pixmult(pixel, skewf);
            /* pixel - left = right */
            pixel = pixel - left + oleft;
            P[width - x + skewi][y] = pixel;
            oleft = left;
        }
        P[skewi][y] = oleft;
    }
}
#endif /* NOT_USED */
