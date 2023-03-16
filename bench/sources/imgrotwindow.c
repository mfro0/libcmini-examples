#include "window.h"
#include "global.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#include "imgrotwindow.h"
#include "bench.h"
#include "imgtools.h"

//#define DEBUG
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
    short angle;
    struct image *integral_images[4];
};

static void timer_imgrotwindow(struct window *wi);
static void delete_imgrotwindow(struct window *wi);
static void draw_imgrotwindow(struct window *wi, short wx, short wy, short ww, short wh);

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

        iw = calloc(1, sizeof(struct imgrotwindow));

        if (iw != NULL)
        {
            wi->priv = iw;
            iw->angle = 0;
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

        dbg("dlg[COLICON].ob_type=%s\n", object_type(icon_tree[COLICON].ob_type));
        dbg("iconblk->mainlist=%p\n", iconblk->mainlist);
        dbg("iconblk->monoblk=%p\n", iconblk->monoblk);

        
        MFDB src_mfdb;
        
        if (gl_nplanes == 1)
        {
            MFDB mfdb =
            {
                .fd_nplanes = 1,
                .fd_addr = iconblk->monoblk.ib_pdata,
                .fd_w = iconblk->monoblk.ib_wicon,
                .fd_h = iconblk->monoblk.ib_hicon,
                .fd_wdwidth = (iconblk->monoblk.ib_wicon + 15) / sizeof(short) / 8,
                .fd_stand = 0,
                .fd_r1 = 0,
                .fd_r2 = 0,
                .fd_r2 = 0,
                .fd_r3 = 0
            };
            src_mfdb = mfdb;
        }
        else
        {
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
            
            if (iconblk == NULL || iconblk->mainlist == NULL)
            {
                form_alert(1, "[2][Could not get color icon][CANCEL]");
                wi->del(wi);
                
                return NULL;
            }
            
            
            MFDB mfdb =
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
            src_mfdb = mfdb;
            
            dbg("src_mfdb.fd_nplanes = %d\n", src_mfdb.fd_nplanes);
            dbg("src_mfdb.fd_addr = %p\n", src_mfdb.fd_addr);
        }
        iw->integral_images[0] = create_image_mfdb(&src_mfdb);

        for (int i = 1; i < 4; i++)
            iw->integral_images[i] = integral_rotate_image(wi, iw->integral_images[0], i);

        do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);


    }

    return wi;
}

static void delete_imgrotwindow(struct window *wi)
{
    /* free window-private memory */
    if (wi && wi->priv)
    {
        struct imgrotwindow *iw = (struct imgrotwindow *) wi->priv;

        for (int i = 0; i < 4; i++)
            if (iw->integral_images[i] != NULL)
                delete_image(iw->integral_images[i]);
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
    short vh = wi->vdi_handle;
    struct imgrotwindow *iw = (struct imgrotwindow *) wi->priv;
    
    (void) wx; (void) wy; (void) ww; (void) wh;
    
    MFDB dst = { 0 };
    
    for (int i = 0; i < 4; i++)
    {
        dbg("integral_images[%d]->mfdb.fd_nplanes = %d\n", i, get_mfdb(iw->integral_images[i])->fd_nplanes);
        dbg("integral_images[%d]->mfdb.fd_w = %d\n", i, get_mfdb(iw->integral_images[i])->fd_w);
        dbg("integral_images[%d]->mfdb.fd_h = %d\n", i, get_mfdb(iw->integral_images[i])->fd_h);
        dbg("integral_images[%d]->mfdb.fd_wdwidth = %d\n", i, get_mfdb(iw->integral_images[i])->fd_wdwidth);
    }
    
    struct image *new_image = shear_rotate_image(wi, iw->integral_images, iw->angle);
    
    /* first, clear the window */
    if (wi->clear) wi->clear(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
    
    /* draw our (possibly rotated) icon */
    
    if (new_image)
    {
        short wicon = get_mfdb(new_image)->fd_w;
        short hicon = get_mfdb(new_image)->fd_h;
        
        dbg("wicon=%d, hicon=%d\r\n", wicon, hicon);
        
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
                  get_mfdb(new_image), &dst);
        delete_image(new_image);
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

