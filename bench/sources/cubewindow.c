#include "window.h"
#include "global.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "cubewindow.h"

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

/* private data for this window type */
struct cubewindow
{
    short node_color;
    short line_color;
    float xrot;
    float yrot;
    float zrot;
};

static void timer_cubewindow(struct window *wi);
static void delete_cubewindow(struct window *wi);
static void draw_cubewindow(struct window *wi, short wx, short wy, short ww, short wh);

/*
 * create a new window and add it to the window list.
 */
struct window *create_cubewindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct cubewindow *cw;

    wi = create_window(wi_kind, title);

    if (wi != NULL)
    {
        wi->wclass = CUBEWINDOW_CLASS;
        wi->draw = draw_cubewindow;
        wi->del = delete_cubewindow;
        wi->timer = timer_cubewindow;

        wi->word_aligned = false;

        cw = malloc(sizeof(struct cubewindow));

        if (cw != NULL)
        {
            wi->priv = cw;
            cw->node_color = 4;
            cw->line_color = 2;
            cw->xrot = 0.0;
            cw->yrot = 0.0;
            cw->zrot = 0.0;
        }
        else
        {
            fprintf(stderr, "could not allocate cubewindow private data\r\n");
            delete_window(wi);

            return NULL;
        }

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = 0;
        wi->doc_height = 0;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */

    }
    return wi;
}

static void delete_cubewindow(struct window *wi)
{
    /* free window-private memory */
    if (wi && wi->priv)
    {
        struct cubewindow *cw = (struct cubewindow *) wi->priv;

        (void) cw; /* we might need it later */

        free(wi->priv);
    }
    /* let the generic window code do the rest */
    delete_window(wi);
}

struct world_point
{
    float x;
    float y;
    float z;
};

/*
 * our world extends from -100, -100, -100 to 100, 100, 100
 */
static struct world_point cube[] =
{
    { -100.0, -100.0, -100.0 },
    { -100.0, -100.0,  100.0 },
    { -100.0,  100.0, -100.0 },
    { -100.0,  100.0,  100.0 },
    {  100.0, -100.0, -100.0 },
    {  100.0, -100.0,  100.0 },
    {  100.0,  100.0, -100.0 },
    {  100.0,  100.0,  100.0 }
};
#define NUM_NODES     sizeof(cube) / sizeof(struct world_point)

static short edges[][2] =
{
    { 0, 1 },
    { 1, 3 },
    { 3, 2 },
    { 2, 0 },
    { 4, 5 },
    { 5, 7 },
    { 7, 6 },
    { 6, 4 },
    { 0, 4 },
    { 1, 5 },
    { 2, 6 },
    { 3, 7 }
};
#define NUM_EDGES   sizeof(edges) / sizeof(short[2])

#define NODE_RADIUS 8

static void rotate_z(float phi)
{
    float sin_p = sin(phi);
    float cos_p = cos(phi);
    int i;

    for (i = 0; i < NUM_NODES; i++)
    {
        struct world_point node = cube[i];

        cube[i].x = node.x * cos_p - node.y * sin_p;
        cube[i].y = node.y * cos_p + node.x * sin_p;
    }
}

static void rotate_y(float phi)
{
    float sin_p = sin(phi);
    float cos_p = cos(phi);
    int i;

    for (i = 0; i < NUM_NODES; i++)
    {
        struct world_point node = cube[i];

        cube[i].x = node.x * cos_p - node.z * sin_p;
        cube[i].z = node.z * cos_p + node.x * sin_p;
    }

}

static void rotate_x(float phi)
{
    float sin_p = sin(phi);
    float cos_p = cos(phi);
    int i;

    for (i = 0; i < NUM_NODES; i++)
    {
        struct world_point node = cube[i];

        cube[i].y = node.y * cos_p - node.z * sin_p;
        cube[i].z = node.z * cos_p + node.y * sin_p;
    }
}

/*
 * draw window
 */
static void draw_cubewindow(struct window *wi, short wx, short wy, short ww, short wh)
{
    short x;
    short y;
    short w;
    short h;
    int i;
    short pxy[4];
    short vh = wi->vdi_handle;

    struct cubewindow *cw = (struct cubewindow *) wi->priv;

    dbg("start");

    /* get size of window's work area */
    wind_get(wi->handle, WF_WORKXYWH, &x, &y, &w, &h);

    /* first, clear it */
    if (wi->clear) wi->clear(wi, x, y, w, h);

    vsl_color(vh, cw->line_color);
    vsf_color(vh, cw->node_color);
    vsl_width(vh, NODE_RADIUS);
    for (i = 0; i < NUM_EDGES; i++)
    {
        struct world_point *p0 = &cube[edges[i][0]];
        struct world_point *p1 = &cube[edges[i][1]];

        pxy[0] = x + w / 2 + p0->x * 200 / (p0->z / 4 + 200);
        pxy[1] = y + h / 2 + p0->y * 200 / (p0->z / 4 + 200);
        pxy[2] = x + w / 2 + p1->x * 200 / (p1->z / 4 + 200);
        pxy[3] = y + h / 2 + p1->y * 200 / (p1->z / 4 + 200);
        v_pline(vh, 2, pxy);
    }

    for (i = 0; i < NUM_NODES; i++)
    {
        struct world_point *node = &cube[i];

        v_circle(vh, x + w / 2 + node->x * 200 / (node->z / 4 + 200),
                             y + h / 2 + node->y * 200 / (node->z / 4 + 200), NODE_RADIUS);
    }
    dbg("end");
}


/*
 * react on timer events
 */
static void timer_cubewindow(struct window *wi)
{
    struct cubewindow *cw = wi->priv;

    dbg("start");
    if (cw != NULL)
    {
        rotate_x(0.1);
        rotate_y(0.05);
        rotate_z(0.1);

        do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
    }
    dbg("end");
}
