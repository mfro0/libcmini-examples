#include "rsrc.h"
#include "global.h"
#include "util.h"

#include <stdlib.h>
#include <osbind.h>

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

short init_resource()
{
    if (! rsrc_load(RESOURCENAME))
    {
        form_alert(1, "[3][RSC file not found!][Exit]");
        exit(-1);
    }
    return 0;
}

void free_resource()
{
    rsrc_free();
}
