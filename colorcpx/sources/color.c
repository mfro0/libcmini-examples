/* ======================================================================
 * FILE: COLOR.C
 * ======================================================================
 * DATE: February 4, 1992 - created Color Version 2.0
 *			  - from work by m.lai from the TT color cpx
 *
 * DESCRIPTION: SPARROW VERSION - COLOR CPX
 * COMPILER: TURBO C Version 2.0
 * 02/07/92 cjg - always display the sliders, even in monochrome.
 * 02/10/92 cjg - converted to lattice C v5.0
 * 02/11/92 cjg - added dialog box when double-clicking on sliders.
 * 02/12/92 cjg - removed dialog box
 *		- work-around a bug with editable text fields
 * 03/20/92 cjg - when the RGB sliders are being manipulated, force
 *		  a redraw of the selected pen box. This makes it
 *		  work in True Color.
 * 03/24/92 cjg - Reload, Undo, and ClrHome will force a redraw of all
 *		  the boxes. This makes it work in True Color.
 * 07/09/92 cjg - If AES Version >= 3.2, then use MFsave, else skip
 * 07/24/92 cjg - Make 3D
 * 08/12/92 cjg - Make 3D - Expand OutWards....
 *		- California, the IOU State, DAY 43
 * 		- Do_Slider() - Check if the mouse moves vertically
 *				before entering sl_dragy()
 * 09/22/92 cjg - Handle 3D code for RAM-resident mode
 * 01/14/93 cjg - Handle New 3D format with ObFlags()
 */


/* INCLUDE FILES
 * ======================================================================
 */

#include <string.h>
#include <osbind.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "portab.h"

#include "color.rsh"
#include "cpxdata.h"
#include "country.h"
#include "color.h"

#include <gemx.h>

#define DEBUG
#ifdef DEBUG
//#include "natfeats.h"
#define dbg(format, arg...) do { printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#define out(format, arg...) do { printf("" format, ##arg); } while (0)
#endif /* DEBUG */

/* DEFINES
 * ======================================================================
 */


/* State of a color */
#define	DIRTY   0x01                /* color has been modified */
#define	CLEAN   0x00                /* color has NOT been modified */

#define MONOCHROME  2
#define MAX_COL_SHOWN   16
#define COL_PER_ROW 4
#define	COL_PER_BNK 16              /* number of colors per bank */
#define	NUMREG      256


/* RGB gun value related */
#define VAL_PAGE    50              /* pager increment */


/* Key codes */
#define	UNDO    0x6100      /* Undo key */
#define	HOME    0x4700      /* Clr Home Key */


/* Palette size */
#define	MONO   	2           /* monochrome */


/* Indices into array of RGB gun values for VDI */
#define	R   0               /* red gun value */
#define	G   1               /* green gun value */
#define	B   2               /* blue gun value */

/* Flags */
#define	HILITE      0       /* highlight an object */
#define	DEHILITE    1       /* de-highlight an object */




/* Structure for RGB gun values of each color */
typedef struct
{
    short rint;             /* red intensity */
    short gint;             /* green intensity */
    short bint;             /* blue intensity */
} RGB;


/* Structure for variables saved at data segment of CPX */
typedef struct
{
    union
    {
        RGB num256[NUMREG];     /* for 256 pen modes   */
        RGB num16[COL_PER_BNK]; /* for all other modes */
    } bank;
} DEFAULTS;



/* PROTOTYPES
 * ======================================================================
 */
CPXINFO *cpx_init(XCPB *Xcpb);
short cpx_call(GRECT *rect);

void open_vwork(void);
void close_vwork(void);

void draw_boxes(void);
void outline(OBJECT *tree, short obj, short flag);
void update_slid(OBJECT *tree, short base, short slider, short value,
                 short min, short max, short draw);
short slidtext(void);
void myitoa(short inword, char *numbuf);
void update_rgb(short draw);

void do_redraw(GRECT *dirty_rect, short *oldclip);

void nxtrow(void);
void nxtpage(void);
short nxtgrp(void);
void nxt_to_show(short toscroll);
void adjcol(void);

void init(DEFAULTS *info);
void slamcol(RGB col_array[]);
void visit_bnk(void);

void cnclchgs(void);
void cnclbnk(void);
void savergb(void);
void cpyrgb(RGB *dest, RGB *src, short count);
void do_rgb(short slider, short base, short index);

void Do_Up(OBJECT *tree, short base, short slider, short button, short index);
void Do_Down(OBJECT *tree, short base, short slider, short button, short index);
void Do_Slider(OBJECT *tree, short base, short slider, short index, bool dclick);
void Do_Base(OBJECT *tree, short base, short slider, short index);

void XSelect(OBJECT *tree, short button);
void XDeselect(OBJECT *tree, short button);

void MakeIndicator(OBJECT *tree, short obj);
void MakeActivator(OBJECT *tree, short obj);
void Do3D( void );
void MakeTed(OBJECT *tree, short obj);


/* EXTERNALS
 * ======================================================================
 */
extern int      saved;      /* 0: no user-preference saved yet */
extern DEFAULTS usr_vals;   /* saved user-preference */
extern DEFAULTS def_vals;   /* system defaults */


/* GLOBALS
 * ======================================================================
 */
XCPB *xcpb;             /* XControl Parameter Block   */
CPXINFO cpxinfo;        /* CPX Information Structure  */

OBJECT *ad_tree;        /* Main cpx tree...           */
OBJECT *ad_slide1;      /* editable text field - pen  */
OBJECT *ad_slide2;      /* editable text field - rgb  */

RGB	oldrgb[NUMREG],     /* old RGBs for all colors */
    *curold = NULL,     /* pointer to old RGBs for current bank */
    newrgb[NUMREG],     /* new RGBs for all colors */
    *curnew = NULL;     /* pointer to new RGBs for current bank */

short currez;           /* current resolution */
short headbox,          /* object number of 1st color box */
      headcol = 0;      /* color # of 1st color box */
short curcol,           /* current color number */
      curbox,           /* current color box selected */
      curslid;          /* current slider */
