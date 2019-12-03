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

#define DEBUG
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

static bool gdos_available;
static short add_fonts = 0;

void init_fontwindow(struct window *wi)
{
    static bool initialized = false;

    if (!initialized)
    {
        int i;

        initialized = true;
        if (!vq_gdos())
        {
            form_alert(2, "[1][No GDOS installed][OK]");
            gdos_available = 0;
        }
        else
        {
            add_fonts = vst_load_fonts(wi->vdi_handle, 0);
            dbg("loaded %d additional fonts\n", add_fonts);
            gdos_available = 1;
        }

        for (i = 0; i < add_fonts + 2; i++)
        {
            char name[32];

            vqt_name(vdi_handle, i, name);
            dbg("font %d=\"%s\"\n", i, name);
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

        /* extend base class */
        fw = malloc(sizeof(struct fontwindow));
        wi->priv = fw;

        fw->bg_buffer = NULL;

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

    cw->bg_buffer = malloc((wi->work.g_w + 15) / 16 * wi->work.g_h * gl_nplanes * sizeof(short));
    if (!cw->bg_buffer)
        exit(1);

    graf_mouse(M_OFF, NULL);
    wind_update(BEG_UPDATE);

    set_clipping(vh, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h, 1);
    clear_window(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);

    // draw_face(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);

    // vro_cpyfm(vh, S_ONLY, pxy, &mfdb_src, &mfdb_dst);

    wind_update(END_UPDATE);
    graf_mouse(M_ON, NULL);
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

    graf_mouse(M_OFF, NULL);
    wind_update(BEG_UPDATE);

    init_fontwindow(wi);
    wi->clear(wi, x, y, w, h);

    for (i = 0; i < add_fonts; i++)
    {
        char name[32];
        short ch_w, ch_h, ce_w, ce_h;
        short fntindex;

        fntindex = vqt_name(vh, i, name);
        vst_height(vh, 12, &ch_w, &ch_h, &ce_w, &ce_h);
        y += ce_h;
        vst_font(vh, fntindex);
        v_gtext(vh, x, y, name);
        if (y > h)
            break;
    }
    // vro_cpyfm(vh, S_ONLY, pxy, &mfdb_src, &mfdb_dst);

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

