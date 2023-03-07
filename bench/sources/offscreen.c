#include "window.h"
#include "global.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "offscreen.h"
#include <gemx.h>
#include <mintbind.h>
#include <mint/errno.h>

//#define DEBUG
#ifdef DEBUG
#ifdef __mcoldfire__
#define dbg(format, arg...) do { fprintf(stderr, "DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { fprintf(format, ##arg); } while (0)
#else
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#endif /* __mcoldfire__ */
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */

struct offscreenwindow
{
    short bm_handle;           /* offscreen bitmap handle */
    MFDB bm_mfdb;
    short ellipse_color;
    short ellipse_pattern;
};

static void draw_sample(struct window *wi, short wx, short wy, short wh, short ww);
static void timer_offscreenwindow(struct window *wi);
static void delete_offscreenwindow(struct window *wi);
static void size_offscreenwindow(struct window *wi, short x, short y, short w, short h);

typedef struct
{
    long id;             /* Identifikations-Code */
    long value;          /* Wert des Cookies     */
} COOKJAR;


#define S_INQUIRE   ((short) 0xffff)
#define S_GETCOOKIE ((short) 0x0008)


static bool get_cookie(long cookie, void *value )
{
  static short use_ssystem = 0;
  COOKJAR *cookiejar;
  long val = -1L;
  short i = 0;

  if (use_ssystem < 0)
    use_ssystem = (Ssystem(S_INQUIRE, 0l, 0) == 0);

  if (use_ssystem)
  {
      if( Ssystem(S_GETCOOKIE, cookie, (long) &val) == 0)
      {
          if (value != NULL)
              *(long *) value = val;
          return true;
      }
  }
  else
  {
      /* Zeiger auf Cookie Jar holen */
      cookiejar = (COOKJAR *)(Setexc(0x05A0 / 4, (const void (*)(void)) -1));

      if (cookiejar)
      {
          for (i = 0; cookiejar[i].id ; i++)
          {
              dbg("cookiejar[%d] = %lx (\"%c%c%c%c\"\r\n", i, cookiejar[i].id,
                  cookiejar[i].id >> 24 & 0xff,
                  cookiejar[i].id >> 16 & 0xff,
                  cookiejar[i].id >> 8 & 0xff,
                  cookiejar[i].id >> 0 & 0xff);
              if (cookiejar[i].id == cookie)
              {
                  if (value)
                      *(long *) value = cookiejar[i].value;
                  return true;
              }
          }
      }
  }

  return false;
}

/*
 * must call EdDI functions with Pure-C calling conventions
 */
bool edi0(short (*func)(void), short parm)
{
    short out;
    
    __asm__ __volatile__("move.l %1,d0                  \n\t"
                         "move.l %2,a0                  \n\t" 
                         "jsr    (a0)                   \n\t"
                         "move.l d0,%0                  \n\t"
                         :"=g"(out)
                         :"g"(parm), "m"(func): "memory");
    return out;
}

static bool is_EdDI(void)
{
    short (*func)(void);
    
    if (get_cookie('EdDI', (long *) &func))
    {
        return edi0(func, 0);
    }
    else
    {
        return false;
    }
}


/*
 * create a new window and add it to the window list.
 */
struct window *create_offscreenwindow(short wi_kind, char *title)
{
    struct window *wi = NULL;
    struct offscreenwindow *ow;
    short work_in[20] = { 0 };
    short work_out[57];

    if (is_EdDI())
        wi = create_window(wi_kind, title);
    else
    {
        form_alert(0, "[0][No EdDI cookie found.|Offscreen bitmaps are unavailable][OK]");
        return NULL;
    }
    
