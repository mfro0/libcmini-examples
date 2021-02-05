/*
 * cpx_preinit.c
 *
 * gcc has a different calling convention than the one used by XCONTROL: while the latter pass short
 * parameters as 16-bit words on the stack (using cdecl calling convention), gcc passes 32 bits with
 * the upper word unused.
 * There's a trick we can use to generate the required calling convention with gcc nevertheless:
 * if we pass all the parameters as one single struct, that's exactly the format required.
 * This file does all the work so we do not need to deal with the calling convention in the CPX code
 * itself. Should work fully transparent as long as both sides *do not* change the XCPB contents after
 * initialization (shouldn't happen as there is normally no reason to do so).
 */

#include <gem.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// #define DEBUG
#ifdef DEBUG
//#include "natfeats.h"
#define dbg(format, arg...) do { printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

typedef struct
{
    short num_objs;
    short num_frstr;
    short num_frimg;
    short num_tree;
    OBJECT *rs_object;
    TEDINFO *rs_tedinfo;
    char **rs_strings;
    ICONBLK *rs_iconblk;
    BITBLK *rs_bitblk;
    long *rs_frstr;
    long *rs_frimg;
    long *rs_trindex;
    struct foobar *rs_imdope;
} RSH_FIX_PARAMS;

typedef struct
{
    OBJECT *tree;
    short curob;
} RSH_OBFIX_PARAMS;

typedef struct
{
    char **items;
    short num_items;
    short default_item;
    short font_size;
    GRECT *button;
    GRECT *world;
} POPUP_PARAMS;

typedef struct
{
    OBJECT *tree;
    short base;
    short slider;
    short num_items;
    short visible;
    short direction;
    short min_size;
} SL_SIZE_PARAMS;

typedef struct
{
    OBJECT *tree;
    short base;
    short slider;
    short value;
    short num_min;
    short num_max;
    void (*foo)(void);
} SL_X_PARAMS;

typedef struct
{
    OBJECT *tree;
    short base;
    short slider;
    short value;
    short num_min;
    short num_max;
    short (*foo)(void);
} SL_Y_PARAMS;

typedef struct
{
    OBJECT *tree;
    short base;
    short slider;
    short obj;
    short inc;
    short min;
    short max;
    short *numvar;
    short direction;
    void (*foo)(void);
} SL_ARROW_PARAMS;

typedef struct
{
    OBJECT *tree;
    short base;
    short slider;
    short min;
    short max;
    short *numvar;
    void (*foo)(void);
} SL_DRAGX_PARAMS;

typedef struct
{
    OBJECT *tree;
    short base;
    short slider;
    short min;
    short max;
    short *numvar;
    short (*foo)(void);
} SL_DRAGY_PARAMS;

typedef struct
{
    OBJECT *tree;
    short startob;
    short *puntmsg;
} XFORM_DO_PARAMS;

typedef struct
{
    short mask;
    MOBLK *m1;
    MOBLK *m2;
    long time;
} SET_EVNT_MASK_PARAMS;

typedef struct
{
    short id;
} XGEN_ALERT_PARAMS;

typedef struct
{
    short saveit;
    MFORM *mf;
} MFSAVE_PARAMS;

/*
 * this is the XCPB as seen from the XCONTROL side
 */
typedef struct
{
    short handle;
    short booting;
    short reserved;
    short SkipRshFix;
    void *reserve1;
    void *reserve2;
    void (*rsh_fix)(RSH_FIX_PARAMS);
    void (*rsh_obfix)(RSH_OBFIX_PARAMS);
    short (*Popup)(POPUP_PARAMS);
    void (*Sl_size)(SL_SIZE_PARAMS);
    void (*Sl_x)(SL_X_PARAMS);
    void (*Sl_y)(SL_Y_PARAMS);
    void (*Sl_arrow)(SL_ARROW_PARAMS);
    void (*Sl_dragx)(SL_DRAGX_PARAMS);
    void (*Sl_dragy)(SL_DRAGY_PARAMS);
    short (*Xform_do)(XFORM_DO_PARAMS params);
    GRECT *(*GetFirstRect)(GRECT *prect);
    GRECT *(*GetNextRect)(void);
    void (*Set_Evnt_Mask)(SET_EVNT_MASK_PARAMS);
    short (*XGen_Alert)(XGEN_ALERT_PARAMS);
    short (*CPX_Save) (void *ptr, long num);
    void *(*Get_Buffer)(void );
    short (*getcookie) (long cookie, long *p_value);
    short Country_Code;
    void (*MFsave)(MFSAVE_PARAMS);
} XCPB_XCONTROL;

