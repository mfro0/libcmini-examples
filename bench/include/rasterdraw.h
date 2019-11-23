#ifndef _RASTERDRAW_H_
#define _RASTERDRAW_H_

#include <stdio.h>
#include <stdbool.h>
#include <mint/mintbind.h>
#include "util.h"

/*
 * 32k clut pulled from a Falcon
 */
static unsigned short color_lookup[] =
{
    0xffdf, 0x0000, 0xf800, 0x07c0, 0x001f, 0x07df, 0xffc0, 0xf81f,
    0xb596, 0x7bcf, 0x9800, 0x04c0, 0x0013, 0x04d3, 0x9cc0, 0x9813,
    0xffdf, 0xdedb, 0xd69a, 0xbdd7, 0xb596, 0x9cd3, 0x9492, 0x7bcf,
    0x738e, 0x5acb, 0x528a, 0x39c7, 0x3186, 0x18c3, 0x1082, 0x0000,
    0xf800, 0xf802, 0xf803, 0xf806, 0xf807, 0xf80a, 0xf80b, 0xf80e,
    0xf80f, 0xf812, 0xf813, 0xf816, 0xf817, 0xf81a, 0xf81b, 0xf81f,
    0xd81f, 0xd01f, 0xb81f, 0xb01f, 0x981f, 0x901f, 0x781f, 0x701f,
    0x581f, 0x501f, 0x381f, 0x301f, 0x181f, 0x101f, 0x001f, 0x009f,
    0x00df, 0x019f, 0x01df, 0x029f, 0x02df, 0x039f, 0x03df, 0x049f,
    0x04df, 0x059f, 0x05df, 0x069f, 0x06df, 0x07df, 0x07db, 0x07da,
    0x07d7, 0x07d6, 0x07d3, 0x07d2, 0x07cf, 0x07ce, 0x07cb, 0x07ca,
    0x07c7, 0x07c6, 0x07c3, 0x07c2, 0x07c0, 0x17c0, 0x1fc0, 0x37c0,
    0x3fc0, 0x57c0, 0x5fc0, 0x77c0, 0x7fc0, 0x97c0, 0x9fc0, 0xb7c0,
    0xbfc0, 0xd7c0, 0xdfc0, 0xffc0, 0xfec0, 0xfe80, 0xfdc0, 0xfd80,
    0xfcc0, 0xfc80, 0xfbc0, 0xfb80, 0xfac0, 0xfa80, 0xf9c0, 0xf980,
    0xf8c0, 0xf880, 0xb000, 0xb002, 0xb003, 0xb006, 0xb007, 0xb00a,
    0xb00b, 0xb00e, 0xb00f, 0xb012, 0xb013, 0xb016, 0x9816, 0x9016,
    0x7816, 0x7016, 0x5816, 0x5016, 0x3816, 0x3016, 0x1816, 0x1016,
    0x0016, 0x0096, 0x00d6, 0x0196, 0x01d6, 0x0296, 0x02d6, 0x0396,
    0x03d6, 0x0496, 0x04d6, 0x0596, 0x0593, 0x0592, 0x058f, 0x058e,
    0x058b, 0x058a, 0x0587, 0x0586, 0x0583, 0x0582, 0x0580, 0x1580,
    0x1d80, 0x3580, 0x3d80, 0x5580, 0x5d80, 0x7580, 0x7d80, 0x9580,
    0x9d80, 0xb580, 0xb4c0, 0xb480, 0xb3c0, 0xb380, 0xb2c0, 0xb280,
    0xb1c0, 0xb180, 0xb0c0, 0xb080, 0x7000, 0x7002, 0x7003, 0x7006,
    0x7007, 0x700a, 0x700b, 0x700e, 0x580e, 0x500e, 0x380e, 0x300e,
    0x180e, 0x100e, 0x000e, 0x008e, 0x00ce, 0x018e, 0x01ce, 0x028e,
    0x02ce, 0x038e, 0x038b, 0x038a, 0x0387, 0x0386, 0x0383, 0x0382,
    0x0380, 0x1380, 0x1b80, 0x3380, 0x3b80, 0x5380, 0x5b80, 0x7380,
    0x72c0, 0x7280, 0x71c0, 0x7180, 0x70c0, 0x7080, 0x3800, 0x3802,
    0x3803, 0x3806, 0x3807, 0x3007, 0x1807, 0x1007, 0x0007, 0x0087,
    0x00c7, 0x0187, 0x01c7, 0x01c6, 0x01c3, 0x01c2, 0x01c0, 0x11c0,
    0x19c0, 0x31c0, 0x39c0, 0x3980, 0x38c0, 0x3880, 0xffdf, 0x0000,
};



