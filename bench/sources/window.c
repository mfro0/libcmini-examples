#include "window.h"
#include "util.h"
#include "global.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// #define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

static short max_windows = 4;
static short num_windows = 0;

#define WINDOW_CLASS	0x1234L

static struct window **window_list;
static bool initialized = false;

/*
 * initialize the window list
 */
void init_windows(void)
{
    if (initialized) {
        form_alert(1, "[3][Attempt for multiple initializations|of window module][CANCEL]");
        exit(1);
    }
    window_list = malloc(sizeof(struct window *) * max_windows);

    initialized = true;
}

/*
 * destroy the window list and its contents
 */
void free_windows(void)
{
    short i;

    for (i = 0; i < num_windows; i++)
    {
        window_list[i]->del(window_list[i]);
        free(window_list[i]);
    }
    free(window_list);
    initialized = 0;
    max_windows = 0;
    num_windows = 0;
}

/*
 * add a window to the window list. Autoextends the list if it is too small
 */
static void add_window(struct window *wi)
{
    short i;

    if (num_windows > max_windows - 1)
    {
        struct window **new_stack;

        new_stack = malloc(sizeof(struct window *) * max_windows * 2);

        for (i = 0; i < max_windows; i++)
        {
            new_stack[i] = window_list[i];
        }
        free(window_list);
        window_list = new_stack;
        max_windows *= 2;
    }
    window_list[num_windows++] = wi;
}

/*
 * remove a window from the window list
 */
void delete_window(struct window *wi)
{
    short i;

    for (i = 0; i < num_windows; i++)
    {
        if (window_list && window_list[i] == wi)
        {
            short j;

            wind_close(wi->handle);
            wind_delete(wi->handle);

            for (j = i; j < num_windows - 1; j++)
            {
                window_list[j] = window_list[j + 1];
            }
            free(wi);
            num_windows--;
            break;
        }
    }
}

/*
 * get a WINDOW ptr for a window handle
 */
struct window *from_handle(short handle)
{
    short i;

    for (i = 0; i < num_windows; i++)
    {
        if (window_list[i]->handle == handle)
        {
            return window_list[i];
        }
    }
    return NULL;
}

/*
 * create a new window and add it to the window list
 */
struct window *create_window(short wi_kind, char *title)
{
    struct window *wi;
    struct window *topped_window;
    short handle;

    handle = wind_create(wi_kind, gl_desk.g_x, gl_desk.g_y, gl_desk.g_w, gl_desk.g_h);

    if (handle < 1)
    {
        form_alert(1, "[1][cannot create window][OK]");
        return NULL;
    }

    wi = malloc(sizeof(struct window));

    wi->wclass = WINDOW_CLASS;
    wi->handle = handle;


    wi->kind = wi_kind;
    wi->fulled = false;

    /*
     * find currently topped window and untop it
     */
    topped_window = top_window();
    if (topped_window)	topped_window->topped = false;

    wi->word_aligned = false;
    wi->topped = true;

    wi->draw = NULL;
    wi->del = delete_window;
    wi->opn = open_window;
    wi->scroll = scroll_window;
    wi->clear = clear_window;
    wi->size = size_window;
    wi->full = full_window;
    wi->timer = NULL;           /* this is not connected by default */

    wi->left = wi->top = 0;		/* start display at top left corner of document */
    wi->doc_width = wi->doc_height = 0;

    v_opnvwk(wi->work_in, &wi->vdi_handle, wi->work_out);
    wind_set_str(wi->handle, WF_NAME, title);
    wind_set_str(wi->handle, WF_INFO, "");

    add_window(wi);

    return wi;
}

/*
 * return the current top window
 */
struct window *top_window(void)
{
    int i;

    for (i = 0; i < num_windows; i++)
    {
        if (window_list[i]->topped)
        {
            return window_list[i];
        }
    }
    return NULL;
}

/*
 * do something (callback) to each window in the window list. If the passed callback function
 * pointer is NULL, nothing is done.
 */
int foreach_window(wi_cb cb)
{
    int i;

    for (i = 0; i < num_windows; i++)
    {
        if (cb) (*cb)(window_list[i]);
    }
    return i;
}

/*
 * resize window. Coordinates are frame coordinates
 */
void size_window(struct window *wi, short x, short y, short w, short h)
{
    if (w < MIN_WIDTH)
    {
        w = MIN_WIDTH;
    }
    if (h < MIN_HEIGHT)
    {
        h = MIN_HEIGHT;
    }

    wind_set(wi->handle, WF_CURRXYWH, x, y, w, h);
    wind_get(wi->handle, WF_WORKXYWH, &wi->work.g_x, &wi->work.g_y, &wi->work.g_w, &wi->work.g_h);
    wind_get(wi->handle, WF_CURRXYWH, &wi->rect.g_x, &wi->rect.g_y, &wi->rect.g_w, &wi->rect.g_h);
    if (wi->scroll) wi->scroll(wi); /* fix slider sizes and positions */
}

/*
 * open window
 */
void open_window(struct window *wi, short x, short y, short w, short h)
{
    graf_growbox(/* desk_x + desk_w / 2 */ 10, /* desk_y + desk_h / 2 */ 10, gl_wbox, gl_hbox,
                 x, y, w, h);
    wind_open(wi->handle, x, y, w, h);

    wind_get(wi->handle, WF_WORKXYWH, &wi->work.g_x, &wi->work.g_y, &wi->work.g_w, &wi->work.g_h);
    wind_get(wi->handle, WF_CURRXYWH, &wi->rect.g_x, &wi->rect.g_y, &wi->rect.g_w, &wi->rect.g_h);
}

