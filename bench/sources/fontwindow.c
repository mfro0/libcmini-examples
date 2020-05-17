#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "window.h"
#include "global.h"
#include "fontwindow.h"
#include <limits.h>
#include "intmath.h"
#include "util.h"

#include <gemx.h>

// #define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

/*
 * local function prototypes
 */
static void draw_fontwindow(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_fontwindow(struct window *wi);
static void delete_fontwindow(struct window *wi);
static void open_fontwindow(struct window *wi, short x, short y, short w, short h);
static void delete_fontwindow(struct window *wi);
static void size_fontwindow(struct window *wi, short x, short y, short w, short h);
static void full_fontwindow(struct window *wi);
static void scroll_fontwindow(struct window *wi);

static bool gdos_available;

void init_fontwindow(struct window *wi)
{
    struct fontwindow *fi = wi->priv;

    if (! fi->gdos_initialised)
    {
        int i;

        fi->gdos_initialised = true;
        if (!vq_gdos())
        {
            form_alert(2, "[1][No GDOS installed][OK]");
            gdos_available = 0;
        }
        else
        {
            fi->add_fonts = vst_load_fonts(wi->vdi_handle, 0);
            dbg("loaded %d additional fonts\n", fi->add_fonts);
            fi->gdos_available = 1;
        }

        if (fi->font_info != NULL)
            free(fi->font_info);
        fi->font_info = malloc(sizeof(struct finfo) * (fi->add_fonts + 2));

        if (fi->font_info == NULL)
        {
            /* TODO: add error message */
            exit(1);
        }

        for (i = 0; i < fi->add_fonts + 2; i++)
        {
            fi->font_info[i].font_index = vqt_name(vdi_handle, i, fi->font_info[i].font_name);
            fi->font_info[i].font_name[32] = '\0';

            dbg("font %d (index %d)=\"%s\"\n", i, fi->font_info[i].font_index, fi->font_info[i].font_name);
        }
    }
}

/*
 * create a new window and add it to the window list.
 */
struct window *create_fontwindow(short wi_kind, char *title)
{
    struct window *wi;
    struct fontwindow *fw;

    /* call base 'class' constructor */
    wi = create_window(wi_kind, title);

    if (wi)
    {
        wi->wclass = FONTWINDOW_CLASS;

        /* override base methods */
        wi->draw = draw_fontwindow;
        wi->del = delete_fontwindow;
        // wi->timer = timer_fontwindow;
        wi->opn = open_fontwindow;
        wi->del = delete_fontwindow;
        wi->size = size_fontwindow;
        wi->full = full_fontwindow;
        wi->scroll = scroll_fontwindow;

        /* extend base class */
        fw = malloc(sizeof(struct fontwindow));
        wi->priv = fw;

        fw->font_info = NULL;
        fw->gdos_initialised = false;
        fw->gdos_available = false;

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = 1;
        wi->y_fac = 1;
    }

    return wi;
}

static void delete_fontwindow(struct window *wi)
{
    short vh = wi->vdi_handle;
    struct fontwindow *fw = wi->priv;

    v_clsvwk(vh);

    if (fw->font_info != NULL)
        free(fw->font_info);
    if (wi->priv) free(wi->priv);

    /* let the generic window code do the rest */

    delete_window(wi);
}


static void open_fontwindow(struct window *wi, short x, short y, short w, short h)
{
    open_window(wi, x, y, w, h);    /* call "base class method" */

    graf_mouse(M_OFF, NULL);
    wind_update(BEG_UPDATE);


    // vro_cpyfm(vh, S_ONLY, pxy, &mfdb_src, &mfdb_dst);

    wind_update(END_UPDATE);
    graf_mouse(M_ON, NULL);
}

/*
 * scroll window contents according to its slider settings
 */
static void scroll_fontwindow(struct window *wi)
{
    short sl_vpos;
    short sl_hpos;
    short sl_hsz;
    short sl_vsz;
    short ret;
    short value;

    scroll_window(wi);

    sl_hsz = 1000L * wi->work.g_w / wi->doc_width;
    sl_hsz = sl_hsz > 1000 ? 1000 : sl_hsz;

    sl_vsz = 1000L * wi->work.g_h / wi->doc_height;
    sl_vsz = sl_vsz > 1000 ? 1000 : sl_vsz;

    dbg("sl_hsz = %d, sl_vsz = %d\r\n", sl_hsz, sl_vsz);


    sl_vpos = 1000L * wi->left / (wi->doc_width - wi->work.g_w);
    sl_vpos = sl_vpos > 1000 ? 1000 : sl_vpos;
    sl_vpos = sl_vpos < 0 ? 0 : sl_vpos;

    sl_hpos = 1000L * wi->top / (wi->doc_height - wi->work.g_h);
    sl_hpos = sl_hpos > 1000 ? 1000 : sl_hpos;
    sl_hpos = sl_hpos < 0 ? 0 : sl_hpos;
    dbg("sl_vpos=%d, sl_hpos=%d\r\n", sl_vpos, sl_hpos);

    /* wind_set is a costly operation. Set values only if we need to */
    wind_get(wi->handle, WF_HSLIDE, &value, &ret, &ret, &ret);
    if (value != sl_vpos)
        wind_set(wi->handle, WF_HSLIDE, sl_vpos, 0, 0, 0);

    wind_get(wi->handle, WF_VSLIDE, &value, &ret, &ret, &ret);
    if (value != sl_hpos)
        wind_set(wi->handle, WF_VSLIDE, sl_hpos, 0, 0, 0);

    wind_get(wi->handle, WF_HSLSIZE, &value, &ret, &ret, &ret);
    if (value != sl_hsz)
        wind_set(wi->handle, WF_HSLSIZE, sl_hsz, 0, 0, 0);

    wind_get(wi->handle, WF_VSLSIZE, &value, &ret, &ret, &ret);
    if (value != sl_vsz)
        wind_set(wi->handle, WF_VSLSIZE, sl_vsz, 0, 0, 0);
}

/*
 * beware: the coordinates passed are the *requested* size of the window frame,
 * *not* the size of the work area.
 * If calling the base class method, these coordinates might even be totally meaningless (as
 * the function might have decided to deny or change the requested size). The rect member
 * of the window should have the correct coordinates, however.
 */
static void size_fontwindow(struct window *wi, short x, short y, short w, short h)
{
    struct fontwindow *cw = wi->priv;
    short vh = wi->vdi_handle;

    size_window(wi, x, y, w, h);
}

void full_fontwindow(struct window *wi)
{
    full_window(wi);

    size_fontwindow(wi, wi->rect.g_x, wi->rect.g_y, wi->rect.g_w, wi->rect.g_h);
}

static void draw_fontwindow(struct window *wi, short x, short y, short w, short h)
{
    short vh = wi->vdi_handle;
    struct fontwindow *fw = wi->priv;
    short i;
    short d;
    short hslpos, vslpos, hslsiz, vslsiz;
    short xoffs, yoffs;

    short wx = wi->work.g_x;
    short wy = wi->work.g_y;

    /*
     * determine scroll offsets
     */
    wind_get(wi->handle, WF_HSLIDE, &hslpos, &d, &d, &d);
    wind_get(wi->handle, WF_VSLIDE, &vslpos, &d, &d, &d);
    wind_get(wi->handle, WF_HSLSIZE, &hslsiz, &d, &d, &d);
    wind_get(wi->handle, WF_VSLSIZE, &vslsiz, &d, &d, &d);

    xoffs = (long) (wi->doc_width - wi->work.g_w) * hslpos / 1000;

    yoffs = (long) (wi->doc_height - wi->work.g_h) * vslpos / 1000;

    graf_mouse(M_OFF, NULL);
    wind_update(BEG_UPDATE);

    init_fontwindow(wi);
    wi->clear(wi, x, y, w, h);
    vst_color(vh, G_BLACK);

    for (i = 0; i < fw->add_fonts + 2; i++)
    {
        short ch_w, ch_h, ce_w, ce_h;
        short hor, vert;
        short fnt_extend[8];

        vst_alignment(vh, TA_LEFT, TA_TOP, &hor, &vert);
        if (hor != TA_LEFT || vert != TA_TOP)
            dbg("did not get alignment we were asking for: hor (should be %d) = %d\n,"
                "vert (should be %d) = %d\n", TA_LEFT, hor, TA_TOP, vert);
        vst_height(vh, 12, &ch_w, &ch_h, &ce_w, &ce_h);
        vst_font(vh, fw->font_info[i].font_index);
        vqt_extent(vh, fw->font_info[i].font_name, fnt_extend);

        /* save new doc width if larger than set value */
        wi->doc_width = fnt_extend[2] - fnt_extend[0] > wi->doc_width ? fnt_extend[2] - fnt_extend[0] : wi->doc_width;

        if (wy - yoffs + (fnt_extend[5] - fnt_extend[1]) >= wi->work.g_y && wy - yoffs <= wi->work.g_y + wi->work.g_h - 1)
            v_ftext(vh, wx - xoffs, wy - yoffs, fw->font_info[i].font_name);
        wy += fnt_extend[5] - fnt_extend[1];
    }
    wi->doc_height = wy - wi->work.g_y;
    wi->scroll(wi);

    wind_update(END_UPDATE);
    graf_mouse(M_ON, NULL);
}


/*
 * react on timer events
 */
static void timer_fontwindow(struct window *wi)
{
    struct fontwindow *fw = wi->priv;

    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
}