typedef struct rectangle
{
    int x;
    int y;
    int w;
    int h;
} RECT;


/*
 * return true if the point at (x, y) is inside the rectangle r, false otherwise
 */
static inline bool inside(const int x, const int y, const RECT * const r)
{
    if (x >= r->x && x < r->x + r->w)
    {
        if (y >= r->y && y < r->y + r->h)
        {
            return true;
        }
    }
    return false;
}

static bool clipping = true;
static RECT clip_rectangle;

static inline void set_clip(int x, int y, int w, int h, bool enable)
{
    clipping = enable;
    clip_rectangle.x = x;
    clip_rectangle.y = y;
    clip_rectangle.w = w;
    clip_rectangle.h = h;
}

static int linewidth = 3;
static inline void set_linewidth(int width)
{
    linewidth = width | 1;      /* must be odd */
}

typedef enum { REPLACE, TRANSPARENT, XOR, REVERSE_TRANSPARENT } write_mode_t;
write_mode_t write_mode;

static inline void set_writemode(write_mode_t w)
{
    write_mode = w;
}

/*
 * properties of a raster supposed to be copied with raster_copy
 */
struct raster
{
    short *address;   /* raster start address */
    int wd_width;     /* width in shorts */
    int width;        /* used width in pixels */
    int height;       /* number of lines */
    int num_planes;   /* number of color planes */
};


/*
 * set a pixel at (x, y) with colorindex c on a screen with interleaved bit planes
 */
static inline void set_pixel256(const struct raster *r, const int x, const int y, const int c)
{
    int i;

    short *the_word = r->address +
                      (y * r->wd_width + x / 16) * r->num_planes;

    for (i = 0; i < r->num_planes; i++)
    {
        int colbit = (c >> i) & 1;
        int mask = 1 << (15 - (x % 16));
        if (colbit)
            *(the_word + i) |= mask;
        else
            *(the_word + i) &= ~mask;
    }
}

/*
 * set a pixel at (x, y) with colorindex c on a screen with interleaved bit planes
 */
static inline void set_pixel32k(const struct raster *r, const int x, const int y, const int c)
{
    short color = color_lookup[c];

    r->address[(long) y * r->wd_width * r->num_planes + x] = color;
}

/*
 * draw a horizontal line from (x0, y) to (x1, y) with color c
 */
static inline void horizontal_line256(const struct raster *r, int x0, const int x1, const int y, const int c)
{
    int i;
    short colbar[8];

    short *the_word = r->address +
                      (y * r->wd_width + x0 / 16) * r->num_planes;

    int left = min(x1, (x0 & 0xfff0) + 16);		/* either end of start word or end of hline */
    int right = x1 & 0xfff0;

    /* do left "incomplete" part */

    while (x0 < left)
    {
        for (i = 0; i < r->num_planes; i++)
        {
            int colbit = (c >> i) & 1;
            int mask = 1 << (15 - (x0 % 16));

            if (colbit)
                *(the_word + i) |= 0xffff & mask;
            else
                *(the_word + i) &= ~ (0xffff & mask);
        }
        x0++;
    }
    the_word += r->num_planes;

    if (x0 == x1) return;		/* hline ended on a word boundary */

    /* initialize colbar, this is used for fast fill of intermediate "complete" screen words */
    for (i = 0; i < r->num_planes; i++)
    {
        int colbit = (c >> i) & 1;
        if (colbit)
            colbar[i] = 0xffff;
        else
            colbar[i] = 0;
    }

    /* do "middle" part */
    while (x0 < right)
    {
        for (i = 0; i < r->num_planes; i++)
            *(the_word + i) = colbar[i];
        x0 += 16;
        the_word += r->num_planes;
    }

    /* do right "incomplete" part */
    while (x0 < x1)
    {
        int mask = 1 << (15 - (x0 % 16));

        for (i = 0; i < r->num_planes; i++)
        {
            int colbit = (c >> i) & 1;
            if (colbit)
                *(the_word + i) |= mask;
            else
                *(the_word + i) &= ~mask;
        }
        x0++;
    }
}

/*
 * draw a horizontal line from (x0, y) to (x1, y) with color c
 */