/*
 * this is the XCPB as as seen from the gcc side
 */
typedef struct
{
    short handle;
    short booting;
    short reserved;
    short SkipRshFix;
    void *reserve1;
    void *reserve2;

    void (*rsh_fix)(short num_objs, short num_frstr, short num_frimg, short num_tree,
                    OBJECT *rs_object, TEDINFO *rs_tedinfo, char **rs_strings, ICONBLK *rs_iconblk,
                    BITBLK *rs_bitblk, long *rs_frstr, long *rs_frimg, long *rs_trindex, struct foobar *rs_imdope);
    void (*rsh_obfix)(OBJECT *tree, short curobj);
    short (*Popup)(char **items, short num_items, short default_item, short font_size, GRECT *button, GRECT *world);

    void (*Sl_size)(OBJECT *tree, short base, short slider, short num_items,
                    short visible, short direction, short min_size);
    void (*Sl_x)(OBJECT *tree, short base, short slider, short value,
                       short num_min, short num_max, void (*foo)(void));
    void (*Sl_y)(OBJECT *tree, short base, short slider,  short value,
                       short num_min, short num_max, short (*foo)(void));
    void (*Sl_arrow)(OBJECT *tree, short base, short slider, short obj,
                     short inc, short min, short max, short *numvar,
                     short direction, void (*foo)(void));
    void (*Sl_dragx)(OBJECT *tree, short base, short slider,
                     short min, short max, short *numvar, void (*foo)(void));
    void (*Sl_dragy)(OBJECT *tree, short base, short slider,
                     short min, short max, short *numvar, short (*foo)(void));
    short (*Xform_do)(OBJECT *tree, short startobj, short *puntmsg);
    GRECT *(*GetFirstRect)(GRECT *prect);
    GRECT *(*GetNextRect)(void);
    void (*Set_Evnt_Mask)(short mask, MOBLK *m1, MOBLK *m2, long time );
    short (*XGen_Alert)(short id);
    short (*CPX_Save)(void *ptr, long num);
    void *  (*Get_Buffer)(void);
    short (*getcookie)(long cookie, long *p_value);
    short Country_Code;
    void (*MFsave)(short saveit, MFORM *mf);
} XCPB;

static XCPB fxcpb;
static XCPB *gcc_xcpb = &fxcpb;
static XCPB_XCONTROL *gxcpb;


static void rsh_fix(short num_objs, short num_frstr, short num_frimg,
        short num_tree, OBJECT *rs_object,
        TEDINFO *rs_tedinfo, char *rs_strings[],
        ICONBLK *rs_iconblk, BITBLK *rs_bitblk,
        long *rs_frstr, long *rs_frimg, long *rs_trindex,
        struct foobar *rs_imdope)
{
    (*gxcpb->rsh_fix)((RSH_FIX_PARAMS) { num_objs, num_frstr, num_frimg, num_tree, rs_object, rs_tedinfo,
                                          rs_strings, rs_iconblk, rs_bitblk, rs_frstr, rs_frimg, rs_trindex, rs_imdope });
}

static void rsh_obfix(OBJECT *tree, short curob)
{
    (*gxcpb->rsh_obfix)((RSH_OBFIX_PARAMS) { tree, curob });
}

static short Popup(char *items[], short num_items, short default_item,
                  short font_size, GRECT *button, GRECT *world)
{
    return (*gxcpb->Popup)((POPUP_PARAMS) { items, num_items, default_item, font_size, button, world });
}

