/* */

// #define DEBUG
#ifdef DEBUG
//#include "natfeats.h"
#define dbg(format, arg...) do { printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

/* INCLUDE FILES
 * ======================================================================
 */

#define __GEMLIB_OLDNAMES
#include "portab.h"
#include <gem.h>
#include "cpxconf.h"
#include "cpxconf.rh"
#include "cpxconf.rsh"
#undef __GEMLIB_OLDNAMES

#include <gem.h>
#include <stdbool.h>
#include <string.h>
#include <osbind.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpxdata.h"


/*
 * cpxconf.c
 *
 * CPX to set CPXHEADER information
 */


/*
 *  variables
 */
static XCPB *xcpb;      /* XControl Parameter Block */

/*
 * cpx_call - the main CPX driver entry point. We spend most of our time
 * in here dispatching the events from Xform_do which it doesn't want to
 * handle itself.
 */
static short cpx_call(GRECT *rect)
{
    short msg[8];
    short button;
    short quit = 0;

    CPXNODE *cpx, *ccpx;

    dbg("rect = %d, %d, %d, %d\r\n", rect->g_x, rect->g_y, rect->g_w, rect->g_h);

    /*
    * Initialise location of form within CPX window
    */
    rs_object[CPXCONF].ob_x = rect->g_x;
    rs_object[CPXCONF].ob_y = rect->g_y;

    cpx = (*xcpb->Get_Head_Node)();

    short color;

    do
    {
        dbg("t_color=%x, i_color=%x\r\n", cpx->cpxhead.t_color, cpx->cpxhead.i_color);

        /*
         * set dialog object values from current CPX header
         */
        strncpy(rs_object[CPXNAME].ob_spec.tedinfo->te_ptext, cpx->cpxhead.title_text, 18);
        rs_object[CPXNAME].ob_spec.tedinfo->te_color = cpx->cpxhead.t_color;

        strncpy(rs_object[CPXITEXT].ob_spec.tedinfo->te_ptext, cpx->cpxhead.i_text, 14);
        rs_object[CPXITEXT].ob_spec.tedinfo->te_txtlen = strlen(cpx->cpxhead.i_text);

        rs_object[CPXICON].ob_spec.bitblk->bi_color = cpx->cpxhead.i_color;
        rs_object[CPXITEXT].ob_spec.tedinfo->te_color = cpx->cpxhead.t_color;

        rs_object[CPXITEXT].ob_spec.tedinfo->te_color = cpx->cpxhead.t_color;
        memcpy(rs_object[CPXICON].ob_spec.bitblk->bi_pdata, cpx->cpxhead.sm_icon,
              48 * sizeof(short));

        sprintf(rs_object[TXTCOL].ob_spec.tedinfo->te_ptext, "%2d", (cpx->cpxhead.t_color >> 8) & 0xf);
        sprintf(rs_object[ICNCOL].ob_spec.tedinfo->te_ptext, "%2d", cpx->cpxhead.i_color >> 12);

        objc_draw(rs_object, ROOT, MAX_DEPTH, rect->g_x, rect->g_y, rect->g_w, rect->g_h);

        /*
         * Sit around waiting for a message
         */
        button = xcpb->Xform_do(&rs_object[CPXCONF], ROOT, msg);

        /* Check if we have a double click item */
        if ((button != -1) && (button & 0x8000))
        {
            button &= 0x7fff;
        }

        /*
        * If it wasn't a button then try to turn it into one.
        */
        if (button == -1)
        {
            switch(msg[0])
            {
                case AC_CLOSE:    // ac_close means cancel
                    button = BCANCEL;
                    break;

                case WM_REDRAW:
                {
                    GRECT *ret;

                    ret = (*xcpb->GetFirstRect)(rect);
                    while (ret)
                    {
                        objc_draw(&rs_object[CPXCONF], ROOT, MAX_DEPTH,
                                  ret->g_x, ret->g_y, ret->g_w, ret->g_h);
                        ret = (*xcpb->GetNextRect)();
                    }
                    continue;
                }
                break;

                case WM_CLOSED:    // wm_close means ok
                    button = BOK;
                    break;

                default:
                    dbg("unknown message type %d (0x%x)\r\n", msg[0], msg[0]);
            }
        }

        dbg("button=%d\r\n", button);

        GRECT butrect;
        short sel;

        switch(button)
        {
            case NCPX:
                /* find and display next CPX's attributes */
                cpx = cpx->next;
                if (cpx == NULL)        /* wrap around */
                {
                    cpx = (*xcpb->Get_Head_Node)();
                }
                rs_object[NCPX].ob_state &= ~OS_SELECTED;
                break;

            case PCPX:
                /* find and display previous CPX's attributes */
                ccpx = cpx;

                /* if we are at the beginning, the previous one is the last one */
                if (cpx == (*xcpb->Get_Head_Node)())
                {
                    while (cpx->next != NULL)
                        cpx = cpx->next;
                }
                else    /* else it's the one that's next pointer points to us */
                {
                    cpx = (*xcpb->Get_Head_Node)();
                    while (cpx->next != ccpx)
                    {
                        cpx = cpx->next;
                    }
                }
                rs_object[PCPX].ob_state &= ~OS_SELECTED;
                break;

            case NICNCOL:
                color = cpx->cpxhead.i_color >> 12;
                color += 1;
                color &= 15;
                cpx->cpxhead.i_color = (cpx->cpxhead.i_color & ~ (0xf << 12)) | (color << 12);
                rs_object[NICNCOL].ob_state &= ~OS_SELECTED;
                break;

            case PICNCOL:
                color = cpx->cpxhead.i_color >> 12;
                color -= 1;
                color &= 15;
                cpx->cpxhead.i_color = (cpx->cpxhead.i_color & ~ (0xf << 12)) | (color << 12);
                rs_object[PICNCOL].ob_state &= ~OS_SELECTED;
                break;

            case NTXTCOL:
                color = (cpx->cpxhead.t_color >> 8) & 0xf;
                color += 1;
                color &= 15;
                cpx->cpxhead.t_color = (cpx->cpxhead.t_color & ~ (0xf << 8)) | (color << 8);
                rs_object[NTXTCOL].ob_state &= ~OS_SELECTED;
                break;

            case PTXTCOL:
                color = (cpx->cpxhead.t_color >> 8) & 0xf;
                color -= 1;
                color &= 15;
                cpx->cpxhead.t_color = (cpx->cpxhead.t_color & ~ (0xf << 8)) | (color << 8);
                rs_object[PTXTCOL].ob_state &= ~OS_SELECTED;
                break;

            case RESPUP:
                objc_offset(rs_object, RESPUP, &butrect.g_x, &butrect.g_y);
                butrect.g_w = rs_object[RESPUP].ob_width;
                butrect.g_h = rs_object[RESPUP].ob_height;

                sel = (*xcpb->Popup)(&rs_frstr[PUPSY], 2, -1, AES_LARGEFONT, &butrect, rect);
                dbg("sel=%d\r\n", sel);
                rs_object[RESPUP].ob_spec.free_string = rs_frstr[sel];
                rs_object[RESPUP].ob_state &= ~OS_SELECTED;
                break;

            case BSAVE:
            case BOK:

                /* fall through */
            case BCANCEL:
                quit = 1;
                break;
        }
    } while (!quit);

    return 0;
}

static CPXINFO cpxinfo = { &cpx_call, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*
 * cpx_init - main entry to the CPX, we arrive here when called either
 * during boot up or at the users request.
 */
CPXINFO *cpx_init(XCPB * Xcpb)
{
    xcpb = Xcpb;

    dbg("xcpb = %p\r\n", (void *) xcpb);

    appl_init();      // initialise private tables

    /*
     * Just in case someone turns the boot flag on in the CPX ...
     */
    if (xcpb->booting)
    {
        /*
         * Nothing special to do at boot time ...
         */
        return (CPXINFO *) 1;    // indicate we want to keep going
    }

    dbg("fix resource\r\n");

    if (!xcpb->SkipRshFix)
    {
        OBJECT *tree = rs_object;
        int obj;

        dbg("rsh_obfix\r\n");

        for (obj = 0; obj < NUM_OBS; obj++)
        {
            (*xcpb->rsh_obfix)(tree, obj);
        }
        xcpb->SkipRshFix = true;
    }
    return &cpxinfo;
}