static inline void horizontal_line32k(const struct raster *r, int x0, const int x1, const int y, const int c)
{
    int i;

    for (i = x0; i <= x1; i++)
    {
        set_pixel32k(r, i, y, c);
    }
}

/*
 * draw a horizontal line from (x0, y) to (x1, y) with color c, respect clipping
 */
static inline void horizontal_line_clipped256(const struct raster *r, int x0, int x1, const int y, const int c)
{
    int i;
    short colbar[8];

    short *the_word;

    x0 = max(x0, clip_rectangle.x);

    int left = min(x1, (x0 & 0xfff0) + 16);		/* either end of start word or end of hline */

    x1 = min(x1, clip_rectangle.x + clip_rectangle.w - 1);

    int right = x1 & 0xfff0;

    if (y < clip_rectangle.y || y > clip_rectangle.y + clip_rectangle.h - 1 ||
            right > left || x0 > x1)
    {
        return;
    }

    the_word = r->address + (y * r->wd_width + x0 / 16) * r->num_planes;

    /* do left "incomplete" part */

    while (x0 < left)
    {
        for (i = 0; i < r->num_planes; i++)
        {
            int colbit = (c >> i) & 1;
            int mask = 1 << (15 - (x0 % 16));
            if (colbit)
                *(the_word + i) |= mask;
            else
                *(the_word + i) &= ~mask;
        }
        x0++;
    }
    the_word += r->num_planes;

    if (x0 == x1) return;		/* hline ended on a word boundary */

    /* initialize colbar, this is used for fast fill of intermediate "complete" screen words */
    for (i = 0; i < r->num_planes; i++)
    {
        int colbit = (c >> i) & 1;
        if (colbit)
            colbar[i] = 0xffff;
        else
            colbar[i] = 0;
    }

    /* do "middle" part */
    while (x0 < right)
    {
        for (i = 0; i < r->num_planes; i++)
            *(the_word + i) = colbar[i];
        x0 += 16;
        the_word += r->num_planes;
    }

    /* do right "incomplete" part */
    while (x0 < x1)
    {
        int mask = 1 << (15 - (x0 % 16));

        for (i = 0; i < r->num_planes; i++)
        {
            int colbit = (c >> i) & 1;
            if (colbit)
                *(the_word + i) |= mask;
            else
                *(the_word + i) &= ~mask;
        }
        x0++;
    }
}

/*
 * draw a horizontal line from (x0, y) to (x1, y) with color c, respect clipping
 */
static inline void horizontal_line_clipped32k(const struct raster *r, int x0, int x1, const int y, const int c)
{
    x0 = max(x0, clip_rectangle.x);
    x1 = min(x1, clip_rectangle.x + clip_rectangle.w - 1);

    if (y >= clip_rectangle.y && y < clip_rectangle.h)
        horizontal_line32k(r, x0, x1, y, c);
}

/*
 * draw a line, without respecting line thickness
 */
static inline void draw_hairline256(const struct raster *r, int x0, int y0, int x1, int y1, const int col)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2; /* error value e_xy */

    for(;;)
    {
        set_pixel256(r, x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 > dy)
        {
            err += dy;
            x0 += sx;
        } /* e_xy + e_x > 0 */
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        } /* e_xy + e_y < 0 */
    }
}

/*
 * draw a line, without respecting line thickness
 */
static inline void draw_hairline32k(const struct raster *r, int x0, int y0, int x1, int y1, const int col)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2; /* error value e_xy */

    for(;;)
    {
        set_pixel32k(r, x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 > dy)
        {
            err += dy;
            x0 += sx;
        } /* e_xy + e_x > 0 */
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        } /* e_xy + e_y < 0 */
    }
}

static inline void vertical_line_clipped256(const struct raster *r, const int x, int y1, int y2, const int color)
{
    y1 = max(y1, clip_rectangle.y);
    y2 = min(y2, clip_rectangle.y + clip_rectangle.h - 1);

    if (x < clip_rectangle.x || x > clip_rectangle.x + clip_rectangle.w - 1 ||
        y1 > y2)
    {
        return;
    }

    draw_hairline256(r, x, y1, x, y2, color);
}

static inline void vertical_line_clipped32k(const struct raster *r, const int x, int y1, int y2, const int color)
{
    y1 = max(y1, clip_rectangle.y);
    y2 = min(y2, clip_rectangle.y + clip_rectangle.h - 1);

    if (x < clip_rectangle.x || x > clip_rectangle.x + clip_rectangle.w - 1 ||
        y1 > y2)
    {
        return;
    }

    draw_hairline32k(r, x, y1, x, y2, color);
}

