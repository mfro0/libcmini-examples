#include "window.h"
#include "global.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "imgrotwindow.h"
#include "intmath.h"
#include "bench.h"
#include <math.h>

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

/* private data for this window type */
struct imgrotwindow
{
    CICONBLK *iconblk;
    short angle;
    short *bitmap;
    MFDB image_mfdb;
};

static void timer_imgrotwindow(struct window *wi);
static void delete_imgrotwindow(struct window *wi);
static void draw_imgrotwindow(struct window *wi, short wx, short wy, short ww, short wh);

static MFDB *integral_rotate_image(struct window *wi, MFDB *src, short rotations);
static void y_shear(struct window *wi, MFDB *src, short shear_y);
static void x_shear(struct window *wi, MFDB *src, short shear_x);
static MFDB *shear_rotate_image(struct window *wi, MFDB *src, short angle);

static char *object_type(short type)
{
    static struct t_text
    {
        short type;
        char *type_text;
    } t[] =
    {
    {   .type = 20, .type_text = "G_BOX" },
    {   .type = 21, .type_text = "G_TEXT" },
    {   .type = 22, .type_text = "G_BOXTEXT" },
    {   .type = 23, .type_text = "G_IMAGE" },
    {   .type = 24, .type_text = "G_USERDEF" },
    {   .type = 25, .type_text = "G_IBOX" },
    {   .type = 26, .type_text = "G_BUTTON" },
    {   .type = 27, .type_text = "G_BOXCHAR" },
    {   .type = 28, .type_text = "G_STRING" },
    {   .type = 29, .type_text = "G_FTEXT" },
    {   .type = 30, .type_text = "G_FBOXTEXT" },
    {   .type = 31, .type_text = "G_ICON" },
    {   .type = 32, .type_text = "G_TITLE" },
    {   .type = 33, .type_text = "G_CICON" },
    {   .type = 34, .type_text = "G_SWBUTTON" },
    {   .type = 35, .type_text = "G_POPUP" },
    {   .type = 36, .type_text = "G_WINTITLE" },
    {   .type = 37, .type_text = "G_EDIT" },
    {   .type = 38, .type_text = "G_SHORTCUT" },
    {   .type = 39, .type_text = "G_SLIST" },
    {   .type = 99, .type_text = "UNKNOWN" }
    };
    int num_t = sizeof(t) / sizeof(struct t_text);
    int i;

    for (i = 0; i < num_t; i++)
    {
        if (type == t[i].type)
            break;
    }
    return t[i].type_text;
}
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
        OBJECT *icon_tree;

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
            form_alert(1, "[1][could not allocate vdiwindow private data][CANCEL]");
            wi->del(wi);

            return NULL;
        }

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */

        if (!rsrc_gaddr(R_TREE, ICNS, &icon_tree))
        {
            form_alert(1, "[2][Could not get ICNS resource][CANCEL]");
            wi->del(wi);
            return NULL;
        }


        CICONBLK *iconblk = icon_tree[COLICON].ob_spec.ciconblk;
        CICON *icon;

        dbg("dlg[COLICON].ob_type=%s\n", object_type(icon_tree[COLICON].ob_type));
        dbg("iconblk->mainlist=%p\n", iconblk->mainlist);
        dbg("iconblk->monoblk=%p\n", iconblk->monoblk);

        icon = iconblk->mainlist;
        if (icon == NULL)
        {
            form_alert(1, "[1][No color icon?][CANCEL]");
            return NULL;
        }
        do
        {
            if (icon->num_planes == gl_nplanes)
                break;
            icon = iconblk->mainlist->next_res;
        } while (icon != NULL);

        if ((iw->iconblk = iconblk) == NULL || (iconblk->mainlist) == NULL)
        {
            form_alert(1, "[2][Could not get color icon][CANCEL]");
            wi->del(wi);

            return NULL;
        }


        MFDB src_mfdb =
        {
            .fd_nplanes = icon->num_planes,
            .fd_addr = icon->col_data,
            .fd_w = iconblk->monoblk.ib_wicon,
            .fd_h = iconblk->monoblk.ib_hicon,
            .fd_wdwidth = (iconblk->monoblk.ib_wicon + 15) / sizeof(short) / 8,
            .fd_stand = 0,
            .fd_r1 = 0,
            .fd_r2 = 0,
            .fd_r3 = 0
        };

        dbg("src_mfdb.fd_nplanes = %d\n", src_mfdb.fd_nplanes);
        dbg("src_mfdb.fd_addr = %p\n", src_mfdb.fd_addr);

        MFDB dst_mfdb = src_mfdb;

        short new_width = (short) ((long) src_mfdb.fd_w * 2000L / 1000L);
        short new_height = (short) ((long) src_mfdb.fd_h * 2000L / 1000L);

        dst_mfdb.fd_w = new_width;
        dst_mfdb.fd_h = new_height;
        dst_mfdb.fd_wdwidth = (new_width + 15) / sizeof(short) / 8;

        if ((dst_mfdb.fd_addr = calloc(1, dst_mfdb.fd_wdwidth *sizeof(short) * dst_mfdb.fd_nplanes
                                       * dst_mfdb.fd_h)) == NULL)
        {
            form_alert(1, "[1][Could not allocate memory for new icon][CANCEL]");
            wi->del(wi);
            return NULL;
        }
        dbg("allocated new address at %p, new dimension=%d,%d\n", dst_mfdb.fd_addr, dst_mfdb.fd_w, dst_mfdb.fd_h);

        short pxy[8] = { 0, 0, new_width - 1, new_height - 1, 0, 0, new_width - 1, new_height - 1 };

        dbg("2\n");
        vro_cpyfm(vh, ALL_BLACK, pxy, NULL, &dst_mfdb);
        dbg("3\n");

        pxy[2] = src_mfdb.fd_w - 1;
        pxy[3] = src_mfdb.fd_h - 1;
        pxy[4] = (new_width - src_mfdb.fd_w) / 2;
        pxy[5] = (new_height - src_mfdb.fd_h) / 2;
        pxy[6] = dst_mfdb.fd_w - pxy[4] - 1;
        pxy[7] = dst_mfdb.fd_h - pxy[5] - 1;

        dbg("copy from (%d,%d)(%d,%d) to (%d,%d)(%d,%d)\n", pxy[0], pxy[1], pxy[2], pxy[3],
                                                            pxy[4], pxy[5], pxy[6], pxy[7]);

        vro_cpyfm(vh, S_ONLY, pxy, &src_mfdb, &dst_mfdb);
        if (iw->iconblk == NULL)
        {
            form_alert(1, "[1][No suitable color icon found][CANCEL]");
            wi->del(wi);

            return NULL;
        }

        iw->image_mfdb = dst_mfdb;

        dbg("finished\n");

    }

    return wi;
}

