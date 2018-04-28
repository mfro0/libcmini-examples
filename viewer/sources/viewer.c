#include <gem.h>
#include "portab.h"
#include <stdlib.h>
#include <osbind.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#include "resource.h"
#include "window.h"
#include "viewer.h"
#include "menu.h"
#include "util.h"
#include "objutil.h"
#include "global.h"
#include "dialog.h"
#include "textwindow.h"

#define DEBUG
#ifdef DEBUG
/*
 * Since writing directly somewhere to the screen would distort GEM, the escape sequences in debug_printf()
 * position the cursor on line 30, column 0, clear to end of line and write the debug message.
 * Make sure you don't add a newline after the message or the screen will be clobbered.
 * This way we have at least one single line to display diagnostic output.
 */
#define debug_printf(format, arg...) do { printf("\033Y\036 \033lDEBUG (%s): " format, __FUNCTION__, ##arg); } while (0)
#else
#define debug_printf(format, arg...) do { ; } while (0)
#endif /* DEBUG */

static short msgbuff[8];    /* event message buffer */
static short mx;
static short my;            /* mouse x and y pos. */
static short butdown;       /* button state tested for, UP/DOWN */
static short ret;           /* dummy return variable */

bool quit;

struct dialog_handler *about_dialog;
struct dialog_handler *prefs_dialog;

short about_exit_handler(struct dialog_handler *dial, short exit_obj);
short prefs_exit_handler(struct dialog_handler *dial, short exit_obj);
bool prefs_touchexit_callback(struct dialog_handler *, short exit_obj);

struct prefs_dialog_private
{
    int num_fonts;
    int first_line_displayed;
};


void set_slider_size(OBJECT *tree, short slidbar, short slider, short permill, bool horiz)
{
    if (horiz)
    {
        tree[slider].ob_width = tree[slidbar].ob_width * permill / 1000L - 2;
    }
    else
    {
        tree[slider].ob_height = tree[slidbar].ob_height * permill / 1000L - 2;
    }
}

void set_slider_pos(OBJECT *tree, short slidbar, short slider, short permill, bool horiz)
{
    if (horiz)
    {
        tree[slider].ob_x = tree[slidbar].ob_width * permill / 1000L + 1;
    }
    else
    {
        tree[slider].ob_y = tree[slidbar].ob_height * permill / 1000L + 1;
    }
}

void prefs_dialog_set_slider(struct dialog_handler *dial)
{
    struct prefs_dialog_private *priv = dial->priv;

    if (priv != NULL)
    {
        int num_fonts = priv->num_fonts;
        int first_line = priv->first_line_displayed;

        int num_strings = SETTINGS_FONT10 - SETTINGS_FONT1 + 1;

        int slsize_permill = num_fonts * 1000L / num_strings;
        int slpos_permill = num_fonts * first_line;

        slsize_permill = slsize_permill > 1000 ? 1000 : slsize_permill;
        slsize_permill = slsize_permill < 0 ? 0 : slsize_permill;

        set_slider_size(dial->dialog_object, SETTINGS_SLIDBAR, SETTINGS_SLIDER, slsize_permill, false);
        set_slider_pos(dial->dialog_object, SETTINGS_SLIDBAR, SETTINGS_SLIDER, slpos_permill, false);
    }

}

static char *items[] =
{
    "Item1",
    "Item2",
    "Item3",
    "Item4",
    "Item5",
    "Item6",
    "Item7",
    "Item8",
    "Item9",
    "Item10",
    "Item11",
    "Item12",
    "Item13",
    "Item14",
    "Item15"
};