/*
 * draw a circle (pixel accurate, respects linewidth, respects clipping)
 */
static inline void draw_circle256(const struct raster *r, const int xc, const int yc, const int radius, const int color)
{
    /* simplified bresenham */

    const int inner = radius - linewidth / 2;
    const int outer = radius + linewidth / 2;
    int xo = outer;
    int xi = inner;
    int y = 0;
    int erro = 1 - xo;
    int erri = 1 - xi;

    while (xo >= y)
    {
        horizontal_line_clipped256(r, xc + xi, xc + xo, yc + y, color);
        vertical_line_clipped256(r, xc + y, yc + xi, yc + xo, color);
        horizontal_line_clipped256(r, xc - xo, xc - xi, yc + y, color);
        vertical_line_clipped256(r, xc - y, yc + xi, yc + xo, color);
        horizontal_line_clipped256(r, xc - xo, xc - xi, yc - y, color);
        vertical_line_clipped256(r, xc - y, yc - xo, yc - xi, color);
        horizontal_line_clipped256(r, xc + xi, xc + xo, yc - y, color);
        vertical_line_clipped256(r, xc + y, yc - xo, yc - xi, color);

        y++;

        if (erro < 0)
        {
            erro += 2 * y + 1;
        }
        else
        {
            xo--;
            erro += 2 * (y - xo + 1);
        }

        if (y > inner)
        {
            xi = y;
        }
        else
        {
            if (erri < 0)
            {
                erri += 2 * y + 1;
            }
            else
            {
                xi--;
                erri += 2 * (y - xi + 1);
            }
        }
    }
}


/*
 * draw a circle (pixel accurate, respects linewidth, respects clipping)
 */
static inline void draw_circle32k(const struct raster *r, const int xc, const int yc, const int radius, const int color)
{
    /* simplified bresenham */

    const int inner = radius - linewidth / 2;
    const int outer = radius + linewidth / 2;
    int xo = outer;
    int xi = inner;
    int y = 0;
    int erro = 1 - xo;
    int erri = 1 - xi;

    while (xo >= y)
    {
        horizontal_line_clipped32k(r, xc + xi, xc + xo, yc + y, color);
        vertical_line_clipped32k(r, xc + y, yc + xi, yc + xo, color);
        horizontal_line_clipped32k(r, xc - xo, xc - xi, yc + y, color);
        vertical_line_clipped32k(r, xc - y, yc + xi, yc + xo, color);
        horizontal_line_clipped32k(r, xc - xo, xc - xi, yc - y, color);
        vertical_line_clipped32k(r, xc - y, yc - xo, yc - xi, color);
        horizontal_line_clipped32k(r, xc + xi, xc + xo, yc - y, color);
        vertical_line_clipped32k(r, xc + y, yc - xo, yc - xi, color);

        y++;

        if (erro < 0)
        {
            erro += 2 * y + 1;
        }
        else
        {
            xo--;
            erro += 2 * (y - xo + 1);
        }

        if (y > inner)
        {
            xi = y;
        }
        else
        {
            if (erri < 0)
            {
                erri += 2 * y + 1;
            }
            else
            {
                xi--;
                erri += 2 * (y - xi + 1);
            }
        }
    }
}

/*
 * draw an arc (pixel accurate, respects linewidth, respects clipping)
 */