static void delete_imgrotwindow(struct window *wi)
{
    dbg("\n");
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

    dbg("\n");

    /* get size of window's work area */
    wind_get(wi->handle, WF_WORKXYWH, &x, &y, &w, &h);

    /* first, clear it */
    if (wi->clear) wi->clear(wi, x, y, w, h);

    /* draw our (possibly rotated) icon */
    MFDB dst = { 0 };


    MFDB *new_image = shear_rotate_image(wi, &iw->image_mfdb, iw->angle);

    dbg("new_image = %p\n", new_image);
    if (new_image)
    {
        short wicon = new_image->fd_w;
        short hicon = new_image->fd_h;

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
              new_image, &dst);
        free(new_image->fd_addr);
        free(new_image);
    }
}


/*
 * react on timer events
 */
static void timer_imgrotwindow(struct window *wi)
{
    struct imgrotwindow *iw = wi->priv;
    static int timer_count = 0;

    timer_count++;
    timer_count %= 15;

    if (iw != NULL)
    {
        if (timer_count == 14)
        {
            do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
            iw->angle += 100;
            iw->angle %= 3600;

            dbg("iw->angle = %d\n", iw->angle);
        }
    }
}

/*
 * return an image that is rotated <rotations> * 90 degrees
 */
static MFDB *integral_rotate_image(struct window *wi, MFDB *src, short rotations)
{
    MFDB *dst = calloc(1, sizeof(MFDB));
    short vh = wi->vdi_handle;

    if (dst)
    {
        dst->fd_wdwidth = (src->fd_h + 15) / 16;
        dst->fd_addr = malloc(dst->fd_wdwidth * sizeof(short) * gl_nplanes * src->fd_h);
        dst->fd_w = src->fd_h;
        dst->fd_h = src->fd_w;
        dst->fd_nplanes = gl_nplanes;
        short pxy[8];

        if (dst->fd_addr)
        {
            switch (rotations)
            {
                case 0:
                default:
                    pxy[0] = pxy[1] = pxy[4] = pxy[5] = 0;
                    pxy[2] = pxy[6] = src->fd_w - 1;
                    pxy[3] = pxy[7] = src->fd_h - 1;
                    vro_cpyfm(vh, S_ONLY, pxy, src, dst);
                    break;

                case 1:
                    /* swap rows and columns */
                    for (short i = 0; i < src->fd_w; i++)
                    {
                        for (short j = 0; j < src->fd_h; j++)
                        {
                            pxy[0] = i;
                            pxy[1] = j;
                            pxy[2] = i;
                            pxy[3] = j;
                            pxy[4] = j;
                            pxy[5] = i;
                            pxy[6] = j;
                            pxy[7] = i;

                            vro_cpyfm(vh, S_ONLY, pxy, src, dst);
                        }
                    }
                    break;

                case 2:
                    /* upside down */
                    for (short j = 0; j < src->fd_h; j++)
                    {
                            pxy[0] = 0;
                            pxy[1] = j;
                            pxy[2] = src->fd_w - 1;
                            pxy[3] = j;
                            pxy[4] = 0;
                            pxy[5] = src->fd_h - 1 - j;
                            pxy[6] = src->fd_w - 1;
                            pxy[7] = src->fd_h - 1 - j;

                            vro_cpyfm(vh, S_ONLY, pxy, src, dst);
                    }
                    break;

                case 3:
                    /* x-mirror */
                    for (short i = 0; i < src->fd_w; i++)
                    {
                        for (short j = 0; j < src->fd_h; j++)
                        {
                            pxy[0] = i;
                            pxy[1] = j;
                            pxy[2] = i;
                            pxy[3] = j;
                            pxy[4] = src->fd_h - 1 - j;
                            pxy[5] = src->fd_w - 1 - i;
                            pxy[6] = src->fd_h - 1 - j;
                            pxy[7] = src->fd_w - 1 - i;

                            vro_cpyfm(vh, S_ONLY, pxy, src, dst);
                        }
                    }
                    break;
            }

            return dst;
        }
    }
    return (MFDB *) NULL;
}

