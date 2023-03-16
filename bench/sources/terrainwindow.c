#include "window.h"
#include "global.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "terrainwindow.h"

#include <png.h>
#include <mint/cookie.h>

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#define out(format, arg...) do { ; } while (0)
#endif /* DEBUG */


/* private data for this window type */
struct terrainwindow
{
    bool new_turn;
    short color;
    MFDB heightmap;
    MFDB colormap;
};

static void timer_terrainwindow(struct window *wi);
static void delete_terrainwindow(struct window *wi);
static void draw_terrainwindow(struct window *wi, short wx, short wy, short ww, short wh);

static char *color_type_string(int color_type)
{
    switch (color_type)
    {
        case PNG_COLOR_TYPE_GRAY:
            return "PNG_COLOR_TYPE_GRAY";
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            return "PNG_COLOR_TYPE_GRAY_ALPHA";
        case PNG_COLOR_TYPE_PALETTE:
            return "PNG_COLOR_TYPE_PALETTE";
        case PNG_COLOR_TYPE_RGB:
            return "PNG_COLOR_TYPE_RGB";
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return "PNG_COLOR_TYPE_RGB_ALPHA";
        default:
            return "unknown color type";
    }
}
static void rd_png_warning(void)
{
    out("png warning");
}

static void rd_png_error(void)
{
    out("png_error");
}


static MFDB read_png(const char *filename)
{
    unsigned char header[8];
    bool is_png;
    MFDB img = { 0 };

    FILE *fp = fopen(filename, "rb");
    if (!fp) return img;

    fread(header, 1, 8, fp);
    is_png = (!png_sig_cmp(header, 0, 8));

    if (!is_png) return img;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) return img;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return img;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return img;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return img;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);      /* we already have read the header */

    png_read_png(png_ptr, info_ptr, 0, NULL);
    unsigned int width = png_get_image_width(png_ptr, info_ptr);
    unsigned int height = png_get_image_height(png_ptr, info_ptr);
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    dbg("sucessfully read png image \"%s\" with width=%d and height=%d\r\n", filename, width, height);
    dbg("bit_depth=%d, color_type=%s\n", png_get_bit_depth(png_ptr, info_ptr),
        color_type_string(png_get_color_type(png_ptr, info_ptr)));

    png_color *palette;
    int num_palette;

    png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
    dbg("# of channels=%d, palette=%p, # palette entries=%d\n", png_get_channels(png_ptr, info_ptr),
        palette, num_palette);

    img.fd_addr = 0L;
    img.fd_w = (short) width;
    img.fd_h = (short) height;
    img.fd_wdwidth = (width + 15) / 16;
    img.fd_stand = 0;
    img.fd_nplanes = gl_nplanes;
    img.fd_r1 = img.fd_r2 = img.fd_r3 = 0;

    img.fd_addr = malloc(width * height * sizeof(long));
    for (unsigned int i = 0; i < height; i++)
        for (unsigned int j = 0; j < width; j++)
        {
            unsigned char colorindex = row_pointers[i][j];
            png_color c = palette[colorindex];

            unsigned long *pixels32 = img.fd_addr;
            unsigned short *pixels16 = img.fd_addr;
            switch (gl_nplanes)
            {
                case 32:
                    /* 32 bit */
                    pixels32[i * width + j] = c.red << 16 | c.green << 8 | c.blue;

                case 16:
                    /* 16 bit */
                    pixels16[i * width + j] = (c.red >> 3) << 11 | (c.green >> 2) << 5 | (c.blue >> 3);
            }
        }
    return img;
}

/*
 * create a new window and add it to the window list.
 */
struct window *create_terrainwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct terrainwindow *vw;

    dbg("start\r\n");

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = TERRAINWINDOW_CLASS;
        wi->draw = draw_terrainwindow;
        wi->del = delete_terrainwindow;

        wi->word_aligned = true;

        vw = malloc(sizeof(struct terrainwindow));

        if (vw != NULL)
        {
            wi->priv = vw;
            vw->color = 0;
            vw->new_turn = true;

            vw->colormap = read_png("C7W.png");
            vw->heightmap = read_png("D7.png");
        }
        else
        {
            fprintf(stderr, "%s: could not allocate terrainwindow private data\r\n", __FUNCTION__);
            return NULL;
        }

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = vw->colormap.fd_w;
        wi->doc_height = vw->colormap.fd_h;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */
    }
    dbg("finished\r\n");

    /* set sliders */
    if (wi->scroll) wi->scroll(wi);

    return wi;
}

static void delete_terrainwindow(struct window *wi)
{
    /* free window-private memory */
    if (wi && wi->priv)
    {
        struct terrainwindow *vw = (struct terrainwindow *) wi->priv;

        free(vw->colormap.fd_addr);
        free(vw->heightmap.fd_addr);


        free(wi->priv);
    }
    /* let the generic window code do the rest */
    delete_window(wi);
}

/*
 * draw window
 */
static void draw_terrainwindow(struct window *wi, short x, short y, short w, short h)
{
    short vh = wi->vdi_handle;

    struct terrainwindow *vw = (struct terrainwindow *) wi->priv;

    MFDB screen = { 0 };

    short pxy[8] =
    {
        wi->left,
        wi->top,
        wi->left + wi->work.g_w - 1,
        wi->top + wi->work.g_h - 1,
        wi->work.g_x,
        wi->work.g_y,
        wi->work.g_x + wi->work.g_y - 1,
        wi->work.g_y + wi->work.g_h - 1
    };

    screen.fd_addr = 0;

    vro_cpyfm(vh, S_ONLY, pxy, &vw->colormap, &screen);
}