short col_min = 0,      /* smallest color number */
      col_max = 0,      /* biggest color number */
      col_page;         /* color pager value */
short curscrn[3],       /* current RGB gun values on screen */
      oldscrn[3];       /* old RGB gun values on screen */
short numcol = 0;       /* # colors that can be displayed at once */
char    setup_bnk,
    touch_bnk,
    dirt_col[NUMREG],   /* dirty list for all colors */
    *curdirt = NULL;    /* dirty list for current bank of colors */

char    PenNum[10];     /* text buffer for Pen Number arrows */


/* VDI arrays */
short contrl[12],
      intin[128],
      intout[128],
      ptsin[128],
      ptsout[128],
      work_in[12],
      work_out[57];
short pxyarray[10];     /* input point array */
short vhandle = -1;     /* virtual workstation handle */
short hcnt = 0;         /* handle count */

/* AES variables */
short gl_hchar, gl_wchar, gl_hbox, gl_wbox;
GRECT desk;
MFORM orig_mf;          /* original mouse form */


/* FUNCTIONS
 * ======================================================================
 */

static short min(short a, short b)
{
    return (a < b ? a : b);
}

static short max(short a, short b)
{
    return (a > b ? a : b);
}

static void rc_2xy(GRECT *rect, short *xy)
{
    xy[0] = rect->g_x;
    xy[1] = rect->g_y;
    xy[2] = rect->g_x + rect->g_w - 1;
    xy[3] = rect->g_y + rect->g_h - 1;
}

/* cpx_init()
 * ======================================================================
 * cpx_init() is where a CPX is first initialized.
 * There are TWO parts.
 *
 * PART ONE: cpx_init() is called when the CPX is first 'booted'. This is
 *	     where the CPX should read in its defaults and then set the
 *	     hardware based on those defaults.
 *
 * PART TWO: The other time cpx_init() is called is when the CPX is being
 *	     executed.  This is where the resource is fixed up and current
 *	     cpx variables are updated from the hardware.  In addition,
 *	     this is where the CPXINFO structure should be filled out.
 *
 * IN:  XCPB	*Xcpb:	Pointer to the XControl Parameter Block
 * OUT: CPXINFO  *ptr:	Pointer to the CP Information Structure
 */

CPXINFO *cpx_init(XCPB *Xcpb)
{
    dbg("before appl_init()\n");

    appl_init();

    xcpb = Xcpb;

    currez = Getrez();          /* find current resolution */
    open_vwork();
    close_vwork();

    dbg("opened and closed virtual workstation\n");

    numcol = work_out[13];      /* size of CLUT */

    if (xcpb->booting)
    {
        if (saved)              /* if user-preference saved */
            init(&usr_vals);    /* init to saved user-preference */
        else
            init(&def_vals);    /* init to default values */

        return (CPXINFO *) true;
    }
    else
    {
        appl_init();

        dbg("AES version reported=%d\n", _AESversion);

        /*
         * can't use rsh_fix here as INTRFACE (which was used to create the RSC)
         * prepares a finished object structure in memory while
         * rsh_fix() expects a serialized structure
         */


        for (short i = 0; i < NUM_OBS; i++)
            rsrc_obfix(rs_object, i);

        ad_tree   = (OBJECT *) rs_trindex[COLOR];
        ad_slide1 = (OBJECT *) rs_trindex[SLIDE1];
        ad_slide2 = (OBJECT *) rs_trindex[SLIDE2];

        ad_slide1[ROOT].ob_x = ad_slide1[ROOT].ob_y = 0;
        ad_slide1[ROOT].ob_width = work_out[0] - 1;
        ad_slide1[ROOT].ob_height = work_out[1] - 1;
        ad_slide1[EXIT1].ob_width = ad_slide1[EXIT1].ob_height = 0;

        ad_slide2[ROOT].ob_x = ad_slide2[ROOT].ob_y = 0;
        ad_slide2[ROOT].ob_width = work_out[0] - 1;
        ad_slide2[ROOT].ob_height = work_out[1] - 1;
        ad_slide2[EXIT2].ob_width = ad_slide2[EXIT2].ob_height = 0;


        ad_tree[BASE2].ob_flags |= OF_HIDETREE;
        ad_tree[BASE4].ob_flags |= OF_HIDETREE;
        ad_tree[BASE16].ob_flags |= OF_HIDETREE;

        /* set up color boxes on screen */
        switch (numcol)
        {
            case 2:
                headbox = curbox = TWO01;
                col_page = 1;
                ad_tree[BASE2].ob_flags &= ~OF_HIDETREE;
                break;

            case 4:
                headbox = curbox = FOUR01;
                col_page = 1;
                ad_tree[BASE4].ob_flags &= ~OF_HIDETREE;
                break;

            default:
                headbox = curbox = BOX0;

                if (numcol == 16)               /* if 16 colors */
                    col_page = 2;               /* page by 2 colors */
                else                            /* if > 16 colors */
                    col_page = MAX_COL_SHOWN;   /* page by max displayed */
                ad_tree[BASE16].ob_flags &= ~OF_HIDETREE;
                break;
        }


        /* set up color # slider */
        headcol = curcol = 0;           /* init color on screen */
        setup_bnk = touch_bnk = CLEAN;
        visit_bnk();                    /* init data structures of current bank */

        curscrn[R] = curscrn[G] = curscrn[B] = -1;
        update_rgb(0);                  /* init RGBs on screen */
        col_max = numcol - 1;
        (*xcpb->Sl_size)(ad_tree, CBASE, CSLIDE, numcol, 1, VERTICAL, 16);
        update_slid(ad_tree, CBASE, CSLIDE, curcol, col_max, col_min, 0 );


        /* Initialize the CPXINFO structure */
        cpxinfo.cpx_call    = cpx_call;
        cpxinfo.cpx_draw    = NULL;
        cpxinfo.cpx_wmove   = NULL;
        cpxinfo.cpx_timer   = NULL;
        cpxinfo.cpx_key     = NULL;
        cpxinfo.cpx_button  = NULL;
        cpxinfo.cpx_m1      = NULL;
        cpxinfo.cpx_m2      = NULL;
        cpxinfo.cpx_hook    = NULL;
        cpxinfo.cpx_close   = NULL;

        /* Convert to 3D if possible - ONLY if AES 0x0400 or Greater!
         * AND if # of pens is greater than LWHITE
         */
        if (!xcpb->SkipRshFix)
        {
            if ((_AESversion >= 0x0330) && (numcol > LWHITE))
            {
                Do3D();
            }
        }
        /* Return the pointer to the CPXINFO structure to XCONTROL */
        return &cpxinfo;
    }
}