static void Sl_size(OBJECT *tree, short base, short slider, short num_items,
                    short visible, short direction, short min_size)
{
    (*gxcpb->Sl_size)((SL_SIZE_PARAMS) { tree, base, slider, num_items, visible, direction, min_size });
}

static void Sl_x(OBJECT *tree, short base, short slider, short value,
                 short num_min, short num_max, void (*foo)(void))
{
    (*gxcpb->Sl_x)((SL_X_PARAMS) { tree, base, slider, value,
                                   num_min, num_max, foo });
}

static void Sl_y(OBJECT *tree, short base, short slider, short value,
                 short num_min, short num_max, short (*foo)(void))
{
    (*gxcpb->Sl_y)((SL_Y_PARAMS) { tree, base, slider, value,
                                   num_min, num_max, foo });
}

static void Sl_arrow(OBJECT *tree, short base, short slider, short obj,
                     short inc, short min, short max, short *numvar,
                     short direction, void (*foo)(void))
{
    (*gxcpb->Sl_arrow)((SL_ARROW_PARAMS) { tree, base, slider, obj,
                                           inc, min, max, numvar,
                                           direction, foo });
}

static void Sl_dragx(OBJECT *tree, short base, short slider,
                     short min, short max, short *numvar, void (*foo)(void))
{
    (*gxcpb->Sl_dragx)((SL_DRAGX_PARAMS) { tree, base, slider,
                       min, max, numvar, foo });
}

static void Sl_dragy(OBJECT *tree, short base, short slider,
                     short min, short max, short *numvar, short (*foo)(void))
{
    (*gxcpb->Sl_dragy)((SL_DRAGY_PARAMS) { tree, base, slider,
                       min, max, numvar, foo });
}

static short Xform_do(OBJECT *tree, short startob, short *puntmsg)
{
    return (*gxcpb->Xform_do)((XFORM_DO_PARAMS) { tree, startob, puntmsg });
}

static void Set_Evnt_Mask(short mask, MOBLK *m1, MOBLK *m2, long time)
{
    (*gxcpb->Set_Evnt_Mask)((SET_EVNT_MASK_PARAMS) { mask, m1, m2, time });
}

static short XGen_Alert(short id)
{
    return (*gxcpb->XGen_Alert)((XGEN_ALERT_PARAMS) { id });
}

static void MFsave(short saveit, MFORM *mf)
{
    (*gxcpb->MFsave)((MFSAVE_PARAMS) { saveit, mf });
}

XCPB *cpx_preinit(XCPB_XCONTROL *xcpb)
{
    static bool once = false;

    if (!once)                  /* this might need to change once we find an XControl that changes the XCB on the fly */
    {
        once = true;

        gxcpb = xcpb;
        /* make both structs identical */
        memcpy(gcc_xcpb, xcpb, sizeof(XCPB));

        dbg("original xcpb=%p, fake xcpb=%p\r\n",
            (void *) gxcpb, (void *) gcc_xcpb);
        /*
     * now replace all functions that have one or more shorts in their parameter list
     * with our intermediates
     */
        gcc_xcpb->rsh_fix = &rsh_fix;
        gcc_xcpb->rsh_obfix = &rsh_obfix;
        gcc_xcpb->Popup = &Popup;
        gcc_xcpb->Sl_size = &Sl_size;
        gcc_xcpb->Sl_x = &Sl_x;
        gcc_xcpb->Sl_y = &Sl_y;
        gcc_xcpb->Sl_arrow = &Sl_arrow;
        gcc_xcpb->Sl_dragx = &Sl_dragx;
        gcc_xcpb->Sl_dragy = &Sl_dragy;
        gcc_xcpb->Xform_do = &Xform_do;
        gcc_xcpb->Set_Evnt_Mask = &Set_Evnt_Mask;
        gcc_xcpb->XGen_Alert = &XGen_Alert;
        gcc_xcpb->MFsave = &MFsave;
    }
    gxcpb->booting = xcpb->booting;

    return gcc_xcpb;
}
