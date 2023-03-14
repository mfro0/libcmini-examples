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
#include <assert.h>

// #define DEBUG
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

static struct image *integral_rotate_image(struct window *wi, struct image *src, short rotations);
static struct image *y_shear(struct window *wi, struct image *src, short shear_y);
static struct image *x_shear(struct window *wi, struct image *src, short shear_x);
static struct image *shear_rotate_image(struct window *wi, struct image *src, short angle);
static struct image *create_image_mfdb(MFDB *mfdb);
static void delete_image(struct image *image);

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
            form_alert(1, "[1][could not allocate imgrotate private data][CANCEL]");
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

        // dbg("dlg[COLICON].ob_type=%s\n", object_type(icon_tree[COLICON].ob_type));
        // dbg("iconblk->mainlist=%p\n", iconblk->mainlist);
        // ("iconblk->monoblk=%p\n", iconblk->monoblk);

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

        // dbg("src_mfdb.fd_nplanes = %d\n", src_mfdb.fd_nplanes);
        // dbg("src_mfdb.fd_addr = %p\n", src_mfdb.fd_addr);

        MFDB dst_mfdb = src_mfdb;

        short new_width = src_mfdb.fd_w;
        short new_height = src_mfdb.fd_h;

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
        // dbg("allocated new address at %p, new dimension=%d,%d\n", dst_mfdb.fd_addr, dst_mfdb.fd_w, dst_mfdb.fd_h);

        short pxy[8] = { 0, 0, new_width - 1, new_height - 1, 0, 0, new_width - 1, new_height - 1 };

        vro_cpyfm(vh, dst_mfdb.fd_nplanes <= 8 ? ALL_WHITE : ALL_BLACK, pxy, NULL, &dst_mfdb);

        pxy[2] = src_mfdb.fd_w - 1;
        pxy[3] = src_mfdb.fd_h - 1;
        pxy[4] = (new_width - src_mfdb.fd_w) / 2;
        pxy[5] = (new_height - src_mfdb.fd_h) / 2;
        pxy[6] = dst_mfdb.fd_w - pxy[4] - 1;
        pxy[7] = dst_mfdb.fd_h - pxy[5] - 1;

        // dbg("copy from (%d,%d)(%d,%d) to (%d,%d)(%d,%d)\n", pxy[0], pxy[1], pxy[2], pxy[3],
        //                                                    pxy[4], pxy[5], pxy[6], pxy[7]);

        vro_cpyfm(vh, S_ONLY, pxy, &src_mfdb, &dst_mfdb);
        if (iw->iconblk == NULL)
        {
            form_alert(1, "[1][No suitable color icon found][CANCEL]");
            wi->del(wi);

            return NULL;
        }

        iw->image_mfdb = dst_mfdb;

        // dbg("finished\n");

    }

    return wi;
}

