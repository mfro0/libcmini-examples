/*
 * =====================================================================================
 *
 *       Filename:  document.c
 *
 *    Description:  the document module of the appskel framework
 *
 *        Version:  1.0
 *        Created:  24.08.2012 10:53:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Markus Fröschle (), mfro@mubf.de
 *   Organization:  
 *
 * =====================================================================================
 */

#include <gem.h>
#include <osbind.h>

#include <stdlib.h>
#include <assert.h>

#include "document.h"


//#define DEBUG
#ifdef DEBUG
/*
 * Since writing directly somewhere to the screen would distort GEM, the escape sequences in debug_printf()
 * position the cursor on line 30, column 0, clear to end of line and write the debug message.
 * Make sure you don't add a newline after the message or the screen will be clobbered.
 * This way we have at least one single line to display diagnostic output.
 */
#define debug_printf(format, arg...) do { printf("\033Y\36 \33lDEBUG (%s): " format, __FUNCTION__, ##arg); } while (0)
#else
#define debug_printf(format, arg...) do { ; } while (0)
#endif /* DEBUG */

/*
 *  init the document module
 */
int init_document()
{
	/* init document module */
	return TRUE;
}

/*
 *  cleanup the document module
 */
int exit_document()	/* module cleanup */
{
	/* cleanup documents module */
	return TRUE;
}

DOCUMENT *create_document()
{
	DOCUMENT *doc;

	doc = malloc(sizeof(DOCUMENT));
	assert(doc != NULL);
	
	return doc;
}