static MFDB *shear_rotate_image(struct window *wi, MFDB *src, short angle)
{
    short rotations;
    MFDB *integral_img;

    /*
     * adjust rotation angle so that we need to shear-rotate by a maximum of +/- 45°
     */
    angle = angle % 3600;
    if (angle < -450)
        angle + 3600;
    for (rotations = 0; angle > 450; rotations++)
        angle -= 90;
    integral_img = integral_rotate_image(wi, src, rotations);
    if (integral_img == NULL)
    {
        form_alert(1, "[1][could not create integral image][OK]");
        return NULL;
    }

    short shear_x = -itan(angle);
    short shear_y = isin(angle);

    if (shear_x == 0 && shear_y == 0)
        return integral_img;

    x_shear(wi, integral_img, shear_x);
    y_shear(wi, integral_img, shear_y);
    x_shear(wi, integral_img, shear_x);

    return integral_img;
}

static void x_shear(struct window *wi, MFDB *src, short shear_x)
{
    short pxy[8];

    /* we assume a half width border left and right */
    short border_width = src->fd_w / 2;


    for (int i = 0; i < src->fd_h / 4; i++)
    {

        short mid_x = src->fd_w / 2;
        short width = mid_x;

        short left = mid_x - width / 2;
        short right = left + width - 1;

        short mid_y = src->fd_h / 2;

        dbg("shear_x=%d, shift=%d\n", shear_x, shear_x * i / SHRT_MAX);

        pxy[0] = left;
        pxy[1] = mid_y + i;
        pxy[2] = right;
        pxy[3] = mid_y + i;

        pxy[4] = left - shear_x * i / SHRT_MAX;
        pxy[5] = mid_y + i;
        pxy[6] = pxy[4] + width - 1;
        pxy[7] = mid_y + i;
        vro_cpyfm(wi->vdi_handle, S_ONLY, pxy, src, src);

        pxy[1] = pxy[3] = pxy[5] = pxy[7] = mid_y - i;
        pxy[4] = -pxy[4];
        pxy[6] = pxy[4] + width - 1;
        vro_cpyfm(wi->vdi_handle, S_ONLY, pxy, src, src);
    }
}

static void y_shear(struct window *wi, MFDB *src, short shear_y)
{
}

