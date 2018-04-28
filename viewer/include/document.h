#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_ 

#include "portab.h"

typedef struct _document {
	long width;			/* canvas width and height in whatever units the user might find useful */
	long height;

	void *contents;
} DOCUMENT;

extern int init_document();	/* module init */
extern int exit_document();	/* module cleanup */

#endif /* _DOCUMENT_H_ */