void init_prefs_dialog(struct dialog_handler *dial)
{
    int obj_index = SETTINGS_FONT1;     /* index of first line item in the list, we expect these to be sorted */
    int i;
    char num_items = sizeof(items) / sizeof(char *);
    short parent_index;

    struct prefs_dialog_private *priv;

    priv = malloc(sizeof(struct prefs_dialog_private));

    if (priv != NULL)
    {
        dial->priv = priv;

        priv->first_line_displayed = 0;
    }
    else
    {
        form_alert(1, "[1][not enough memory][OK]");
        exit(1);
    }

    obj_index = SETTINGS_FONT1;
    parent_index = get_parent_object(dial->dialog_object, SETTINGS_FONT1);

    printf("num lines: %d\r\n", num_children_of_type(dial->dialog_object, parent_index, G_BOXTEXT));
    i = 0;
    do
    {
        if (dial->dialog_object[obj_index].ob_type == G_BOXTEXT ||
            dial->dialog_object[obj_index].ob_type == G_TEXT)
        {
            int txtlen = strlen(items[i]);

            memcpy(dial->dialog_object[obj_index].ob_spec.tedinfo->te_ptext, items[i], txtlen);
            memset(dial->dialog_object[obj_index].ob_spec.tedinfo->te_ptext + txtlen, ' ',
                   dial->dialog_object[obj_index].ob_spec.tedinfo->te_txtlen - txtlen);
            i++;
        }
        obj_index = dial->dialog_object[obj_index].ob_next;

    } while (obj_index != parent_index && i < num_items);

#ifdef _NOT_USED_
    gdos = vq_gdos();
    if (gdos)
    {
        char name[34];
        short num_system_fonts = work_out[10];

        num_fonts = vst_load_fonts(vdi_handle, 0);      /* load fonts, if not done already */
        num_fonts += num_system_fonts;

        obj_index = SETTINGS_FONT1;
        for (i = 0; i < num_fonts; i++)
        {
            int txtlen = 32;

            vqt_name(vdi_handle, i, name);
            // txtlen = MIN(32, dial->dialog_object[i].ob_spec.tedinfo->te_txtlen);

            memcpy(dial->dialog_object[obj_index].ob_spec.tedinfo->te_ptext, name, txtlen);
            //memset(dial->dialog_object[obj_index].ob_spec.tedinfo->te_ptext + 32, ' ',
            //       dial->dialog_object[obj_index].ob_spec.tedinfo->te_txtlen - txtlen - 1);
            obj_index++;
            if (obj_index > SETTINGS_FONT10)
                break;
        }
    }
    else
    {
        form_alert(1, "[1][GDOS not installed][OK]");
    }
#endif /* _NOT_USED_ */

    //priv->num_fonts = num_fonts;
    prefs_dialog_set_slider(dial);
}

/*
 * forward function prototypes
 */
void multi(void);

int main(int argc, char *argv[])
{
    gl_apid = appl_init();
    phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
    wind_get_grect(0, WF_WORKXYWH, &gl_desk);
    graf_mouse(ARROW, 0x0L);
    vdi_handle = open_vwork(work_out);

    init_global();
    init_util();
    init_windows();
    init_resource();
    init_dialogs();
    init_menu();

    butdown = 1;
    quit = false;

    //about_dialog = create_dialog(ABOUT, &about_exit_handler, NULL);
    prefs_dialog = create_dialog(SETTINGS, &prefs_exit_handler, &prefs_touchexit_callback);

    /*
     * populate dialog
     */
    init_prefs_dialog(prefs_dialog);

    if (argc == 2)
        create_textwindow_with_file(argv[1]);

    multi();
    if (gl_menu != NULL)
    {
        menu_bar(gl_menu, 0);
        gl_menu = NULL;
    }


    free_dialogs();
    free_menu();
    free_windows();
    free_resource();
    free_util();
    free_global();

    return 0;
}

short about_exit_handler(struct dialog_handler *dial, short exit_obj)
{
    return 0;
}

bool prefs_touchexit_callback(struct dialog_handler *dial, short exit_obj)
{
    /*
     * reset selected state of exit object
     */
    dial->dialog_object[exit_obj].ob_state &= ~OS_SELECTED;
    objc_draw(dial->dialog_object, exit_obj, MAX_DEPTH, 0, 0, 0, 0);
    return true;    /* continue */
}

short prefs_exit_handler(struct dialog_handler *dial, short exit_obj)
{
    /*
     * reset selected state of exit object
     */
    dial->dialog_object[exit_obj].ob_state &= ~OS_SELECTED;

    return 0;
}

int timer_cb(struct window *wi)
{
    if (wi->timer) (*wi->timer)(wi);

    return 0;
}

/*
 * event dispatcher
 */
