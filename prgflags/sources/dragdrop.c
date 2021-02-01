#include <signal.h>
#include <mintbind.h>
#include <string.h>
#include <error.h>
#include <math.h>
#include "dragdrop.h"

//#define DEBUG
#ifdef DEBUG
//#include "natfeats.h"
#define dbg(format, arg...) do { printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

#define SIGPIPE         13
#define DD_NAMEMAX      128

static char pipename[24];
static void  *oldpipesig;

/*
 * Close a drag & drop operation. If handle is -1, don't do anything.
 */

void ddclose(short fd)
{
    if (fd >= 0)
    {
        Psignal(SIGPIPE, oldpipesig);
        Fclose(fd);
    }
}

/*
 * open a drag & drop pipe
 *
 * Input Parameters:
 * ddnam:       the pipe's name (from the last word of
 *              the AES message)
 * preferext:   a list of DD_NUMEXTS 4 byte extensions we understand
 *              these should be listed in order of preference
 *              if we like fewer than DD_NUMEXTS extensions, the
 *              list should be padded with 0s
 *
 * Output Parameters: none
 *
 * Returns:
 * A (positive) file handle for the drag & drop pipe, on success
 * -1 if the drag & drop is aborted
 * A negative error number if an error occurs while opening the
 * pipe.
 */

short ddopen(short ddnam, char *preferext)
{
    short fd;
    char outbuf[DD_EXTSIZE + 1];

    pipename[18] = (char) (ddnam & 0x00ff);
    pipename[17] = (char) ((ddnam & 0xff00) >> 8);

    fd = Fopen(pipename, 2);

    if (fd >= 0)
    {
        outbuf[0] = DD_OK;
        strncpy(outbuf+1, preferext, DD_EXTSIZE);

        oldpipesig = (void *) Psignal(SIGPIPE, SIG_IGN);

        if (Fwrite(fd, (long) DD_EXTSIZE + 1, outbuf) != DD_EXTSIZE + 1)
        {
            ddclose(fd);
            return -1;
        }
    }

    return fd;
}


/*
* ddrtry: get the next header from the drag & drop originator
*
* Input Parameters:
* fd:          the pipe handle returned from ddopen()
*
* Output Parameters:
* name:        a poshorter to the name of the drag & drop item
*              (note: this area must be at least DD_NAMEMAX bytes long)
* whichext:    a poshorter to the 4 byte extension
* size:        a poshorter to the size of the data
*
* Returns:
* 0 on success
* -1 if the originator aborts the transfer
*
* Note: it is the caller's responsibility to actually
* send the DD_OK byte to start the transfer, or to
* send a DD_NAK, DD_EXT, or DD_LEN reply with ddreply().
*/

short ddrtry(short fd, char *name, char *whichext, long *size)
{
    short
            hdrlen,
            i;

    char
            buf[80];

    if ((short)Fread(fd, 2L, &hdrlen) != 2 || hdrlen < 9)
        return -1;

    if ((short)Fread(fd, 4L, whichext) == 4)
    {
        whichext[4] = 0;

        if ((short)Fread(fd, 4L, size) == 4)
        {
            hdrlen -= 8;

            i = min(hdrlen, DD_NAMEMAX);

            if (Fread(fd, (long)i, name) == i)
            {
                hdrlen -= i;

                /* skip any extra header, no more than 80 bytes at a time */

                while (hdrlen > 0)
                {
                    Fread(fd, (long)min(80, hdrlen), buf);
                    hdrlen -= 80;
                }

                return 0;
            }
        }
    }

    return -1;
}


/*
* send a 1 byte reply to the drag & drop originator
*
* Input Parameters:
* fd:          file handle returned from ddopen()
* ack:         byte to send (e.g. DD_OK)
*
* Output Parameters:
* none
*
* Returns: 0 on success, -1 on failure
* in the latter case the file descriptor is closed
*/

short ddreply(short fd, short ack)
{
    char c = ack;

    if (Fwrite(fd, 1L, &c) != 1L)
        Fclose(fd);

    return 0;
}