/* cpx_call()
 * ======================================================================
 * Called ONLY when the CPX is being executed. Note that it is CPX_INIT()
 * that returned the ptr to cpx_call().
 * CPX_CALL() is the entry point to displaying and manipulating the
 * dialog box.
 *
 * IN: GRECT *rect:	Ptr to a GRECT that describes the current work
 *			area of the XControl window.
 *
 * OUT:
 *   FALSE:     The CPX has exited and no more messages are
 *		needed.  XControl will either return to its
 *		main menu or close its windows.
 *		This is used by XForm_do() type CPXs.
 *
 *   TRUE:	The CPX requests that XCONTROL continue to
 *		send AES messages.  This is used by Call-type CPXs.
 */
short cpx_call(GRECT *rect)
{
    short button;
    bool quit = 0;
    WORD msg[8];
    MRETS mk;
    short ox, oy;
    bool dclick;
    short CurPen;
    GRECT orect;

    wind_get_grect(0, WF_WORKXYWH, &desk);   /* set clipping to */
    short clip[4] = { desk.g_x, desk.g_y, desk.g_x + desk.g_w - 1, desk.g_y + desk.g_h - 1 };

    open_vwork();
    vs_clip(vhandle, 1, clip);

    ad_tree[ROOT].ob_x = rect->g_x;
    ad_tree[ROOT].ob_y = rect->g_y;
    objc_draw(ad_tree, ROOT, MAX_DEPTH, ad_tree[ROOT].ob_x,ad_tree[ROOT].ob_y,
              ad_tree[ROOT].ob_width, ad_tree[ROOT].ob_height);
    draw_boxes();                      /* draw the color boxes */
    outline(ad_tree, curbox, HILITE);           /* show selected box */
    close_vwork();


    do
    {
        dclick = false;
        button = (*xcpb->Xform_do)(ad_tree, 0, msg);

        if ((button != -1) && (button & 0x8000))
        {
            dclick = true;
            button &= 0x7fff;
        }

        switch (button)
        {
            case CCANCEL:
                cnclchgs();                 /* cancel all changes made */

            case COK:
                quit = true;
                ad_tree[button].ob_state &= ~OS_SELECTED;
                break;

            case CRELOAD:                   /* reload saved user-preference */
                if (saved)
                    init(&usr_vals);
                else
                    init(&def_vals);
                update_rgb(1);              /* update RGB on screen */

                /* Redraw the boxes deliberately */
                open_vwork();
                vs_clip(vhandle, 1, clip);
                draw_boxes();               /* draw the color boxes */
                close_vwork();


                XDeselect(ad_tree, button);
                break;


            case CSAVE:	/* save current values */
                if ((*xcpb->XGen_Alert)(SAVE_DEFAULTS) == true)
                {
                    (*xcpb->MFsave)(MFSAVE, &orig_mf);
                    if (_AESversion >= 0x0320)
                        graf_mouse(BUSYBEE, 0L);

                    savergb();              /* save color info */
                    /* update RGBs */
                    cpyrgb((RGB *) &oldrgb, (RGB *) &newrgb, numcol);

                    /* save current data to file */
                    saved = 1;
                    (*xcpb->CPX_Save)((void *) &saved, sizeof(DEFAULTS) + 2);
                    (*xcpb->MFsave)(MFRESTORE, &orig_mf);
                }
                XDeselect(ad_tree, button );
                break;


            case CUP:
                (*xcpb->Sl_arrow)(ad_tree, CBASE, CSLIDE, CUP, -1,
                                  col_max, col_min, &curcol, VERTICAL, nxtrow);
                break;

            case CDOWN:
                (*xcpb->Sl_arrow)(ad_tree, CBASE, CSLIDE, CDOWN, 1,
                                  col_max, col_min, &curcol, VERTICAL, nxtrow);
                break;


            case CSLIDE:
                if (dclick)     /* double click allows editing the value */
                {
                    orect = * (GRECT *) &ad_tree[CSLIDE].ob_x;
                    objc_offset(ad_tree, CSLIDE, &orect.g_x, &orect.g_y);

                    ad_slide1[CTSLIDE].ob_x = ad_slide1[EXIT1].ob_x = orect.g_x - 2;
                    ad_slide1[CTSLIDE].ob_y = ad_slide1[EXIT1].ob_y = orect.g_y - 2;
                    ad_slide1[CTSLIDE].ob_width = orect.g_w + 3;
                    ad_slide1[CTSLIDE].ob_height = orect.g_h + 4;


                    myitoa(curcol, &PenNum[0]);
                    ad_slide1[CTSLIDE].ob_spec.tedinfo->te_ptext = PenNum;

                    objc_draw(ad_slide1, CTSLIDE, 0, ad_slide1[CTSLIDE].ob_x, ad_slide1[CTSLIDE].ob_y,
                              ad_slide1[CTSLIDE].ob_width, ad_slide1[CTSLIDE].ob_height);
                    form_do(ad_slide1, CTSLIDE);

                    if (strlen(ad_slide1[CTSLIDE].ob_spec.tedinfo->te_ptext))
                    {
                        CurPen = atoi(ad_slide1[CTSLIDE].ob_spec.tedinfo->te_ptext);
                        CurPen = min(CurPen, col_max);
                        CurPen = max(CurPen, col_min);
                        curcol = CurPen;
                        ad_slide1[CTSLIDE].ob_state &= ~OS_SELECTED;
                        ad_slide1[EXIT1].ob_state &= ~OS_SELECTED;

                        objc_draw(ad_tree, CSLIDE, MAX_DEPTH, ad_tree[ROOT].ob_x, ad_tree[ROOT].ob_y,
                                                              ad_tree[ROOT].ob_width, ad_tree[ROOT].ob_height);

                        headcol = ( curcol / COL_PER_BNK )* COL_PER_BNK;
                        open_vwork();
                        rc_2xy(&desk, clip);
                        vs_clip(vhandle, 1, clip);
                        draw_boxes();
                        close_vwork();
                        nxtgrp();
                        update_slid(ad_tree, CBASE, CSLIDE, curcol, col_max, col_min, 1 );
                    }
                    else
                    {
                        ad_slide1[CTSLIDE].ob_state &= ~OS_SELECTED;
                        ad_slide1[EXIT1].ob_state &= ~OS_SELECTED;
                        objc_draw(ad_tree, CSLIDE, 0, ad_tree[CSLIDE].ob_x, ad_tree[CSLIDE].ob_y,
                                                      ad_tree[CSLIDE].ob_width, ad_tree[CSLIDE].ob_height);
                    }
                    evnt_button(1, 1, 0, &mk.x, &mk.y, &mk.buttons, &mk.kstate);
                }
                else
                {
                    (*xcpb->MFsave)(MFSAVE, &orig_mf);
                    if (_AESversion >= 0x0320)
                        graf_mouse( FLAT_HAND, 0L );

                    if ((_AESversion >= 0x0330) && (numcol > LWHITE))
                        XSelect(ad_tree, CSLIDE );

                    (*xcpb->Sl_dragy)(ad_tree, CBASE, CSLIDE, col_max,
                                      col_min, &curcol, nxtgrp);

                    if ((_AESversion >= 0x0330 ) && (numcol > LWHITE))
                        XDeselect(ad_tree, CSLIDE);
                    (*xcpb->MFsave)(MFRESTORE, &orig_mf);
                }
                break;

            case CBASE:
                graf_mkstate(&mk.x, &mk.y, &mk.buttons, &mk.kstate);
                objc_offset(ad_tree, CSLIDE, &ox, &oy);

                if(mk.y < oy)
                    oy = -col_page;
                else
                    oy = col_page;

                curslid = CSLIDE;
                (*xcpb->Sl_arrow)(ad_tree, CBASE, CSLIDE, -1, oy, col_max, col_min, &curcol, VERTICAL, nxtpage);

                break;


            case RUP:
                Do_Up(ad_tree, RBASE, RSLIDE, RUP, R);
                break;

            case RDOWN:
                Do_Down(ad_tree, RBASE, RSLIDE, RDOWN, R);
                break;

            case RSLIDE:
                Do_Slider(ad_tree, RBASE, RSLIDE, R, dclick);
                break;

            case RBASE:
                Do_Base(ad_tree, RBASE, RSLIDE, R);
                break;

            case GUP:
                Do_Up(ad_tree, GBASE, GSLIDE, GUP, G);
                break;

            case GDOWN:
                Do_Down(ad_tree, GBASE, GSLIDE, GDOWN, G);
                break;

            case GSLIDE:
                Do_Slider(ad_tree, GBASE, GSLIDE, G, dclick);
                break;

            case GBASE:
                Do_Base(ad_tree, GBASE, GSLIDE, G);
                break;

            case BUP:
                Do_Up(ad_tree, BBASE, BSLIDE, BUP, B);
                break;

            case BDOWN:
                Do_Down(ad_tree, BBASE, BSLIDE, BDOWN, B);
                break;

            case BSLIDE:
                Do_Slider(ad_tree, BBASE, BSLIDE, B, dclick);
                break;

            case BBASE:
                Do_Base(ad_tree, BBASE, BSLIDE, B);
                break;


            case TWO01:         /* for ST High rez */
            case TWO02:

            case FOUR01:        /* for ST Medium rez */
            case FOUR02:
            case FOUR03:
            case FOUR04:

            case BOX0:          /* for all other rez */
            case BOX1:
            case BOX2:
            case BOX3:
            case BOX4:
            case BOX5:
            case BOX6:
            case BOX7:
            case BOX8:
            case BOX9:
            case BOX10:
            case BOX11:
            case BOX12:
            case BOX13:
            case BOX14:
            case BOX15:
                if (button != curbox)           /* select requested color */
                {
                    curcol = button - headbox + headcol;
                    nxt_to_show(0);
                    update_slid(ad_tree, CBASE, CSLIDE, curcol, col_max, col_min, 1);
                }
                break;

            default:
                if (button == -1)
                {
                    switch (msg[0])
                    {
                        case WM_REDRAW:                 /* redraw the cpx */
                            do_redraw((GRECT *) &msg[4], clip);
                            break;

                        case AC_CLOSE:                  /* treated like a cancel */
                            cnclchgs();                 /* cancel changes made */

                        case WM_CLOSED:
                            quit = true;                /* treated like an OK */
                            break;

                        case CT_KEY:
                            switch (msg[3])             /* check which key is returned */
                            {
                                case UNDO:              /* if Undo key */
                                    cnclbnk();          /* cancel color changes */
                                    update_rgb(1);      /* update RGB on screen */

                                    /* Redraw the boxes deliberately */
                                    open_vwork();
                                    rc_2xy(&desk, clip);    /*   Desktop space */
                                    vs_clip(vhandle, 1, clip);
                                    draw_boxes();       /* draw the color boxes */
                                    close_vwork();

                                    break;

                                case HOME:                  /* if Clr Home key */
                                    init(&def_vals);        /* init to system defs */
                                    update_rgb(1);          /* update RGB on screen */

                                    setup_bnk = CLEAN;      /*cjg*/
                                    visit_bnk();            /*cjg*/

                                    /* Redraw the boxes deliberately */
                                    open_vwork();
                                    rc_2xy(&desk, clip);  /*   Desktop space */
                                    vs_clip(vhandle, 1, clip);
                                    draw_boxes();           /* draw the color boxes */
                                    close_vwork();

                                    break;
                            }

                            break;

                        default:
                            break;
                    }
                }
                break;
        }
    } while(!quit);
    return false;
}