static inline void draw_arc256(const struct raster *r, const int xc, const int yc, const int radius,
                               const int start_angle, const int end_angle, const int color)
{
    /* simplified bresenham */

    const int inner = radius - linewidth / 2;
    const int outer = radius + linewidth / 2;
    int xo = outer;
    int xi = inner;
    int y = 0;
    int erro = 1 - xo;
    int erri = 1 - xi;

    /*
     * calculate end points of circles
     */
    /*
    int xstart_inner;
    int ystart_inner;
    int xend_inner;
    int yend_inner;

    int xstart_outer;
    int ystart_outer;
    int xend_outer;
    int yend_outer;

    xstart_inner = xc + inner * icos(start_angle) / 32767;
    ystart_inner = yc + inner * isin(start_angle) / 32767;
    xend_inner = xc + inner * icos(end_angle) / 32767;
    yend_inner = yc + inner * isin(end_angle) / 32767;

    xstart_outer = xc + outer * icos(start_angle) / 32767;
    ystart_outer = yc + outer * isin(start_angle) / 32767;
    xend_outer = xc + outer * icos(end_angle) / 32767;
    yend_outer = yc + outer * isin(end_angle) / 32767;

    dbg("(xc,yc,r)=(%d,%d,%d) start=(%d,%d)(%d,%d), end=(%d,%d)(%d,%d)\r\n",
        xc, yc, radius,
        xstart_inner, ystart_inner, xstart_outer, ystart_outer,
        xend_inner, yend_inner, xend_outer, yend_outer);

    */
    while (xo >= y)
    {
        horizontal_line_clipped256(r, xc + xi, xc + xo, yc + y, color);
        vertical_line_clipped256(r, xc + y, yc + xi, yc + xo, color);
        horizontal_line_clipped256(r, xc - xo, xc - xi, yc + y, color);
        vertical_line_clipped256(r, xc - y, yc + xi, yc + xo, color);
        horizontal_line_clipped256(r, xc - xo, xc - xi, yc - y, color);
        vertical_line_clipped256(r, xc - y, yc - xo, yc - xi, color);
        horizontal_line_clipped256(r, xc + xi, xc + xo, yc - y, color);
        vertical_line_clipped256(r, xc + y, yc - xo, yc - xi, color);

        y++;

        if (erro < 0)
        {
            erro += 2 * y + 1;
        }
        else
        {
            xo--;
            erro += 2 * (y - xo + 1);
        }

        if (y > inner)
        {
            xi = y;
        }
        else
        {
            if (erri < 0)
            {
                erri += 2 * y + 1;
            }
            else
            {
                xi--;
                erri += 2 * (y - xi + 1);
            }
        }
    }
}

/*
 * draw a "hairline" circle (does not respect linewidth)
 */
static inline void draw_hairline_circle256(const struct raster *r, const int xc, const int yc, const int radius, const int color)
{
    /* simplified bresenham */
    int d;
    int dx;
    int dxy;
    int x;
    int y;

    x = 0;
    y = radius;
    d = 1 - radius;
    dx = 3;
    dxy = -2 * radius + 5;

    while (y >= x)
    {
        /* draw mirrored circle in eight octants */

        if (clipping)
        {
            if (inside(xc + x, yc + y, &clip_rectangle))
            {
                set_pixel256(r, xc + x, yc + y, color);
            }

            if (inside(xc + y, yc + x, &clip_rectangle))
            {
                set_pixel256(r, xc + y, yc + x, color);

            }

            if (inside(xc + y, yc - x, &clip_rectangle))
            {
                set_pixel256(r, xc + y, yc - x, color);
            }

            if (inside(xc + x, yc - y, &clip_rectangle))
            {
                set_pixel256(r, xc + x, yc - y, color);
            }

            if (inside(xc - x, yc - y, &clip_rectangle))
            {
                set_pixel256(r, xc - x, yc - y, color);
            }

            if (inside(xc - y, yc - x, &clip_rectangle))
            {
                set_pixel256(r, xc - y, yc - x, color);
            }

            if (inside(xc - y, yc + x, &clip_rectangle))
            {
                set_pixel256(r, xc - y, yc + x, color);
            }

            if (inside(xc - x, yc + y, &clip_rectangle))
            {
                set_pixel256(r, xc - x, yc + y, color);
            }
        }
        else
        {
            set_pixel256(r, xc + x, yc + y, color);
            set_pixel256(r, xc + y, yc + x, color);
            set_pixel256(r, xc + y, yc - x, color);
            set_pixel256(r, xc + x, yc - y, color);
            set_pixel256(r, xc - x, yc - y, color);
            set_pixel256(r, xc - y, yc - x, color);
            set_pixel256(r, xc - y, yc + x, color);
            set_pixel256(r, xc - x, yc + y, color);
        }

        if (d < 0)
        {
            d += dx;
            dx += 2;
            dxy += 2;
            x++;
        }
        else
        {
            d += dxy;
            dx += 2;
            dxy += 4;
            x++;
            y--;
        }
    }
}

/*
 * draw a "hairline" circle (does not respect linewidth)
 */
