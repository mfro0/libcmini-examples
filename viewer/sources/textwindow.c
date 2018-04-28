#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ext.h>

#include "window.h"
#include "textwindow.h"
#include "util.h"
#include "global.h"
#include "document.h"
#include "viewer.h"

#ifdef MIN_WIDTH
#undef MIN_WIDTH
#endif /* MIN_WIDTH */
#ifdef MIN_HEIGHT
#undef MIN_HEIGHT
#endif /* MIN_HEIGHT */
#define MIN_WIDTH  (6 * gl_wbox)
#define MIN_HEIGHT (3 * gl_hbox)

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

struct textwindow
{
    long buffer_size;       /* size of window's text buffer */
	long buffer_end;		/* what's used so far of it */
	char *buffer;			/* the buffer itself */
	long num_lines;
	char **index;			/* pointer array to individual lines in buffer */

	/* parts of struct that should probably go to the doc member later */
	bool dosmode;
	bool expand_tabs;
	int tabsize;
	DOCUMENT *doc;
};

/*
 * create a new window and add it to the window list.
 */
struct window *create_textwindow(short wi_kind, char *title)
{
	struct window *wi;
	struct textwindow *tw;
  
	wi = create_window(wi_kind, title);

	wi->class = TEXTWINDOW_CLASS;
	wi->draw = draw_textwindow;
    wi->del = delete_textwindow;
	tw = malloc(sizeof(struct textwindow));
    wi->priv = tw;
	tw->buffer_end = 0;
	tw->buffer_size = 2048;
	tw->buffer = malloc(tw->buffer_size);
	tw->buffer[0] = '\0';
	tw->index = NULL;
	tw->num_lines = 0;

	wi->top = 0;
	wi->left = 0;
	wi->doc_width = 0;
	wi->doc_height = 0;
	wi->x_fac = gl_wchar;	/* width of one character */
	wi->y_fac = gl_hchar;	/* height of one character */

	tw->doc = malloc(sizeof(DOCUMENT));

	return wi;
}

/*
 * delete window from window list, free memory and delete in AES
 */
void delete_textwindow(struct window *wi)
{
    struct textwindow *tw = wi->priv;

	if (tw) {
		/* delete the textwindow-specific parts of the struct */
		if (tw->buffer) {
			free(tw->buffer);
		}
		if (tw->doc) {
			free(tw->doc);
		}
		if (tw->index) {
			free(tw->index);
		}
		free(tw);
	}
    wi->priv = NULL;

	/* let the generic window code do the rest */
	delete_window(wi);
}

/*
 * count number of lines in buffer by counting \n-Characters
 */
static int num_lines(struct textwindow *tw)
{
	char *str = tw->buffer;
	int nlines = 0;

	do {
		if (*str == '\n')
		{
			nlines++;
		}
	} while (*str++);

	return nlines;
}

/*
 * create a lines index for the text so that individual text buffer
 * lines can be accessed directly by tw->index[line-no]
 */
static char **reindex_lines(struct textwindow *tw)
{
	char *lp = tw->buffer;
	int n_lines = num_lines(tw);
	char **ip;

	if (tw->index != NULL)	/* index is already there, rebuild it */
	{
		free(tw->index);
	}
	tw->index = calloc(n_lines, sizeof(char *));

	ip = tw->index;
	*ip++ = lp;

	do {
		switch (*lp)
		{
		case '\n':
			*ip++ = lp + 1;
			break;

		default:
			;
			/* no break */
		}
	} while (*lp++);
	tw->num_lines = n_lines;
	return tw->index;
}

/*
 * update TEXTWINDOW struct members that depend on it from buffer contents
 */
static void update_document(struct window *wi)
{
    struct textwindow *tw = wi->priv;
	char *cp = tw->buffer;
	long lineno = 0;
	long maxwidth = 0, width = 0;
	char *left = cp;

	while ((cp - tw->buffer) < tw->buffer_size)
	{
		if (*cp++ == '\n')	
		{
			width = cp - left;
			if (width > maxwidth)
			{
				maxwidth = width;
			}
			left = cp;
			lineno++;
			if (*(cp - 2) == '\r')
			{
                tw->dosmode = true;
			}
		}
	}
	if (*(cp - 1) != '\n')
	{
		lineno++;
		strcat(tw->buffer, "\n");
	}
	wi->doc_height = lineno;
	wi->doc_width = maxwidth;
	sprintf(wi->info, "Lines: %ld,  Columns: %ld",  lineno,  maxwidth);
	wind_set_str(wi->handle,  WF_INFO,  wi->info);

	tw->index = reindex_lines(tw);
}

/*
 * expand tabs in line. Line must have enough space for that or we'll experience crashes
 */