/*
 * Open virtual workstation
 */
void open_vwork(void)
{
  int i;

  if (hcnt == 0) {
    for (i = 1; i < 10;)
        work_in[i++] = 1;
    work_in[0] = currez + 2;
    work_in[10] = 2;
    vhandle = xcpb->handle;
    v_opnvwk(work_in, &vhandle, work_out);
  }
  hcnt++;
}


/*
 * Close virtual workstation
 */
void close_vwork(void)
{
    hcnt--;
    if (!hcnt)
    {
        v_clsvwk(vhandle);
        vhandle = -1;
    }
}


/*
 * Draw the color boxes
 */
void draw_boxes(void)
{
    GRECT obrect;
    short obj, objcol;
    short lastbox;

    wind_update(BEG_UPDATE);
    graf_mouse(M_OFF, 0L);

    if (numcol < MAX_COL_SHOWN)                 /* init last box to be drawn */
        lastbox = headbox + numcol - 1;
    else
        lastbox = headbox + MAX_COL_SHOWN - 1;

    vsf_interior(vhandle, FIS_SOLID);           /* fill with SOLID pattern */
    for (obj = headbox, objcol = headcol; obj <= lastbox; obj++, objcol++)
    {
        vsf_color(vhandle, objcol);             /* fill with color of obj */
        obrect = * (GRECT *) &ad_tree[obj].ob_x;
        objc_offset(ad_tree, obj, &obrect.g_x, &obrect.g_y);

        pxyarray[0] = obrect.g_x;
        pxyarray[1] = obrect.g_y;
        pxyarray[2] = obrect.g_x + obrect.g_w - 1;
        pxyarray[3] = obrect.g_y + obrect.g_h - 1;
        v_bar(vhandle, pxyarray);
    }
    graf_mouse(M_ON, 0L);
    wind_update(END_UPDATE);
}