    if (wi != NULL)
    {
        wi->wclass = OFFWINDOW_CLASS;
        wi->draw = draw_sample;
        wi->del = delete_offscreenwindow;
        wi->timer = timer_offscreenwindow;
        wi->size = size_offscreenwindow;

        ow = malloc(sizeof(struct offscreenwindow));
        if (ow != NULL)
        {
            wi->priv = ow;
            ow->ellipse_color = 0;
            ow->ellipse_pattern = 0;
        }

        ow->bm_handle = wi->vdi_handle;
        memset(&ow->bm_mfdb, 0, sizeof(MFDB));

        work_in[10] = 2;

        work_in[11] = 1279;
        work_in[12] = 959;
        work_in[13] = 1000;
        work_in[14] = 1000;

        dbg("VDI handle: %d\n", vdi_handle);
        v_opnbm(work_in, &ow->bm_mfdb, &ow->bm_handle, work_out);

        dbg("bitmap handle: %d\n", ow->bm_handle);
        dbg("MFDB.fd_addr=%p\n", ow->bm_mfdb.fd_addr);
        dbg("MFDB.fd_w=%d\n", ow->bm_mfdb.fd_w);
        dbg("MFDB.fd_h=%d\n", ow->bm_mfdb.fd_h);
        dbg("MFDB.fd_wdwidth=%d\n", ow->bm_mfdb.fd_wdwidth);
        dbg("MFDB.fd_stand=%d\n", ow->bm_mfdb.fd_stand);
        dbg("MFDB.fd_nplanes=%d\n", ow->bm_mfdb.fd_nplanes);

        if (! ow->bm_handle)
        {
            form_alert(1, "[Could not get offscreen | bitmap handle (v_opnbm)][CANCEL]");

            wi->del(wi);
            return NULL;
        }

        wi->top = 0;
        wi->left = 0;
        wi->doc_width = ow->bm_mfdb.fd_w;
        wi->doc_height = ow->bm_mfdb.fd_h;
        wi->x_fac = gl_wchar;	/* width of one character */
        wi->y_fac = gl_hchar;	/* height of one character */
    }
    return wi;
}

static void delete_offscreenwindow(struct window *wi)
{
    if (wi && wi->priv)
    {
        struct offscreenwindow *ow = (struct offscreenwindow *) wi->priv;

        if (ow->bm_handle)
        {
            v_clsbm(ow->bm_handle);
        }
        free(wi->priv);
    }
    /* let the generic window code do the rest */
    delete_window(wi);
}

static void size_offscreenwindow(struct window *wi, short x, short y, short w, short h)
{
    struct offscreenwindow *ow = wi->priv;

    wi->top = 0;
    wi->left = 0;

    size_window(wi, x, y, w, h);        /* call super "class"' size method */

}
/*
 * Draw Filled Ellipse
 */
static void draw_sample(struct window *wi, short wx, short wy, short ww, short wh)
{
    struct offscreenwindow *ow = wi->priv;
    short vh = wi->vdi_handle;

    MFDB screen = { 0 };
    short w_x = wx - wi->work.g_x;
    short w_y = wy - wi->work.g_y;
    short pxy[8] = { wi->left + w_x, wi->top + w_y, wi->left + w_x + ww - 1, wi->top + w_y + wh - 1,
                     wx, wy, wx + ww - 1, wy + wh - 1};

    
    vro_cpyfm(vh, S_ONLY, pxy, &ow->bm_mfdb, &screen);
}


/*
 * react on timer events
 */
static void timer_offscreenwindow(struct window *wi)
{
    struct offscreenwindow *ow = (struct offscreenwindow *) wi->priv;
    short bh = ow->bm_handle;

    short pxy[8] = { 
        0, 0, ow->bm_mfdb.fd_w - 1, ow->bm_mfdb.fd_h - 1, 
        0, 0, ow->bm_mfdb.fd_w - 1, ow->bm_mfdb.fd_h - 1 
    };

    vr_recfl(ow->bm_handle, pxy);
    
    vro_cpyfm(ow->bm_handle, ALL_WHITE, pxy, &ow->bm_mfdb, &ow->bm_mfdb);

    vsf_style(bh, FIS_PATTERN);
    vsf_interior(bh, ow->ellipse_pattern);
    vsf_interior(bh, 1);
    vsf_color(bh, ow->ellipse_color);
    v_ellipse(bh, wi->work.g_w / 2, wi->work.g_h / 2,
                  wi->work.g_w / 2, wi->work.g_h / 2);


    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);

    ow->ellipse_color++;
    ow->ellipse_color &= 15;

    if (ow->ellipse_color == 15)
    {
        ow->ellipse_pattern++;
        ow->ellipse_pattern &= 15;
    }

}