static char *expand_tabs(char *line, int tabsize)
{
	char tmp[1024];
	char *src;
	char *tgt;

	src = line;
	tgt = tmp;

	do
	{
		switch (*src)
		{
		case '\t':
			do
			{
				*tgt++ = ' ';
			} while ((tgt - tmp) % tabsize);
			break;
		default:
			*tgt++ = *src;
			break;
		}
	} while (*src++ != '\0');
	*tgt = '\0';
	strcpy(line, tmp);

	return line;
}

/*
 * Draw window contents
 */
void draw_textwindow(struct window *wi, short x, short y, short w, short h)
{
    struct textwindow *tw = wi->priv;
	long start;
	long end;
	long curr;
	const int tabsize = 4;

	wi->clear(wi, x, y, w, h);

	vst_color(vdi_handle, 1);		/* black */

	if (tw->index != NULL)	/* only draw text if we have some */
	{
		int dx = wi->work.g_x;
		int dy = wi->work.g_y;

		/* start of area to display (wi->top lines from beginning) */
		start = wi->top;

		/* find end of area to display (wi->top + wi->work.g_h / y_fac lines from beginning) */
		end = MIN(wi->top + wi->work.g_h / wi->y_fac, tw->num_lines - 1);
	
		curr = start;

		while (curr < end)
		{
			char line[1024];		/* temporary line buffer */
			int length;

			length = tw->index[curr + 1] - tw->index[curr];

			strncpy(line, tw->index[curr], length);
			line[length - 1] = '\0';	/* replace '\n' with '\0' in copy */
			expand_tabs(line, tabsize);

			v_gtext(vdi_handle, dx, dy + gl_hchar, line + wi->left);	/* print it */
	
			dy += gl_hchar;
			curr++;
		}
	}
}

/*
 * Print text into window
 */
int printf_textwindow(struct window *wi, const char* format, ...)
{
	va_list list;
	int num_printed = 0;
	struct textwindow *tw;
	char c;
	const char *fp = format;

	va_start(list, format);

    tw = wi->priv;

	while ((c = *fp++) && wi)
	{
		if (c != '%')
		{
			tw->buffer[tw->buffer_end++] = c;	/* copy character by character to buffer */

			if (tw->buffer_end > tw->buffer_size - 1) 	/* we're running out of space */
			{
				long new_size;
				char *new_buffer;

				new_size = tw->buffer_size * 2;
				new_buffer = realloc(tw->buffer, new_size);	/* allocate more space */
				if (new_buffer)
				{
					tw->buffer = new_buffer;
					tw->buffer_size = new_size;
				}
				else
				{
					/* out of memory */
				}
			}
			else
			{
				/* found start of format string */

				/* TODO: ignored for now */
			}
		}
	}
	va_end(list);
	update_document(wi);
	return num_printed;
}

char *textwindow_load(struct window *wi, char *filename)
{
	struct stat st;
	long fhandle;
	char *buffer;
	long err;
	long size;
	
    struct textwindow *tw = wi->priv;

	/* look for file's size first */
	if ((err = stat(filename, &st)) >= 0)
	{
		size = st.st_size;
	}
	else
	{
		return NULL;
	}

	/* malloc a buffer suitable for the file's size */
	buffer = malloc(size + 1);
	if (buffer != NULL)
	{ 
		if ((fhandle = Fopen(filename, 0)) > 0)
		{
			long res;

			res = Fread(fhandle, size, buffer);

			if (res != size)
			{
				free(buffer);
				return NULL;
			}
		}
		Fclose(fhandle);
	}
	else
	{
		return NULL;
	}

	if (tw->buffer)
	{
		free(tw->buffer);
	}
	tw->buffer = buffer;
	tw->buffer_end = tw->buffer_size = size;

	tw->buffer[size] = '\0';

	update_document(wi);

	wi->scroll(wi);

	return buffer;
}

void create_textwindow_with_file(char *filename)
{
	char *res;
	struct window *wi;

	wi = create_textwindow(TEXTWINDOW_WINELEMENTS, "Text Window");
	if (wi != NULL)
	{
		res = textwindow_load(wi, filename);
		if (res != NULL)
		{
			wind_set_str(wi->handle, WF_NAME, wi->name);
			open_window(wi, window_open_pos_x, window_open_pos_y, MIN_WIDTH, MIN_HEIGHT);
			window_open_pos_x += 10;
			window_open_pos_y += 10;
			do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
		}
		else
		{
			char errstr[512];
			char *fileerr;

			rsrc_gaddr(R_STRING, FILEERR, &fileerr);
			sprintf(errstr, fileerr, filename);
			form_alert(1, errstr);
		}
	}
}