/*
 * scroll window contents according to its slider settings
 */
void scroll_window(struct window *wi)
{
    short xpos;
    short ypos;
    short sl_horiz;
    short sl_vert;
    short ret;
    short value;

    sl_horiz = (int)((float) wi->work.g_w / wi->x_fac / wi->doc_width * 1000);
    sl_vert = (int)((float) wi->work.g_h / wi->y_fac / wi->doc_height * 1000);

    xpos = (int)((float) wi->left / (wi->doc_width - wi->work.g_w / wi->x_fac) * 1000);
    ypos = (int)((float) wi->top / (wi->doc_height - wi->work.g_h / wi->y_fac) * 1000);

    /* wind_set is a costly operation. Set values only if we need to */
    wind_get(wi->handle, WF_HSLIDE, &value, &ret, &ret, &ret);
    if (value != xpos)
        wind_set(wi->handle, WF_HSLIDE, xpos, 0, 0, 0);

    wind_get(wi->handle, WF_VSLIDE, &value, &ret, &ret, &ret);
    if (value != ypos)
        wind_set(wi->handle, WF_VSLIDE, ypos, 0, 0, 0);

    wind_get(wi->handle, WF_HSLSIZE, &value, &ret, &ret, &ret);
    if (value != sl_horiz)
        wind_set(wi->handle, WF_HSLSIZE, sl_horiz, 0, 0, 0);

    wind_get(wi->handle, WF_VSLSIZE, &value, &ret, &ret, &ret);
    if (value != sl_vert)
        wind_set(wi->handle, WF_VSLSIZE, sl_vert, 0, 0, 0);
}

void full_window(struct window *wi)
{
    if (wi->fulled)
    {
        wind_calc(WC_WORK, wi->kind, wi->old.g_x, wi->old.g_y,
                  wi->old.g_w, wi->old.g_h,
                  &wi->work.g_x, &wi->work.g_y, &wi->work.g_w, &wi->work.g_h);
        wind_set(wi->handle, WF_CURRXYWH, wi->old.g_x, wi->old.g_y, wi->old.g_w, wi->old.g_h);
    }
    else
    {
        wind_calc(WC_BORDER, wi->kind, wi->work.g_x, wi->work.g_y,
                  wi->work.g_w, wi->work.g_h,
                  &wi->old.g_x, &wi->old.g_y, &wi->old.g_w, &wi->old.g_h);
        wind_calc(WC_WORK, wi->kind, gl_desk.g_x, gl_desk.g_y, gl_desk.g_w, gl_desk.g_h,
                  &wi->work.g_x, &wi->work.g_y, &wi->work.g_w, &wi->work.g_h);
        wind_set(wi->handle, WF_CURRXYWH, gl_desk.g_x, gl_desk.g_y, gl_desk.g_w, gl_desk.g_h);
    }
    wind_get(wi->handle, WF_CURRXYWH, &wi->rect.g_x, &wi->rect.g_y, &wi->rect.g_w, &wi->rect.g_h);
    wind_get(wi->handle, WF_WORKXYWH, &wi->work.g_x, &wi->work.g_y, &wi->work.g_w, &wi->work.g_h);
    wi->fulled ^= 1;
}

static long time;
long *_hz_200 = (long *) 0x4ba;

static long start_timer(void)
{
    return time = *_hz_200;
}

static long stop_timer(void)
{
    return *_hz_200 - time;
}

/*
 * find and redraw all clipping rectangles
 */
void do_redraw(struct window *wi, short xc, short yc, short wc, short hc)
{
    GRECT t1, t2 = { xc, yc, wc, hc };
    short vh = wi->vdi_handle;

    graf_mouse(M_OFF, NULL);
    wind_update(BEG_UPDATE);

    Supexec(start_timer);

    wind_get(wi->handle, WF_FIRSTXYWH, &t1.g_x, &t1.g_y, &t1.g_w, &t1.g_h);
    while (t1.g_w || t1.g_h)
    {
        if (rc_intersect(&t2, &t1))
        {
            set_clipping(vh, t1.g_x, t1.g_y, t1.g_w, t1.g_h, 1);
            dbg("redraw window contents (%d, %d) to (%d, %d)\n",
                t1.g_x, t1.g_y, t1.g_x + t1.g_w, t1.g_y + t1.g_h);
            if (wi->draw) wi->draw(wi, t1.g_x, t1.g_y, t1.g_w, t1.g_h);
        }
        wind_get(wi->handle, WF_NEXTXYWH, &t1.g_x, &t1.g_y, &t1.g_w, &t1.g_h);
    }
    sprintf(wi->info, "Time for redraw: %ld ms", Supexec(stop_timer) * 5);
    wind_set_str(wi->handle, WF_INFO, wi->info);

    wind_update(END_UPDATE);
    graf_mouse(M_ON, NULL);
}

/*
 * clear window contents (to background color white)
 */
void clear_window(struct window *wi, short x, short y, short w, short h)
{
    short pxy[4];
    short vh = wi->vdi_handle;

    vsf_interior(vh, 1);
    vsf_style(vh, 0);
    vsf_color(vh, 0);
    pxy[0] = x;
    pxy[1] = y;
    pxy[2] = x + w - 1;
    pxy[3] = y + h - 1;
    v_bar(vh, pxy);  /* blank the interior */
}

/*
 * send a redraw event to our own application
 */
void send_redraw(struct window *wi, short x, short y, short w, short h)
{
    short msg[8] = { WM_REDRAW, ap_id, 0, wi->handle, x, y, w, h };

    appl_write(ap_id, sizeof(msg), msg);
}


