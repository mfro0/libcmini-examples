#include <gem.h>
#include <osbind.h>

#include "util.h"
#include "global.h"

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

static void init_userdef(void);

short init_util(void)
{
    init_userdef();

    return 0;
}

short free_util(void)
{
    v_clsvwk(vdi_handle);

    return 0;
}


static short handle;
static USERBLK menu_blk;

short open_vwork(short work_out[], short ext_out[])
{
    short vdi_handle;
    int i;

    vdi_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
    for (i = 0; i < 10; work_in[i++] = 1);
    work_in[10] = 2;
    v_opnvwk(work_in, &vdi_handle, work_out);
    vq_extnd(vdi_handle, 1, ext_out);
    gl_nplanes = ext_out[4];

    return vdi_handle;
}

void set_clipping(short handle, short x, short y, short w, short h, short on)
{
    short clip[4];

    clip[0] = x;
    clip[1] = y;
    clip[2] = clip[0] + w - 1;
    clip[3] = clip[1] + h - 1;

    vs_clip(handle, on, clip);
}

void fix_menu(OBJECT *tree)
{
    short i = -1;

    do {
        i++;
        if ((tree[i].ob_type & 0xff) == G_STRING)
        {
            if ((tree[i].ob_state & OS_DISABLED) && (tree[i].ob_spec.free_string[0] == '-'))
            {
                tree[i].ob_type = (tree[i].ob_type << 8) + G_USERDEF;
                tree[i].ob_spec.userblk = &menu_blk;
            }
        }
    }
    while (!(tree[i].ob_flags & OF_LASTOB));
}

/*
 * Menu-tuning: divider lines instead of disabled dashes
 * (ST-Computer 3/92, p. 87)
 */
static short draw_menuline(PARMBLK *p)
{
    short pxy[4];

    vs_clip(handle, 0, pxy);

    pxy[0] = p->pb_x;
    pxy[1] = p->pb_y + (p->pb_h / 2) - 1;
    pxy[2] = p->pb_x + p->pb_w - 1;
    pxy[3] = p->pb_y + (p->pb_h / 2);

    vsf_interior(handle, FIS_PATTERN);
    vsf_style(handle, 4);
    vsf_color(handle, G_BLACK);
    vr_recfl(handle, pxy);

    return 0;
}

static void init_userdef(void)
{
    short work_out[57];
    short ext_out[57];

    handle = open_vwork(work_out, ext_out);
    menu_blk.ub_parm = 0;
    menu_blk.ub_code = draw_menuline;
}

