#ifndef CPXDATA_H
#define CPXDATA_H

#include <gem.h>

#include <stdint.h>

typedef struct
{
    short x;
    short y;
    short buttons;
    short kstate;
} MRETS;

typedef struct
{
    short (*cpx_call)(GRECT *work);
    void (*cpx_draw)(GRECT *clip);
    void (*cpx_wmove)(GRECT *work);

    void (*cpx_timer)(short *quit);
    void (*cpx_key)(short kstate, short key, short *quit);
    void (*cpx_button)(MRETS *mrets, short nclicks, short *quit);
    void (*cpx_m1)(MRETS *mrets, short *quit);
    void (*cpx_m2)(MRETS *mrets, short *quit);
    bool (*cpx_hook)(short event, short *msg, MRETS *mrets,
                     short *key, short *nclicks);

    void (*cpx_close)( bool flag );
} CPXINFO;

typedef struct
{
    short handle;
    short booting;
    short reserved;
    short SkipRshFix;
    void *reserve1;
    void *reserve2;

    void (*rsh_fix)(short num_objs, short num_frstr, short num_frimg, short num_tree,
                    OBJECT *rs_object, TEDINFO *rs_tedinfo, char **rs_strings, ICONBLK *rs_iconblk,
                    BITBLK *rs_bitblk, long *rs_frstr, long *rs_frimg, long *rs_trindex, struct foobar *rs_imdope);
    void (*rsh_obfix)(OBJECT *tree, short curobj);
    short (*Popup)(char **items, short num_items, short default_item, short font_size, GRECT *button, GRECT *world);

    void (*Sl_size)(OBJECT *tree, short base, short slider, short num_items,
                    short visible, short direction, short min_size);
    void (*Sl_x)(OBJECT *tree, short base, short slider, short value,
                       short num_min, short num_max, void (*foo)());
    void (*Sl_y)(OBJECT *tree, short base, short slider,  short value,
                       short num_min, short num_max, short (*foo)());
    void (*Sl_arrow)(OBJECT *tree, short base, short slider, short obj,
                     short inc, short min, short max, short *numvar,
                     short direction, void (*foo)());
    void (*Sl_dragx)(OBJECT *tree, short base, short slider,
                     short min, short max, short *numvar, void (*foo)());
    void (*Sl_dragy)(OBJECT *tree, short base, short slider,
                     short min, short max, short *numvar, short (*foo)());
    short (*Xform_do)(OBJECT *tree, short startobj, short *puntmsg);
    GRECT *(*GetFirstRect)(GRECT *prect);
    GRECT *(*GetNextRect)(void);
    void (*Set_Evnt_Mask)(short mask, MOBLK *m1, MOBLK *m2, long time );
    short (*XGen_Alert)(int id);
    short (*CPX_Save)(void *ptr, long num);
    void *  (*Get_Buffer)(void);
    short (*getcookie)(long cookie, long *p_value);
    short Country_Code;
    short (*MFsave)(int saveit, MFORM *mf);
} XCPB;

#endif /* CPXDATA_H */
