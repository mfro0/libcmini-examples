#include <stdint.h>
#include "global.h"
#include "window.h"

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

short vdi_handle;    		/* virtual workstation handle */

short work_in[11];   		/* Input to GSX parameter array */
short work_out[57];  		/* Output from GSX parameter array */
short ext_out[57];
short ap_id;

short window_open_pos_x;
short window_open_pos_y;

short phys_handle;
short gl_hbox;
short gl_wbox;
short gl_hchar;
short gl_wchar;
short gl_nplanes;
short gl_nsysfonts;

OBJECT *gl_menu;

GRECT gl_desk;

void init_global(void)
{
    window_open_pos_x = 20;
    window_open_pos_y = 20;
}

void free_global(void)
{
}