static inline void draw_hairline_circle32k(const struct raster *r, const int xc, const int yc, const int radius, const int color)
{
    /* simplified bresenham */
    int d;
    int dx;
    int dxy;
    int x;
    int y;

    x = 0;
    y = radius;
    d = 1 - radius;
    dx = 3;
    dxy = -2 * radius + 5;

    while (y >= x)
    {
        /* draw mirrored circle in eight octants */

        if (clipping)
        {
            if (inside(xc + x, yc + y, &clip_rectangle))
            {
                set_pixel32k(r, xc + x, yc + y, color);
            }

            if (inside(xc + y, yc + x, &clip_rectangle))
            {
                set_pixel32k(r, xc + y, yc + x, color);

            }

            if (inside(xc + y, yc - x, &clip_rectangle))
            {
                set_pixel32k(r, xc + y, yc - x, color);
            }

            if (inside(xc + x, yc - y, &clip_rectangle))
            {
                set_pixel32k(r, xc + x, yc - y, color);
            }

            if (inside(xc - x, yc - y, &clip_rectangle))
            {
                set_pixel32k(r, xc - x, yc - y, color);
            }

            if (inside(xc - y, yc - x, &clip_rectangle))
            {
                set_pixel32k(r, xc - y, yc - x, color);
            }

            if (inside(xc - y, yc + x, &clip_rectangle))
            {
                set_pixel32k(r, xc - y, yc + x, color);
            }

            if (inside(xc - x, yc + y, &clip_rectangle))
            {
                set_pixel32k(r, xc - x, yc + y, color);
            }
        }
        else
        {
            set_pixel32k(r, xc + x, yc + y, color);
            set_pixel32k(r, xc + y, yc + x, color);
            set_pixel32k(r, xc + y, yc - x, color);
            set_pixel32k(r, xc + x, yc - y, color);
            set_pixel32k(r, xc - x, yc - y, color);
            set_pixel32k(r, xc - y, yc - x, color);
            set_pixel32k(r, xc - y, yc + x, color);
            set_pixel32k(r, xc - x, yc + y, color);
        }

        if (d < 0)
        {
            d += dx;
            dx += 2;
            dxy += 2;
            x++;
        }
        else
        {
            d += dxy;
            dx += 2;
            dxy += 4;
            x++;
            y--;
        }
    }
}

/*
 * draw a filled circle
 */
static inline void draw_filled_circle256(const struct raster *r, const int xc, const int yc, const int radius, const int color)
{
    /* simplified bresenham */
    int d;
    int dx;
    int dxy;
    int x;
    int y;

    x = 0;
    y = radius;
    d = 1 - radius;
    dx = 3;
    dxy = -2 * radius + 5;

    while (y >= x)
    {
        horizontal_line256(r, xc - x, xc + x, yc + y, color);
        horizontal_line256(r, xc - x, xc + x, yc - y, color);
        horizontal_line256(r, xc - y, xc + y, yc + x, color);
        horizontal_line256(r, xc - y, xc + y, yc - x, color);

        if (d < 0)
        {
            d += dx;
            dx += 2;
            dxy += 2;
            x++;
        }
        else
        {
            d += dxy;
            dx += 2;
            dxy += 4;
            x++;
            y--;
        }
    }
}

/*
 * draw a filled circle
 */
static inline void draw_filled_circle32k(const struct raster *r, const int xc, const int yc, const int radius, const int color)
{
    /* simplified bresenham */
    int d;
    int dx;
    int dxy;
    int x;
    int y;

    x = 0;
    y = radius;
    d = 1 - radius;
    dx = 3;
    dxy = -2 * radius + 5;

    while (y >= x)
    {
        horizontal_line_clipped32k(r, xc - x, xc + x, yc + y, color);
        horizontal_line_clipped32k(r, xc - x, xc + x, yc - y, color);
        horizontal_line_clipped32k(r, xc - y, xc + y, yc + x, color);
        horizontal_line_clipped32k(r, xc - y, xc + y, yc - x, color);

        if (d < 0)
        {
            d += dx;
            dx += 2;
            dxy += 2;
            x++;
        }
        else
        {
            d += dxy;
            dx += 2;
            dxy += 4;
            x++;
            y--;
        }
    }
}

/*
 * draw a filled rectangle
 */
static inline void draw_filled_rectangle256(const struct raster *r, const int x0, int y0, const int x1, const int y1, const int col)
{
    while (y0 <= y1)
    {
        horizontal_line256(r, x0, y0, x1, col);
        y0++;
    }
}

/*
 * draw a filled rectangle
 */
static inline void draw_filled_rectangle32k(const struct raster *r, const int x0, int y0, const int x1, const int y1, const int col)
{
    while (y0 <= y1)
    {
        horizontal_line32k(r, x0, y0, x1, col);
        y0++;
    }
}
#endif /* _RASTERDRAW_H_ */
