#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "window.h"
#include "global.h"
#include "clockwindow.h"
#include <limits.h>
#include "intmath.h"
#include "util.h"

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
static void draw_clockwindow(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_clockwindow(struct window *wi);
static void delete_clockwindow(struct window *wi);
static void open_clockwindow(struct window *wi, short x, short y, short w, short h);
static void delete_clockwindow(struct window *wi);
static void size_clockwindow(struct window *wi, short x, short y, short w, short h);
static void full_clockwindow(struct window *wi);

/*
 * create a new window and add it to the window list.
 */
struct window *create_clockwindow(short wi_kind, char *title)
{
    struct window *wi;
    struct clockwindow *cw;

    /* call base 'class' constructor */
    wi = create_window(wi_kind, title);

    if (wi)
    {
        wi->wclass = CLOCKWINDOW_CLASS;

        /* override base methods */
        wi->draw = draw_clockwindow;
        wi->del = delete_clockwindow;
        wi->timer = timer_clockwindow;
        wi->opn = open_clockwindow;
        wi->del = delete_clockwindow;
        wi->size = size_clockwindow;
        wi->full = full_clockwindow;

        /* extend base class */
        cw = malloc(sizeof(struct clockwindow));
        wi->priv = cw;

        cw->face_buffer = NULL;

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = 1;
        wi->y_fac = 1;
    }

    return wi;
}

/*
 * some housekeeping
 * make sure the face_buffer memory gets freed before the window is deleted
 */
static void delete_clockwindow(struct window *wi)
{
    short vh = wi->vdi_handle;
    struct clockwindow *cw = wi->priv;


    v_clsvwk(vh);

    if (cw->face_buffer)
        free(cw->face_buffer);
    if (wi->priv) free(wi->priv);

    /* let the generic window code do the rest */

    delete_window(wi);
}

/*
 * draw a clock face within the window we just created
 */
static void draw_face(struct window *wi, short x, short y, short w, short h)
{
    short minute;
    short ang;
    short pxy[4];
    short rad;
    short vh = wi->vdi_handle;

    rad = min(w, h) / 2;

    wi->clear(wi, x, y, w, h);

    /* an hour equals 60 minutes */

    for (minute = 0; minute < 60; minute++)
    {
        ang = minute * 3600L / 60L;

        /* rotate indicator into place */
        short min5 = minute % 5 == 0;       /* each full 5 minute gets a standout indicator */

        pxy[0] = (min5 ? rad - 18 : rad - 10) * icos(ang) / SHRT_MAX;
        pxy[1] = (min5 ? rad - 18 : rad - 10) * isin(ang) / SHRT_MAX;
        pxy[2] = (rad - 2) * icos(ang) / SHRT_MAX;
        pxy[3] = (rad - 2) * isin(ang) / SHRT_MAX;

        /* translate to position */
        pxy[0] += x + w / 2;
        pxy[1] += y + h / 2;
        pxy[2] += x + w / 2;
        pxy[3] += y + h / 2;

        /* draw */
        vsl_color(vh, 1);
        vsl_width(vh, 1);
        v_pline(vh, 2, pxy);
    }
}

/*
 * draw the clock hands
 */
static void draw_hands(struct window *wi)
{
    struct clockwindow *cw = wi->priv;
    struct tm *lt = &cw->prev;

    short ang;
    short vh = wi->vdi_handle;
    short rad = min(wi->work.g_w, wi->work.g_h) / 2;
    short pxy[4];

    dbg("%02d:%02d:%02d\n", lt->tm_hour, lt->tm_min, lt->tm_sec);

    /* draw "short hand" - hours */
    ang = (((lt->tm_hour % 12) * 3600L + lt->tm_min * 3600L / 60) / 12 + 2700) % 3600;
    dbg("hours ang=%d\n", ang);
    pxy[0] = (rad - 35) * icos(ang) / SHRT_MAX;
    pxy[1] = (rad - 35) * isin(ang) / SHRT_MAX;
    pxy[2] = -10 * icos(ang) / SHRT_MAX;
    pxy[3] = -10 * isin(ang) / SHRT_MAX;

    /* translate to position */
    pxy[0] += wi->work.g_x + wi->work.g_w / 2;
    pxy[1] += wi->work.g_y + wi->work.g_h / 2;
    pxy[2] += wi->work.g_x + wi->work.g_w / 2;
    pxy[3] += wi->work.g_y + wi->work.g_h / 2;

    vsl_width(vh, 5);
    v_pline(vh, 2, pxy);

    /* "long hand" - minutes */
    ang = (lt->tm_min * 3600L / 60 + 2700) % 3600;
    dbg("minutes ang=%d\n", ang);
    pxy[0] = (rad - 25) * icos(ang) / SHRT_MAX;
    pxy[1] = (rad - 25) * isin(ang) / SHRT_MAX;
    pxy[2] = -15 * icos(ang) / SHRT_MAX;
    pxy[3] = -15 * isin(ang) / SHRT_MAX;

    /* translate to position */
    pxy[0] += wi->work.g_x + wi->work.g_w / 2;
    pxy[1] += wi->work.g_y + wi->work.g_h / 2;
    pxy[2] += wi->work.g_x + wi->work.g_w / 2;
    pxy[3] += wi->work.g_y + wi->work.g_h / 2;

    vsl_width(vh, 3);
    v_pline(vh, 2, pxy);

    /* draw "light hand" - seconds */
    ang = (lt->tm_sec * 3600L / 60 + 2700) % 3600;
    dbg("seconds ang=%d\n", ang);
    pxy[0] = (rad - 20) * icos(ang) / SHRT_MAX;
    pxy[1] = (rad - 20) * isin(ang) / SHRT_MAX;
    pxy[2] = -10 * icos(ang) / SHRT_MAX;
    pxy[3] = -10 * isin(ang) / SHRT_MAX;

    /* translate to position */
    pxy[0] += wi->work.g_x + wi->work.g_w / 2;
    pxy[1] += wi->work.g_y + wi->work.g_h / 2;
    pxy[2] += wi->work.g_x + wi->work.g_w / 2;
    pxy[3] += wi->work.g_y + wi->work.g_h / 2;

    vsl_width(vh, 1);
    v_pline(vh, 2, pxy);
}

/*
 * we override the "open" function to to copy away the image of the clock face.
 * We then only need to blit it on redraws (at least until the window does not
 * get resized) instead of doing the expensive redraw each update
 */
static void open_clockwindow(struct window *wi, short x, short y, short w, short h)
{
    open_window(wi, x, y, w, h);    /* call "base class method" */

    struct clockwindow *cw = wi->priv;
    short vh = wi->vdi_handle;

    short pxy[8] =
    {
        wi->work.g_x,                       /* source and ... */
        wi->work.g_y,
        wi->work.g_x + wi->work.g_w - 1,
        wi->work.g_y + wi->work.g_h - 1,
        0,                                  /* ... destination coordinates for the image save blit */
        0,
        wi->work.g_w - 1,
        wi->work.g_h - 1
    };

    cw->face_buffer = realloc(cw->face_buffer, (w + 15) / 16 * gl_nplanes * sizeof(short) * h);
    if (!cw->face_buffer)
        exit(1);

    MFDB mfdb_src =
    {
        .fd_addr = NULL,
    };
    MFDB mfdb_dst =
    {
        .fd_addr = cw->face_buffer,
        .fd_w = w,
        .fd_h = h,
        .fd_wdwidth = (wi->work.g_w + 15) / sizeof(short) / 8,
        .fd_stand = 0,
        .fd_nplanes = gl_nplanes,
        0, 0, 0
    };


    /*
     * make sure mouse pointer does not appear on the face copy
     */
    graf_mouse(M_OFF, 0);
    draw_face(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
    vro_cpyfm(vh, S_ONLY, pxy, &mfdb_src, &mfdb_dst);
    graf_mouse(M_ON, 0);
}

/*
 * beware: the coordinates passed are the *requested* size of the window frame,
 * *not* the size of the work area.
 * If calling the base class method, these coordinates might even be totally meaningless (as
 * the function might have decided to deny or change the requested size). The rect member
 * of the window should have the correct coordinates, however.
 */
static void size_clockwindow(struct window *wi, short x, short y, short w, short h)
{
    struct clockwindow *cw = wi->priv;
    short vh = wi->vdi_handle;

    size_window(wi, x, y, w, h);

    short pxy[8] =
    {
        wi->work.g_x,
        wi->work.g_y,
        wi->work.g_x + wi->work.g_w - 1,
        wi->work.g_y + wi->work.g_h - 1,
        0,
        0,
        wi->work.g_w - 1,
        wi->work.g_h - 1
    };


    /* realloc clock face buffer */
    cw->face_buffer = realloc(cw->face_buffer, (wi->work.g_w + 15) / 16 * wi->work.g_h * gl_nplanes * sizeof(short));
    if (!cw->face_buffer)
        exit(1);

    MFDB mfdb_src =
    {
        .fd_addr = NULL,
    };
    MFDB mfdb_dst =
    {
        .fd_addr = cw->face_buffer,
        .fd_w = wi->work.g_w,
        .fd_h = wi->work.g_h,
        .fd_wdwidth = (wi->work.g_w + 15) / sizeof(short) / 8,
        .fd_stand = 0,
        .fd_nplanes = gl_nplanes,
        0, 0, 0
    };

    set_clipping(vh, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h, 1);
    clear_window(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
    draw_face(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
    vro_cpyfm(vh, S_ONLY, pxy, &mfdb_src, &mfdb_dst);
    draw_hands(wi);
}

void full_clockwindow(struct window *wi)
{
    full_window(wi);

    size_clockwindow(wi, wi->rect.g_x, wi->rect.g_y, wi->rect.g_w, wi->rect.g_h);
}
/*
 * Draw a wall clock
 */
static void draw_clockwindow(struct window *wi, short x, short y, short w, short h)
{
    short vh = wi->vdi_handle;
    struct clockwindow *cw = wi->priv;

    MFDB mfdb_dst =
    {
        .fd_addr = NULL,
    };
    MFDB mfdb_src =
    {
        .fd_addr = cw->face_buffer,
        .fd_w = w,
        .fd_h = h,
        .fd_wdwidth = (wi->work.g_w + 15) / sizeof(short) / 8,
        .fd_stand = 0,
        .fd_nplanes = gl_nplanes,
        0, 0, 0
    };
    short pxy[8] =
    {
        0,
        0,
        wi->work.g_w - 1,
        wi->work.g_h - 1,
        wi->work.g_x,
        wi->work.g_y,
        wi->work.g_x + wi->work.g_w - 1,
        wi->work.g_y + wi->work.g_h - 1
    };

    // wi->clear(wi, x, y, w, h);

    vro_cpyfm(vh, S_ONLY, pxy, &mfdb_src, &mfdb_dst);
    draw_hands(wi);
}


/*
 * react on timer events
 */
static void timer_clockwindow(struct window *wi)
{
    struct clockwindow *cw = wi->priv;
    struct tm *prev = &cw->prev;
    /* if the time didn't change since the last timer-driven draw, we do not need to redraw the clock */
    /*
     * get time from OS
     */
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);

    if (!(lt->tm_hour == prev->tm_hour && lt->tm_min == prev->tm_min && lt->tm_sec == prev->tm_sec))
    {
        *prev = *lt;
        do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
    }
}