void multi(void)
{
    short event;
    short keystate;
    short keyreturn;
    long msec = 600;
    struct window *wi = NULL;

    do
    {
        event = evnt_multi(MU_MESAG /* | MU_BUTTON */ | MU_KEYBD | MU_TIMER,
                        1, 1, butdown,
                        0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0,
                        msgbuff, msec, &mx, &my, &ret, &keystate, &keyreturn, &ret);

        wind_update(true);

        if (event & MU_MESAG)
        {
            wi = from_handle(msgbuff[3]);
            switch (msgbuff[0])
            {
                case WM_REDRAW:
                    do_redraw(wi, msgbuff[4], msgbuff[5], msgbuff[6], msgbuff[7]);
                    break;

                case WM_NEWTOP:
                case WM_ONTOP:
                case WM_TOPPED:
                    wind_set(wi->handle, WF_TOP, 0, 0, 0, 0);
                    wi->topped = true;
                    break;

                case WM_UNTOPPED:
                    wi->topped = false;
                    break;

                case WM_SIZED:
                case WM_MOVED:
                    if (wi->size) wi->size(wi, msgbuff[4], msgbuff[5], msgbuff[6], msgbuff[7]);
                    break;

                case WM_FULLED:
                    if (wi->full) wi->full(wi);
                    break;

                case WM_CLOSED:
                    if (wi->del) wi->del(wi);
                    break;

                case WM_ARROWED:
                    switch (msgbuff[4])
                    {
                        case WA_UPPAGE:
                            wi->top -= wi->work.g_h / wi->y_fac;
                            break;

                        case WA_DNPAGE:
                            wi->top += wi->work.g_h / wi->y_fac;
                            break;

                        case WA_UPLINE:
                            wi->top--;
                            break;

                        case WA_DNLINE:
                            wi->top++;
                            break;

                        case WA_LFPAGE:
                            wi->left -= wi->doc_width - wi->work.g_w / wi->x_fac;
                            break;

                        case WA_RTPAGE:
                            wi->left += wi->doc_width - wi->work.g_w / wi->x_fac;
                            break;

                        case WA_LFLINE:
                            wi->left--;
                            break;

                        case WA_RTLINE:
                            wi->left++;
                            break;
                    } /* switch */
                    if (wi->top > wi->doc_height - wi->work.g_h / wi->y_fac)
                    {
                        wi->top = wi->doc_height - wi->work.g_h / wi->y_fac;
                    }
                    if (wi->top < 0) wi->top = 0;
                    if (wi->left > wi->doc_width - wi->work.g_w / wi->x_fac)
                    {
                        wi->left = wi->doc_width - wi->work.g_w / wi->x_fac;
                    }
                    if (wi->left < 0) wi->left = 0;
                    if (wi->scroll) wi->scroll(wi);
                    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
                    break;

                case WM_HSLID:
                    wi->left = (int)((float) msgbuff[4] / 1000.0 * wi->doc_width);
                    if (wi->scroll) wi->scroll(wi);
                    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
                    break;

                case WM_VSLID:
                    wi->top = (int)((float) msgbuff[4] / 1000.0 * wi->doc_height);
                    if (wi->scroll) wi->scroll(wi);
                    do_redraw(wi, wi->work.g_x, wi->work.g_y, wi->work.g_w, wi->work.g_h);
                    break;

                case MN_SELECTED:
                    handle_menu(gl_menu, msgbuff[3], msgbuff[4]);
                    menu_tnormal(gl_menu, msgbuff[3], true);
                    break;

                default:
                    printf("unhandled event %x\n", msgbuff[0]);
                    break;
            } /* switch (msgbuff[0]) */
        }

        else if (event & MU_TIMER)
        {
            /*
             * we send a timer event to each window that requests regular scheduling services,
             * no matter if it's on top or not
             */
            foreach_window(timer_cb);
        }


        else if (event & MU_BUTTON)
        {
            if (butdown)
            {
                butdown = 0;
            }
            else
            {
                butdown = 1;
            }
        }
        else if (event & MU_KEYBD)
        {
            short title, item;

            if (is_menu_key(gl_menu, keyreturn, keystate, &title, &item))
            {
                menu_tnormal(gl_menu, title, false);
                handle_menu(gl_menu, title, item);
                menu_tnormal(gl_menu, title, true);
            }
        }
        wind_update(false);
    } while (!quit);

    free_windows();
    exit(0);
}