static void delete_imgrotwindow(struct window *wi)
{
    // dbg("\n");
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

struct image {  
    MFDB mfdb;
    short imgdata[];
};

/*
 * draw window
 */
static void draw_imgrotwindow(struct window *wi, short wx, short wy, short ww, short wh)
{
    short vh = wi->vdi_handle;
    struct imgrotwindow *iw = (struct imgrotwindow *) wi->priv;
    struct image *img;
    
    (void) wx; (void) wy; (void) ww; (void) wh;
    
    // dbg("\n");
    
    MFDB dst = { 0 };
    
    img = create_image_mfdb(&iw->image_mfdb);
    if (img != NULL)
    {
        struct image *new_image = shear_rotate_image(wi, img, iw->angle);
        
        /* first, clear the window */
        if (wi->clear) wi->clear(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
        
        /* draw our (possibly rotated) icon */
        
        // dbg("new_image = %p\n", new_image);
        if (new_image)
        {
            short wicon = new_image->mfdb.fd_w;
            short hicon = new_image->mfdb.fd_h;
            
            // dbg("wicon=%d, hicon=%d\r\n", wicon, hicon);
            
            /*
             * center icon into window's work area
             */
            short pxy[8] =
            {
                0, 0, wicon - 1, hicon - 1,
                wi->work.g_x + wi->work.g_w / 2 - wicon / 2,
                wi->work.g_y + wi->work.g_h / 2 - hicon / 2,
                wi->work.g_x + wi->work.g_w / 2 + wicon / 2 - 1,
                wi->work.g_y + wi->work.g_h / 2 + hicon / 2 - 1
            };
            
            vro_cpyfm(vh, S_ONLY,
                      pxy,
                      &new_image->mfdb, &dst);
            delete_image(new_image);
        }
        delete_image(img);
    }
}


/*
 * react on timer events
 */
static void timer_imgrotwindow(struct window *wi)
{
    struct imgrotwindow *iw = wi->priv;
    static int timer_count = 0;

    //timer_count++;
    //timer_count %= 5;

    if (iw != NULL)
    {
        //if (timer_count == 4)
        {
            do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
            iw->angle += 50;
            //if (iw->angle > 450)
            //    iw->angle = -450;
            iw->angle %= 3600;

            dbg("iw->angle = %d\n", iw->angle);
        }
    }
}

static struct image *create_image_like(struct image *like_image)
{
    struct image *new_image;
    
    new_image = calloc(1, sizeof(struct image) + 
                       (size_t) like_image->mfdb.fd_wdwidth * like_image->mfdb.fd_h * like_image->mfdb.fd_h);
    if (new_image != NULL)
    {
        new_image->mfdb = like_image->mfdb;
        new_image->mfdb.fd_addr = new_image->imgdata;
    }

    assert(new_image != NULL);
    return new_image;
}

/*
 * create a struct image in memory (empty) from width, height and number of planes
 * The image is in device specific format (fd_stand = 0)
 */
static struct image *create_image_whp(short width, short height, short nplanes)
{
    struct image *new_image;
    short wdwidth = (width + 15) / 16;
    
    dbg("width=%d, height=%d, nplanes=%d\r\n", width, height, nplanes);
    
    new_image = calloc(1, sizeof(struct image) +
                       (size_t) wdwidth * height * nplanes * sizeof(short));
    if (new_image != NULL)
    {
        new_image->mfdb.fd_addr = new_image->imgdata;
        new_image->mfdb.fd_wdwidth = wdwidth;
        new_image->mfdb.fd_nplanes = nplanes;
        new_image->mfdb.fd_h = height;
        new_image->mfdb.fd_w = width;
        new_image->mfdb.fd_stand = 0;
    }
    assert(new_image != NULL);
    
    return new_image;
}

static struct image *create_image_mfdb(MFDB *mfdb)
{
    struct image *new_image;
    new_image = calloc(1, sizeof(struct image) +
                       (size_t) mfdb->fd_wdwidth * mfdb->fd_h * mfdb->fd_nplanes * sizeof(short));
    if (new_image != NULL)
    {
        new_image->mfdb = *mfdb;
        memcpy(new_image->imgdata, mfdb->fd_addr, mfdb->fd_wdwidth * mfdb->fd_h * mfdb->fd_nplanes * sizeof(short));
    }
    assert(new_image != NULL);
    return new_image;
}

static void delete_image(struct image *image)
{
    free(image);
}

/*
 * return an image that is rotated <rotations> * 90 degrees
 */
static struct image *integral_rotate_image(struct window *wi, struct image *src, short rotations)
{
    struct image *dst = rotations & 1 ? create_image_whp(src->mfdb.fd_h, src->mfdb.fd_w, src->mfdb.fd_nplanes) :
                                        create_image_like(src);
    short vh = wi->vdi_handle;

    if (dst)
    {
        short pxy[8];
        
        assert(dst != NULL);
        
        switch (rotations)
        {
            case 0:
            default:
                pxy[0] = pxy[1] = pxy[4] = pxy[5] = 0;
                pxy[2] = pxy[6] = src->mfdb.fd_w - 1;
                pxy[3] = pxy[7] = src->mfdb.fd_h - 1;
                vro_cpyfm(vh, S_ONLY, pxy, &src->mfdb, &dst->mfdb);
                break;
                
            case 1: /* 90 degrees counter clockwise */
                /* swap rows and reverse columns */
                //dst->mfdb.fd_w = src->mfdb.fd_h;
                //dst->mfdb.fd_h = src->mfdb.fd_w;
                //dst->mfdb.fd_wdwidth = (dst->mfdb.fd_w + 15) / 16;
                for (short y = 0; y < src->mfdb.fd_h; y++)   /* rows */
                {
                    for (short x = 0; x < src->mfdb.fd_w; x++) /* columns */
                    {
                        pxy[0] = x;
                        pxy[1] = y;
                        pxy[2] = x;
                        pxy[3] = y;
                        
                        pxy[4] = y;
                        pxy[5] = dst->mfdb.fd_h - x - 1;
                        pxy[6] = y;
                        pxy[7] = dst->mfdb.fd_h - x - 1;
                        
                        vro_cpyfm(vh, S_ONLY, pxy, &src->mfdb, &dst->mfdb);
                    }
                }
                break;
                
            case 2:
                /* 180 degrees counter clockwise (aka upside-down) */
                for (short y = 0; y < src->mfdb.fd_h; y++)
                {
                    for (short x = 0; x < src->mfdb.fd_w; x++)
                    {
                        pxy[0] = x;
                        pxy[1] = y;
                        pxy[2] = x;
                        pxy[3] = y;
                        
                        pxy[4] = dst->mfdb.fd_w - x - 1;
                        pxy[5] = dst->mfdb.fd_h - y - 1;
                        pxy[6] = dst->mfdb.fd_w - x - 1;
                        pxy[7] = dst->mfdb.fd_h - y - 1;
                        
                        vro_cpyfm(vh, S_ONLY, pxy, &src->mfdb, &dst->mfdb);
                    }
                }
                break;
                
            case 3:
                /* 270 degrees counter clockwise */
                //dst->mfdb.fd_w = src->mfdb.fd_h;
                //dst->mfdb.fd_h = src->mfdb.fd_w;
                //dst->mfdb.fd_wdwidth = (dst->mfdb.fd_w + 15) / 16;
                for (short y = 0; y < src->mfdb.fd_h; y++)
                {
                    for (short x = 0; x < src->mfdb.fd_w; x++)
                    {
                        pxy[0] = x;
                        pxy[1] = y;
                        pxy[2] = x;
                        pxy[3] = y;
                        
                        pxy[4] = dst->mfdb.fd_w - 1 - y;
                        pxy[5] = x;
                        pxy[6] = dst->mfdb.fd_w - 1 - y;
                        pxy[7] = x;
                        
                        vro_cpyfm(vh, S_ONLY, pxy, &src->mfdb, &dst->mfdb);
                    }
                }
                break;
        }
        return dst;
    }
    return NULL;
}

static struct image *shear_rotate_image(struct window *wi, struct image *src, short angle)
{
    short n_90rot;
    short rest_angle;
    struct image *integral_img;
    struct image *x_sheared_img, *y_sheared_img;

    /*
     * adjust rotation angle so that we need to shear-rotate by a maximum amount of +/- 45°
     */
    rest_angle = angle % 3600;
    n_90rot = rest_angle / 900;
    rest_angle = rest_angle % 900;
    if (rest_angle > 450)
    {
        rest_angle = rest_angle - 900;
        n_90rot = (n_90rot + 1) % 4;
    }
    
    integral_img = integral_rotate_image(wi, src, n_90rot);

    if (integral_img == NULL)
    {
        form_alert(1, "[1][could not create integral image][OK]");
        return NULL;
    }

    dbg("angle=%d, n_90rot=%d, rest_angle=%d\r\n", angle, n_90rot, rest_angle);
    /*
     * make sure to call the simple trigonometric functions with positive angle only
     */
    //short shear_x = rest_angle < 0 ? -itan(-rest_angle / 2) : -itan(rest_angle / 2);
    //short shear_y = rest_angle < 0 ? isin(-rest_angle) : isin(rest_angle);
    
    short shear_x = (short) (-tan(rest_angle / 10.0 / 2.0 * M_PI / 180.0) * SHRT_MAX);
    short shear_y = (short) (sin(rest_angle / 10.0 * M_PI / 180.0) * SHRT_MAX);
    
    if (shear_x == 0 && shear_y == 0)
        return integral_img;

    x_sheared_img = x_shear(wi, integral_img, shear_x);
    free(integral_img);

    y_sheared_img = y_shear(wi, x_sheared_img, shear_y);
    free(x_sheared_img);
            
    x_sheared_img = x_shear(wi, y_sheared_img, shear_x);
    return x_sheared_img;
}

#undef DEBUG
/*
 * shear image in x direction.
 * shear_x (the tangent of the shear angle) is given in units of 1/SHRT_MAX.
 */
static struct image *x_shear(struct window *wi, struct image *src, short shear_x)
{
    short winc = (short) abs((long) src->mfdb.fd_h * shear_x / SHRT_MAX);
    dbg("winc=%d\r\n", winc);
    
    short new_width = src->mfdb.fd_w + winc;
    short new_height = src->mfdb.fd_h;
    struct image *sheared = create_image_whp(new_width, new_height, src->mfdb.fd_nplanes);
    short pxy[8] = {0, 0, new_width - 1, new_height - 1, 0, 0, new_width - 1, new_height - 1 };
    
    assert(sheared != NULL);
    
    vro_cpyfm(wi->vdi_handle, sheared->mfdb.fd_nplanes <= 8 ? ALL_WHITE : ALL_BLACK, pxy, &src->mfdb, &sheared->mfdb);
    
    if (sheared != NULL)
    {
        for (int i = 0; i < src->mfdb.fd_h; i++)
        {
            
            short skew = shear_x > 0 ?
                             (short)((long) i * (long) shear_x / SHRT_MAX) :
                             -(short)((long) (src->mfdb.fd_h - i - 1) * (long) shear_x / SHRT_MAX);
            short left = 0;
            short right = src->mfdb.fd_w - 1;
            
            pxy[0] = left;
            pxy[1] = i;
            pxy[2] = right;
            pxy[3] = i;
            
            pxy[4] = winc - skew;
            pxy[5] = i;
            pxy[6] = pxy[4] + src->mfdb.fd_w - 1;
            pxy[7] = i;
            vro_cpyfm(wi->vdi_handle, S_ONLY, pxy, &src->mfdb, &sheared->mfdb);
        }
    }
    return sheared;
}

static struct image *y_shear(struct window *wi, struct image *src, short shear_y)
{    
    short hinc = (short) abs((long) src->mfdb.fd_w * shear_y / SHRT_MAX);
    dbg("hinc=%d\r\n", hinc);
    
    short new_width = src->mfdb.fd_w;
    short new_height = src->mfdb.fd_h + hinc;
    struct image *sheared = create_image_whp(new_width, new_height, src->mfdb.fd_nplanes);
    short pxy[8] = {0, 0, new_width - 1, new_height - 1, 0, 0, new_width - 1, new_height - 1 };
    
    assert(sheared != NULL);
    
    vro_cpyfm(wi->vdi_handle, sheared->mfdb.fd_nplanes <= 8 ? ALL_WHITE : ALL_BLACK, pxy, &src->mfdb, &sheared->mfdb);
    
    if (sheared != NULL)
    {
        for (int i = 0; i < src->mfdb.fd_w; i++)
        {
            
            short skew = shear_y > 0 ? (short)((long) i * (long) shear_y / SHRT_MAX) :
                                       -(short)((long) (src->mfdb.fd_w - i - 1) * (long) shear_y / SHRT_MAX);
            short top = 0;
            short bottom = src->mfdb.fd_h - 1;
            
            pxy[0] = i;
            pxy[1] = top;
            pxy[2] = i;
            pxy[3] = bottom;
            
            
            pxy[4] = i;
            pxy[5] = hinc - skew; 
            pxy[6] = i;
            pxy[7] = pxy[5] + src->mfdb.fd_h - 1;
            dbg("pxy[] = {%d, %d, %d, %d, %d, %d, %d, %d}\r\n",
                pxy[0], pxy[1], pxy[2], pxy[3], pxy[4], pxy[5], pxy[6], pxy[7]);
            vro_cpyfm(wi->vdi_handle, S_ONLY, pxy, &src->mfdb, &sheared->mfdb);
        }
    }
    return sheared;
}

