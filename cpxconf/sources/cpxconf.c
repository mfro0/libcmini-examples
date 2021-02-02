/* */

#define DEBUG
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

static struct           /* program header */
{
    short magic;
    long dummy[5];
    unsigned long flags;
} header;

#define HEADER_MAGIC 0x601a
#define FASTLOAD_FLAG 0x00000001UL
#define PROGLOAD_FLAG 0x00000002UL
#define MEMALLOC_FLAG 0x00000004UL
#define FLAG_MASK  (FASTLOAD_FLAG|PROGLOAD_FLAG|MEMALLOC_FLAG)

static char pathname[140];   /* for fsel_exinput() */

/*
* function prototypes
*/
static short cpx_call(GRECT *rect);
CPXINFO *cpx_init(XCPB * Xcpb);
static void disable_flag_buttons(void);
static void enable_flag_buttons(short flags);
static short get_flag_buttons(void);
static short read_program_header(char *filename);
static void write_program_header(char *filename);


/*
* cpx_call - the main CPX driver entry point. We spend most of our time
* in here dispatching the events from Xform_do which it doesn't want to
* handle itself.
*/
static short cpx_call(GRECT *rect)
{
    short msg[8];
    short button, rc;
    char *p;
    short flags, drive, quit = 0;

    TEDINFO *ted;

    dbg("rect = %d, %d, %d, %d\r\n", rect->g_x, rect->g_y, rect->g_w, rect->g_h);

    /*
    * Initialise location of form within CPX window
    */
    rs_object[CPXCONF].ob_x = rect->g_x;
    rs_object[CPXCONF].ob_y = rect->g_y;

    disable_flag_buttons();


    do
    {
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

        switch(button)
        {
            case BSAVE:
            case BOK:
                flags = get_flag_buttons();
                if (flags >= 0)
                {
                    header.flags &= ~FLAG_MASK;
                    header.flags |= flags;
                    write_program_header(pathname);
                }
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

/*
* disable & deselect all the buttons
*/
void disable_flag_buttons(void)
{

}

/*
* enable all the buttons & select those corresponding to 'flags'
*/
void enable_flag_buttons(short flags)
{

}

short get_flag_buttons(void)
{

}

/*
* read the header of the program
* returns the flags, or -1 if an error
*/
short read_program_header(char *filename)
{

}

/*
* write the header of the program
*/
void write_program_header(char *filename)
{

}