/*
 * Highlight or de-highlight a color box
 */
void outline(OBJECT *tree, short obj, short flag)
{
    short color;
    GRECT obrect;

    if (flag == HILITE)
        color = 1;              /* highlight box with foreground color */
    else
    {
        color = WHITE;          /* de-light box with background color */
    }
    wind_update(BEG_UPDATE);
    graf_mouse(M_OFF, 0L);
    vsl_color(vhandle, color);

    obrect = * (GRECT *) &tree[obj].ob_x;

    objc_offset(tree, obj, &obrect.g_x, &obrect.g_y);
    pxyarray[0] = pxyarray[2] = pxyarray[8] = obrect.g_x - 2;
    pxyarray[1] = pxyarray[7] = pxyarray[9] = obrect.g_y - 2;
    pxyarray[3] = pxyarray[5] = obrect.g_y + obrect.g_h + 1;
    pxyarray[4] = pxyarray[6] = obrect.g_x + obrect.g_w + 1;
    v_pline(vhandle, 5, pxyarray);

    pxyarray[0] = pxyarray[2] = pxyarray[8] = obrect.g_x - 3;
    pxyarray[1] = pxyarray[7] = pxyarray[9] = obrect.g_y - 3;
    pxyarray[3] = pxyarray[5] = obrect.g_y + obrect.g_h + 2;
    pxyarray[4] = pxyarray[6] = obrect.g_x + obrect.g_w + 2;
    v_pline(vhandle, 5, pxyarray);
    graf_mouse(M_ON, 0L);
    wind_update(END_UPDATE);
}


/*
 * Update and draw (if requested) a specified slider
 */
void update_slid(OBJECT *tree, short base, short slider, short value,
                 short min, short max, short draw)
{
    GRECT obrect = * (GRECT *) &tree[slider].ob_x;           /* location of slider */

    objc_offset(tree, slider, &obrect.g_x, &obrect.g_y);
    curslid = slider;

    (*xcpb->Sl_y)(tree, base, slider, value, min, max, slidtext);

    if (draw)       /* if requested to draw, draw the slider */
    {
        obrect.g_x -= 3;            /* account for outline */
        obrect.g_y -= 3;
        obrect.g_w += 6;
        obrect.g_h += 6;

        /* undraw old */
        objc_draw(tree, base, MAX_DEPTH, obrect.g_x, obrect.g_y, obrect.g_w, obrect.g_h);
        objc_offset(tree, slider, &obrect.g_x, &obrect.g_y);
        obrect.g_x -= 3;            /* account for outline */
        obrect.g_y -= 3;

        /* draw new */
        objc_draw(tree, base, MAX_DEPTH, obrect.g_x, obrect.g_y, obrect.g_w, obrect.g_h);
    }
}



/*
 * Update R, G, B, color or bank index on screen.
 */
short slidtext(void)
{
    switch (curslid) {

        case RSLIDE:
            myitoa(curscrn[R], ad_tree[curslid].ob_spec.tedinfo->te_ptext);
            break;

        case GSLIDE:
            myitoa(curscrn[G], ad_tree[curslid].ob_spec.tedinfo->te_ptext);
            break;

        case BSLIDE:
            myitoa(curscrn[B], ad_tree[curslid].ob_spec.tedinfo->te_ptext);
            break;

        case CSLIDE:
            myitoa(curcol, ad_tree[curslid].ob_spec.tedinfo->te_ptext);
            break;

        default:
            break;
    }
    return 0;
}


/*
 * Convert binary number to ascii value
 */
void myitoa(short inword, char *numbuf)
{
    short temp1, value;
    int i, j;
    char tmpbuf[10];
    char *ascbuf;

    ascbuf = numbuf;
    i = 0;                              /* if the value is non zero  */

    if (!inword)
        *ascbuf++ = '0';
    else
    {
        value = inword;
        while(value)
        {
            temp1 = value % 10;         /*  find the remainder	*/
            temp1 += 0x0030;            /*  convert to ASCII	*/
            tmpbuf[i++] = (char) temp1; /*  buffer is reverse	*/
            value = value / 10;
        }

        for (j = i-1; j >= 0; j--)      /* reverse it back	*/
            *ascbuf++ = tmpbuf[j];
    }

    *ascbuf = 0;                        /* end of string mark	*/
    return;
}



