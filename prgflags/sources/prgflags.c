/* */

//#define DEBUG
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

#include "portab.h"
#include <gem.h>
#include "prgflags.h"
#include "prgflags.rsh"

#include <stdbool.h>
#include <string.h>
#include <osbind.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpxdata.h"
#include "dragdrop.h"

/*
 * prgflags.c
 *
 * CPX to set a file's program flags
 *
 *
 * IMPORTANT: BUILDING THE CPX
 *  You *must* update the CPX version number in the makefile!
 *  Note that we use a makefile rather than a project file, since
 *  we need to decompile the resource and build the CPX header.
 *  However, the project file still exists so that we can verify
 *  that the code compiles cleanly before running the make.
 *
 *
 * version 1.00 January/2020  Roger Burrows
 *
 * adapted to gcc calling conventions and ORCS as resource editor
 * (and adopted) as libcmini example - thank you!
 *
 * version 1.10 January/2021 Markus Fröschle
 *
 *
 *  this code borrows heavily from the Lattice sample ... thanks, guys!
 *
 */


/*
*  variables
*/
static XCPB *xcpb;      /* XControl Parameter Block */

static struct       /* program header */
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
static char filename[14];

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
    char exts[DD_EXTSIZE] = { 0 };
    short ddpipe;

    TEDINFO *ted;

    dbg("rect = %d, %d, %d, %d\r\n", rect->g_x, rect->g_y, rect->g_w, rect->g_h);

    /*
    * Initialise location of form within CPX window
    */
    rs_object[PRGFLAGS].ob_x = rect->g_x;
    rs_object[PRGFLAGS].ob_y = rect->g_y;

    disable_flag_buttons();


    do
    {
        /*
        * Sit around waiting for a message
        */
        button = xcpb->Xform_do(&rs_object[PRGFLAGS], ROOT, msg);

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
                    button = B_CANCEL;
                    break;

                case WM_REDRAW:
                {
                    GRECT *ret;

                    ret = (*xcpb->GetFirstRect)(rect);
                    while (ret)
                    {
                        objc_draw(&rs_object[PRGFLAGS], ROOT, MAX_DEPTH,
                                  ret->g_x, ret->g_y, ret->g_w, ret->g_h);
                        ret = (*xcpb->GetNextRect)();
                    }
                    continue;
                }
                break;

                case WM_CLOSED:    // wm_close means ok
                    button = B_OK;
                    break;

                case AP_DRAGDROP:
                    strcpy(exts, "ARGS");
                    ddpipe = ddopen(msg[7], exts);
                    dbg("pipe handle from drag & drop message = %d\r\n", ddpipe);
                    continue;
                    break;

                default:
                    dbg("unknown message type %d (0x%x)\r\n", msg[0], msg[0]);
            }
        }

        dbg("button=%d\r\n", button);

        switch(button)
        {
            case FILENAME:
                /*
                 * pop up the file selector
                 */
                drive = Dgetdrv();
                pathname[0] = drive + 'A';
                pathname[1] = ':';
                Dgetpath(pathname+2,drive+1); /* current path on current drive */
                strcat(pathname,"\\*.*");
                filename[0] = '\0';
                rc = fsel_exinput(pathname,filename,&button,"Select program");
                flags = -1;
                if (rc && button)
                {
                    ted = rs_object[FILENAME].ob_spec.tedinfo;
                    strcpy(ted->te_ptext + 6, filename);
                    p = pathname + strlen(pathname);
                    while (*p != '\\')
                        p--;
                    strcpy(p + 1, filename);
                    flags = read_program_header(pathname);
                }
                if (flags < 0)
                    disable_flag_buttons();
                else
                    enable_flag_buttons(flags);
                objc_draw(&rs_object[PRGFLAGS], ROOT, MAX_DEPTH, rect->g_x, rect->g_y, rect->g_w, rect->g_h);
                break;

            case B_SAVE:
            case B_OK:
                flags = get_flag_buttons();
                if (flags >= 0)
                {
                    header.flags &= ~FLAG_MASK;
                    header.flags |= flags;
                    write_program_header(pathname);
                }
                /* fall through */
            case B_CANCEL:
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

    (*xcpb->rsh_fix)(NUM_OBS, NUM_FRSTR, NUM_FRIMG, NUM_TREE,
                   rs_object, rs_tedinfo, rs_strings, rs_iconblk,
                   rs_bitblk, rs_frstr, rs_frimg, rs_trindex, rs_imdope);


    return &cpxinfo;
}

/*
* disable & deselect all the buttons
*/
void disable_flag_buttons(void)
{
    rs_object[FASTLOAD].ob_state |= OS_DISABLED;
    rs_object[FASTLOAD].ob_state &= ~OS_SELECTED;

    rs_object[PROGLOAD].ob_state |= OS_DISABLED;
    rs_object[PROGLOAD].ob_state &= ~OS_SELECTED;

    rs_object[MEMALLOC].ob_state |= OS_DISABLED;
    rs_object[MEMALLOC].ob_state &= ~OS_SELECTED;
}

/*
* enable all the buttons & select those corresponding to 'flags'
*/
void enable_flag_buttons(short flags)
{
    rs_object[FASTLOAD].ob_state &= ~OS_DISABLED;
    if (flags & FASTLOAD_FLAG)
        rs_object[FASTLOAD].ob_state |= OS_SELECTED;
    else
        rs_object[FASTLOAD].ob_state &= ~OS_SELECTED;

    rs_object[PROGLOAD].ob_state &= ~OS_DISABLED;
    if (flags & PROGLOAD_FLAG)
        rs_object[PROGLOAD].ob_state |= OS_SELECTED;
    else
        rs_object[PROGLOAD].ob_state &= ~OS_SELECTED;

    rs_object[MEMALLOC].ob_state &= ~OS_DISABLED;
    if (flags & MEMALLOC_FLAG)
        rs_object[MEMALLOC].ob_state |= OS_SELECTED;
    else
        rs_object[MEMALLOC].ob_state &= ~OS_SELECTED;
}

short get_flag_buttons(void)
{
    long flags = 0;

    if (rs_object[FASTLOAD].ob_state & OS_DISABLED)
        return -1;

    if (rs_object[FASTLOAD].ob_state & OS_SELECTED)
        flags |= FASTLOAD_FLAG;
    if (rs_object[PROGLOAD].ob_state & OS_SELECTED)
        flags |= PROGLOAD_FLAG;
    if (rs_object[MEMALLOC].ob_state & OS_SELECTED)
        flags |= MEMALLOC_FLAG;

    return (short)flags;
}

/*
* read the header of the program
* returns the flags, or -1 if an error
*/
short read_program_header(char *filename)
{
    long rc;
    short fh, flags = -1;

    rc = Fopen(filename,0);
    if (rc < 0)
        return -1;
    fh = (short)rc;

    if (Fread(fh, sizeof(header), &header) == sizeof(header))
        if (header.magic == HEADER_MAGIC)
            flags = (short)(header.flags & FLAG_MASK);

    Fclose(fh);

    return flags;
}

/*
* write the header of the program
*/
void write_program_header(char *filename)
{
    long rc;
    short fh;

    rc = Fopen(filename,1);
    if (rc < 0)
        return;
    fh = (short)rc;

    if (Fwrite(fh, sizeof(header), &header) != sizeof(header))
        xcpb->XGen_Alert(2); /* oops */

    Fclose(fh);
}
