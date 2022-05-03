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

/*
 * local function prototypes
 */
static void draw_fontwindow(struct window *wi, short wx, short wy, short wh, short ww);
static void delete_fontwindow(struct window *wi);

static bool gdos_available;

void init_fontwindow(struct window *wi)
{
    struct fontwindow *fi = wi->priv;

    dbg("\n");
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
        fi->font_info = malloc(sizeof(struct finfo) * (fi->add_fonts + gl_nsysfonts));

        if (fi->font_info == NULL)
        {
            form_alert(2, "[1][could not get font info][OK]");
            exit(1);
        }

        for (i = 0; i < fi->add_fonts + gl_nsysfonts; i++)
        {
            fi->font_info[i].font_id = vqt_name(wi->vdi_handle, i, fi->font_info[i].font_name);
            fi->font_info[i].font_name[32] = '\0';

            dbg("font %d (index %d)=\"%s\"\n", i, fi->font_info[i].font_id, fi->font_info[i].font_name);
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

    /* call base 'class' method */
    wi = create_window(wi_kind, title);

    if (wi)
    {
        wi->wclass = FONTWINDOW_CLASS;

        /* override base methods */
        wi->draw = draw_fontwindow;
        wi->del = delete_fontwindow;
        wi->del = delete_fontwindow;

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

    wi->scroll(wi);
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

static void draw_fontwindow(struct window *wi, short x, short y, short w, short h)
{
    short vh = wi->vdi_handle;
    struct fontwindow *fw = wi->priv;
    short i;
    short d;
    short hslpos, vslpos;
    short xoffs, yoffs;

    short wx = wi->work.g_x;
    short wy = wi->work.g_y;

    dbg("\n");

    /*
     * determine scroll offsets
     */
    wind_get_int(wi->handle, WF_HSLIDE, &hslpos);
    wind_get_int(wi->handle, WF_VSLIDE, &vslpos);

    xoffs = (long) (wi->doc_width - wi->work.g_w) * hslpos / 1000;

    yoffs = (long) (wi->doc_height - wi->work.g_h) * vslpos / 1000;

    init_fontwindow(wi);
    wi->clear(wi, x, y, w, h);
    vst_color(vh, G_BLACK);

    for (i = 0; i < fw->add_fonts + gl_nsysfonts; i++)
    {
        short ch_w, ch_h, ce_w, ce_h;
        short hor, vert;
        short fnt_extent[8];
        short max_str_width = 0;
        short str_height = 0;
        char fntstr[128];

        vst_alignment(vh, TA_LEFT, TA_TOP, &hor, &vert);
        if (hor != TA_LEFT || vert != TA_TOP)
            dbg("did not get alignment we were asking for: hor (should be %d) = %d\n,"
                "vert (should be %d) = %d\n", TA_LEFT, hor, TA_TOP, vert);
        vst_font(vh, fw->font_info[i].font_id);
        vst_height(vh, 16, &ch_w, &ch_h, &ce_w, &ce_h);

        strcpy(fntstr, fw->font_info[i].font_name);
        strcat(fntstr, " (font id=");
        itoa(fw->font_info[i].font_id, &fntstr[strlen(fntstr)], 10);
        strcat(fntstr, ")");

        vqt_extent(vh, fntstr, fnt_extent);

        /* save new doc width if larger than set value */
        max_str_width = fnt_extent[2] - fnt_extent[0];
        wi->doc_width = max_str_width > wi->doc_width ? max_str_width : wi->doc_width;

        str_height = fnt_extent[5] - fnt_extent[1];

        /*
         * small performance improvement:
         * draw text if inside window work area, otherwise just add up text height to
         * calculate the correct dimensions of the canvas in order to set scrollbars right
         */
        if (wy - yoffs + str_height >= wi->work.g_y && wy - yoffs <= wi->work.g_y + wi->work.g_h - 1)
        {
            v_ftext(vh, wx - xoffs, wy - yoffs, fntstr);
        }
        wy += str_height;
    }
    wi->doc_height = wy - wi->work.g_y;
    wi->scroll(wi);
}