/*
 * Update and draw (if requested) text in the RGB sliders
 */
void update_rgb(short draw)
{
    RGB *ptr;                           /* ptr to RGB intensities of current color */

    /* Inquire the RGB intensities for current pen */
    ptr = curnew + curcol;
    open_vwork();
    vq_color(vhandle, curcol, 0, (short *) ptr);
    close_vwork();

    /* Update the RGB gun values for current pen */
    oldscrn[R] = curscrn[R];
    oldscrn[G] = curscrn[G];
    oldscrn[B] = curscrn[B];
    curscrn[R] = ptr->rint;
    curscrn[G] = ptr->gint;
    curscrn[B] = ptr->bint;

    /* Record old location of sliders, and update indices */
    /* on R, G and B sliders if necessary */
    if (oldscrn[R] != curscrn[R])
        update_slid(ad_tree, RBASE, RSLIDE, curscrn[R], 0, 1000, draw);

    if (oldscrn[G] != curscrn[G])
        update_slid(ad_tree, GBASE, GSLIDE, curscrn[G], 0, 1000, draw);

    if (oldscrn[B] != curscrn[B])
        update_slid(ad_tree, BBASE, BSLIDE, curscrn[B], 0, 1000, draw);
}



/*
 * Find and redraw all clipping rectangles
 */
void do_redraw(GRECT *dirty_rect, short *oldclip)
{
    GRECT *r1;
    WORD clip[4];
    GRECT r2;

        open_vwork();
        r1 = (*xcpb->GetFirstRect)(dirty_rect);
        while (r1)
        {
            r2 = *r1;
            /* set clipping rectangle */
            rc_2xy(&r2, clip);
            vs_clip(vhandle, 1, clip);
            draw_boxes();
            outline(ad_tree, curbox, HILITE);
            r1 = (*xcpb->GetNextRect)();
        }
        vs_clip(vhandle, 1, oldclip);	/* restore original clipping */
        close_vwork();
}


/*
 * Show the next row of colors
 * (when using arrows to scroll through color map)
 */
void nxtrow(void)
{
    nxt_to_show(COL_PER_ROW);
}


/*
 * Show the next page of colors
 * (when using base to page through color map)
 */
void nxtpage(void)
{
    nxt_to_show(MAX_COL_SHOWN);
}


/*
 * Show the next group
 * (when using slider to drag through color map)
 */
short nxtgrp(void)
{
    if (curcol < headcol)       /* dragging backwards */
        nxt_to_show(headcol - curcol);
    else                        /* dragging forward */
        nxt_to_show(curcol - headcol - MAX_COL_SHOWN + 1);

    return 0;
}


/*
 * Show the selected color of the appropiate line or page
 */
void nxt_to_show(short toscroll)
{
    short obj;
    short clip[4];

    open_vwork();
    rc_2xy(&desk, clip);
    vs_clip(vhandle, 1, clip );

    /* if current color is not shown, page accordingly to show it */
    if (curcol < headcol) {                         /* page backwards */
        if ((headcol -= toscroll) < 0)              /* update color to start with */
            headcol = 0;
        draw_boxes();                               /* redraw color boxes */
    } else if (curcol >= headcol + MAX_COL_SHOWN) { /* page forward */
        if ((headcol += toscroll) > numcol - MAX_COL_SHOWN)
            headcol = numcol - MAX_COL_SHOWN;
        draw_boxes();                               /* redraw color boxes */
    }

    /* deselect previous color and select current one */
    obj = curcol - headcol + headbox;
    if (obj != curbox) {
        outline(ad_tree, curbox, DEHILITE);
        outline(ad_tree, obj, HILITE);
        curbox = obj;                               /* update current box selected */
    }

    /* update color # and RGB sliders */
    curslid = CSLIDE;
    slidtext();                                     /* update color # ONLY */
    update_rgb(1);                                  /* update and draw RGB gun values */
    close_vwork();
}



/*
 * Adjust color of selected color pen with
 * RGB gun values requested.
 *
 * Force the redraw of the currently selected pen box.
 */
void adjcol(void)
{
    GRECT obrect;

    (curnew + curcol)->rint = curscrn[R];
    (curnew + curcol)->gint = curscrn[G];
    (curnew + curcol)->bint = curscrn[B];

    slidtext();
    open_vwork();

    vs_color(vhandle, curcol, (short *) &curnew[curcol]);

    /* cjg - force a redraw fo the curbox */
    vsf_color(vhandle, curcol);     /* fill with color of obj */
    obrect = * (GRECT *) &ad_tree[curbox].ob_x;
    objc_offset(ad_tree, curbox, &obrect.g_x, &obrect.g_y);

    short clip[4] = { obrect.g_x, obrect.g_y,
                      obrect.g_w - obrect.g_x - 1, obrect.g_h - obrect.g_y - 1 };
    vs_clip(vhandle, 1, clip);

    pxyarray[0] = obrect.g_x;
    pxyarray[1] = obrect.g_y;
    pxyarray[2] = obrect.g_x + obrect.g_w - 1;
    pxyarray[3] = obrect.g_y + obrect.g_h - 1;
    v_bar(vhandle, pxyarray);

    close_vwork();
    *(curdirt + curcol) = touch_bnk = DIRTY;
}



/*
 * Initialize system with given information.
 */
void init(DEFAULTS *info)
{
    open_vwork();

    switch (numcol)
    {
       case 1:
       case 2:
       case 4:
       case 16:
            slamcol( info->bank.num16 );
            break;

       case 256:
            slamcol( info->bank.num256 );
            break;

       default:
            slamcol( info->bank.num16 );
            break;
    }
    close_vwork();
 }


/*
 * Set VDI color lookup table with the given RGB intensities
 * Passed:
 *          RGB col_array[] - array of RGB intensities
 */
void slamcol(RGB col_array[])
{
    short i;		/* counter */

    open_vwork();
    for (i = 0; i < numcol; i++ )
    {
        if (col_array[i].rint == -1)
            vq_color(vhandle, i, 0, (short *) &col_array[i]);
        vs_color(vhandle, i, (short *) &col_array[i]);
    }
    close_vwork();
}


/*
 * Update all data structures of current bank
 */
void visit_bnk(void)
{
    short i;      /* counter */
    RGB *ptr;   /* temp pointer */

    /* init pointers to offset to current bank */
    curold = (RGB *) &oldrgb[0];            /* old RGBs of current bank */
    curnew = (RGB *) &newrgb[0];            /* new RGBs of current bank */
    curdirt = (char *) &dirt_col[0];        /* dirty list for current bank */

    /* if bank has never been setup before, fill data structures */
    if (setup_bnk == CLEAN)
    {
        open_vwork();
        for (i = 0, ptr = curnew; i < numcol; i++, ptr = curnew+i)
        {
            vq_color(vhandle, i, 0, (short *) ptr);
            *(curold + i)  = *ptr;
            *(curdirt + i) = CLEAN;
        }
        close_vwork();
        setup_bnk = DIRTY;      /* remember it's been setup */
    }

}



/*
 * Cancel changes made to any color
 */
void cnclchgs(void)
{
    if (touch_bnk == DIRTY)
    {
       visit_bnk();
       cnclbnk();
    }
}


/*
 * Cancel changes made to current bank
 */
void cnclbnk(void)
{
    short j;

    open_vwork();
    for (j = 0; j < numcol; j++)		/* for all colors */
    {
        if( * (curdirt + j ) == DIRTY )	/* if color modified */
        {
                        /* reset it */
            vs_color(vhandle, j, (short *)(curold + j));
            *(curnew + j) = *(curold + j);
        }
    }
    close_vwork();
}


/*
 * Save current RGB intensities of all colors for all banks
 * to their save area
 */
void
savergb(void)
{
    /* save RGBs of bank 0 */
    if (setup_bnk == CLEAN)
        visit_bnk();

    cpyrgb(usr_vals.bank.num256, newrgb, numcol);
}


/*
 * Copy RGB values from source buffer to destination buffer
 * for given number of colors.
 */
void cpyrgb(RGB *dest, RGB *src, short count)
{
    for (int i = 0; i < count; i++)
        *(dest + i) = *(src + i);
}




/*
 * Double clicked on an RGB slider
 */
void do_rgb(short slider, short base, short index)
{
    short CurValue;
    GRECT orect;
    MRETS mk;

    orect = * (GRECT *) &ad_tree[slider].ob_x;
    objc_offset(ad_tree, slider, &orect.g_x, &orect.g_y);


    ad_slide2[CSLIDERS].ob_x = ad_slide2[EXIT2].ob_x = orect.g_x -= 2;
    ad_slide2[CSLIDERS].ob_y = ad_slide2[EXIT2].ob_y = orect.g_y -= 2;
    ad_slide2[CSLIDERS].ob_width = orect.g_w += 3;
    ad_slide2[CSLIDERS].ob_height = orect.g_h += 4;

    myitoa(curscrn[index], &PenNum[0]);

    dbg("curscrn[index]=%d, PenNum=%s\n", curscrn[index], PenNum);

    ad_slide2[CSLIDERS].ob_spec.tedinfo->te_ptext = PenNum;

    objc_draw(ad_slide2, CSLIDERS, 0, ad_slide2[CSLIDERS].ob_x, ad_slide2[CSLIDERS].ob_y,
                                      ad_slide2[CSLIDERS].ob_width, ad_slide2[CSLIDERS].ob_height);
    form_do(ad_slide2, CSLIDERS);

    if (strlen(ad_slide2[CSLIDERS].ob_spec.tedinfo->te_ptext))
    {
        CurValue = (short) atoi(ad_slide2[CSLIDERS].ob_spec.tedinfo->te_ptext);
        CurValue = min(CurValue, 1000);
        CurValue = max(CurValue, 0);
        curscrn[index] = CurValue;
    }

    ad_slide2[CSLIDERS].ob_state &= ~OS_SELECTED;
    ad_slide2[EXIT2].ob_state &= ~OS_SELECTED;

     adjcol();
     update_slid(ad_tree, base, slider, curscrn[index], 0, 1000, 1);
     evnt_button(1, 1, 0, &mk.x, &mk.y, &mk.buttons, &mk.kstate);
}



void Do_Up(OBJECT *tree, short base, short slider, short button, short index )
{
     curslid = slider;
     (*xcpb->Sl_arrow)(tree, base, slider, button, 1, 0,
               1000, &curscrn[index], VERTICAL, adjcol);
}



void Do_Down(OBJECT *tree, short base, short slider, short button, short index)
{
    curslid = slider;
    (*xcpb->Sl_arrow)(tree, base, slider, button, -1,
                  0, 1000, &curscrn[index], VERTICAL, adjcol);
}


void Do_Slider(OBJECT *tree, short base, short slider, short index, bool dclick )
{
    MRETS mk;
    short oldy;

    if (dclick)
        do_rgb( slider, base, index );
    else
    {
        (*xcpb->MFsave)(MFSAVE, &orig_mf);
        if (_AESversion >= 0x0320)
            graf_mouse(FLAT_HAND, 0L);

        if ((_AESversion >= 0x0330) && (numcol > LWHITE))
            XSelect(tree, slider);

        curslid = slider;

        graf_mkstate(&mk.x, &oldy, &mk.buttons, &mk.kstate);
        do
        {
            graf_mkstate(&mk.x, &mk.y, &mk.buttons, &mk.kstate);
        } while (mk.buttons && ( mk.y == oldy));

        if (mk.buttons && (mk.y != oldy)) {
            (*xcpb->Sl_dragy)(tree, base, slider, 0, 1000,
                              &curscrn[index],
                              (short (*)(void)) adjcol);
        }

        if ((_AESversion >= 0x0330) && (numcol > LWHITE))
            XDeselect(tree, slider );
        (*xcpb->MFsave)(MFRESTORE, &orig_mf);
     }
}



void Do_Base(OBJECT *tree, short base, short slider, short index)
{
    MRETS mk;
    short ox,oy;

    graf_mkstate(&mk.x, &mk.y, &mk.buttons, &mk.kstate);
    objc_offset(tree, slider, &ox, &oy);

    if (mk.y < oy)
        oy = VAL_PAGE;
    else
        oy = -VAL_PAGE;
    curslid = slider;

    (*xcpb->Sl_arrow)(tree, base, slider, -1, oy, 0,
                      1000, &curscrn[index], VERTICAL, adjcol);
}



void XDeselect(OBJECT *tree, short button)
{
    GRECT rect;

    tree[button].ob_state &= ~OS_SELECTED;
    rect = * (GRECT *) &tree[button].ob_x;

    objc_offset(tree, button, &rect.g_x, &rect.g_y);
    rect.g_x -= 2;
    rect.g_y -= 2;
    rect.g_w += 4;
    rect.g_h += 4;
    objc_draw(tree, button, MAX_DEPTH, rect.g_x, rect.g_y, rect.g_w, rect.g_h);
}


void XSelect(OBJECT *tree, short button)
{
   GRECT rect;

   tree[button].ob_state |= OS_SELECTED;

   rect = * (GRECT *) &tree[button].ob_x;
   objc_offset( tree, button, &rect.g_x, &rect.g_y );
   rect.g_x -= 2;
   rect.g_y -= 2;
   rect.g_w += 4;
   rect.g_h += 4;
   objc_draw(tree, button, MAX_DEPTH, rect.g_x, rect.g_y, rect.g_w, rect.g_h);
}



void MakeIndicator(OBJECT *tree, short obj)
{
    tree[obj].ob_flags |= OF_FL3DIND;
    tree[obj].ob_flags &= ~OF_FL3DACT;
}


void MakeActivator(OBJECT *tree, short obj)
{
    tree[obj].ob_flags |= OF_FL3DACT;
    tree[obj].ob_flags &= ~OF_FL3DIND;
}



void Do3D(void)
{
    short i;

    /*
     * reset border color to white
     */
    ad_tree[BASER].ob_spec.obspec.framecol = 0;
    ad_tree[BASER].ob_x += 1;
    ad_tree[BASER].ob_y += 1;
    ad_tree[BASER].ob_width -= 2;
    ad_tree[BASER].ob_height -= 2;

    MakeActivator(ad_tree, CUP);
    MakeActivator(ad_tree, CDOWN);
    MakeActivator(ad_tree, CBASE);
    MakeActivator(ad_tree, CSLIDE);

    MakeTed(ad_tree, CUP);
    MakeTed(ad_tree, CDOWN);
    MakeTed(ad_tree, CSLIDE);

    ad_tree[CUP].ob_x += 2;
    ad_tree[CUP].ob_y += 2;
    ad_tree[CUP].ob_width -= 4;
    ad_tree[CUP].ob_height -= 4;

    ad_tree[CDOWN].ob_x += 2;
    ad_tree[CDOWN].ob_y += 2;
    ad_tree[CDOWN].ob_width -= 4;
    ad_tree[CDOWN].ob_height -= 4;

    ad_tree[CSLIDE].ob_width -= 4;
    ad_tree[CSLIDE].ob_height -= 2;

    ad_tree[CBASE].ob_x += 2;
    ad_tree[CBASE].ob_width -= 4;
    ad_tree[CBASE].ob_y = ad_tree[CUP].ob_y + ad_tree[CUP].ob_height + 5;
    ad_tree[CBASE].ob_height = (ad_tree[CDOWN].ob_y - 5) - ad_tree[CBASE].ob_y;

    for (i = RUP; i <= BDOWN; i++)
    {
        if (ad_tree[i].ob_type == G_BOXTEXT)
            MakeTed(ad_tree, i);
        MakeActivator(ad_tree, i);

        if ((i != RSLIDE) && (i != GSLIDE) && (i != BSLIDE))
        {
            ad_tree[i].ob_x += 2;
            ad_tree[i].ob_y += 2;
            ad_tree[i].ob_height -= 4;
        }
        else
            ad_tree[i].ob_height = 16;
        ad_tree[i].ob_width -= 4;

        /* FIXME: this appears strange */
        if ((i == RBASE) && (i == GBASE) && (i == BBASE))
        {
            ad_tree[i].ob_y = ad_tree[RUP].ob_y + ad_tree[RUP].ob_height + 5;
            ad_tree[i].ob_height = (ad_tree[RDOWN].ob_y - 5) - ad_tree[i].ob_y;
        }
    }

    MakeActivator(ad_tree, CSAVE);
    MakeTed(ad_tree, CSAVE);

    MakeActivator(ad_tree, COK);
    MakeTed(ad_tree, COK);

    MakeActivator(ad_tree, CCANCEL);
    MakeTed(ad_tree, CCANCEL);

    MakeActivator(ad_tree, CRELOAD);
    MakeTed(ad_tree, CRELOAD);

    ad_tree[CRELOAD].ob_x -= 1;
    ad_tree[CRELOAD].ob_y -= 1;

    for (i = CSAVE; i <= CCANCEL; i++)
    {
        ad_tree[i].ob_x += 2;
        ad_tree[i].ob_y += 2;
        ad_tree[i].ob_width -= 4;
        ad_tree[i].ob_height -= 4;
    }
    ad_tree[CRELOAD].ob_x += 2;
    ad_tree[CRELOAD].ob_y += 2;
    ad_tree[CRELOAD].ob_width -= 4;
    ad_tree[CRELOAD].ob_height -= 3;
}


void MakeTed(OBJECT *tree, short obj)
{
    ((OBJC_COLORWORD *) &tree[obj].ob_spec.tedinfo->te_color)->fillc = G_LWHITE;
}
