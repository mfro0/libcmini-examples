/* GEM Resource C Source */

#include <portab.h>
#include <aes.h>
#include "CPXCONF.H"

#if !defined(WHITEBAK)
#define WHITEBAK    0x0040
#endif
#if !defined(DRAW3D)
#define DRAW3D      0x0080
#endif

#define FLAGS9  0x0200
#define FLAGS10 0x0400
#define FLAGS11 0x0800
#define FLAGS12 0x1000
#define FLAGS13 0x2000
#define FLAGS14 0x4000
#define FLAGS15 0x8000
#define STATE8  0x0100
#define STATE9  0x0200
#define STATE10 0x0400
#define STATE11 0x0800
#define STATE12 0x1000
#define STATE13 0x2000
#define STATE14 0x4000
#define STATE15 0x8000

static TEDINFO FAR rs_tedinfo[] =
{ "________________",
  "________________",
  "XXXXXXXXXXXXXXXX",
  IBM  , 0, TE_LEFT , 0x1180, 0, -1, 17, 17,
  "CPX Options",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 12, 1,
  "Text Color:",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 12, 1,
  "Icon Color:",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 12, 1,
  "99",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 3, 1,
  "99",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 3, 1,
  "RAM Resident:",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 14, 1
};

static WORD FAR RSBB0DATA[] =
{ 0xFFFF, 0xFFFF, 0xFFFE, 0x8000, 
  0x0000, 0x0002, 0x87FF, 0xFFFF, 
  0xFFC2, 0x8C00, 0x0000, 0x0063, 
  0x983F, 0xF01F, 0xF833, 0x987F, 
  0xF830, 0x0C33, 0x987F, 0xF830, 
  0x0C33, 0x987F, 0xF830, 0x0C33, 
  0x987F, 0xF830, 0x0C33, 0x987F, 
  0xF830, 0x0C33, 0x983F, 0xF01F, 
  0xF833, 0x8C00, 0x0000, 0x0063, 
  0x87FF, 0xFFFF, 0xFFC3, 0x8000, 
  0x0000, 0x0003, 0x9F98, 0x63C3, 
  0xC7F3, 0x861C, 0xE666, 0x6603, 
  0x861B, 0x67E6, 0x07C3, 0x8618, 
  0x6666, 0xE603, 0x9F98, 0x6663, 
  0xE7F3, 0x8000, 0x0000, 0x0003, 
  0x8000, 0x0000, 0x0003, 0xFFFF, 
  0xFFFF, 0xFFFF, 0x1FFF, 0xFFFF, 
  0xFFFF, 0x0000, 0x0000, 0x0000
};

static BITBLK FAR rs_bitblk[] =
{ RSBB0DATA,   6,  24,   0,   0, 0x0001
};

static OBJECT FAR rs_object[] =
{ 
  /******** Tree 0 CPXCONF ****************************************************/
        -1,        1,       22, G_BOX             ,   /* Object 0  */
  NONE, NORMAL, (LONG)0x00FF1141L,
  0x0000, 0x0000, 0x0020, 0x000B,
         3, B_SAVE          , B_SAVE          , G_BOX             ,   /* Object 1  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0000, 0x0808, 0x000A, 0x0802,
         1,       -1,       -1, G_BUTTON          ,   /* Object 2 B_SAVE */
  SELECTABLE|EXIT, NORMAL, (LONG)"Save",
  0x0001, 0x0001, 0x0008, 0x0001,
         6, B_OK            , B_CANCEL        , G_BOX             ,   /* Object 3  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x000A, 0x0808, 0x0016, 0x0802,
  B_CANCEL        ,       -1,       -1, G_BUTTON          ,   /* Object 4 B_OK */
  SELECTABLE|DEFAULT|EXIT, NORMAL, (LONG)"  OK  ",
  0x0601, 0x0001, 0x0008, 0x0001,
         3,       -1,       -1, G_BUTTON          ,   /* Object 5 B_CANCEL */
  SELECTABLE|EXIT, NORMAL, (LONG)"Cancel",
  0x000C, 0x0001, 0x0008, 0x0001,
        11, CPXIMAGE        , NEXT_CPX        , G_BOX             ,   /* Object 6  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0200, 0x0200, 0x041F, 0x0202,
  PREV_CPX        ,       -1,       -1, G_IMAGE           ,   /* Object 7 CPXIMAGE */
  NONE, NORMAL, (LONG)&rs_bitblk[0],
  0x0003, 0x0500, 0x0006, 0x0801,
  CPXNAME         ,       -1,       -1, G_BOXCHAR         ,   /* Object 8 PREV_CPX */
  NONE, NORMAL, (LONG) ((LONG)'C' << 24)|0x00FF1100L,
  0x0000, 0x0000, 0x0002, 0x0202,
  NEXT_CPX        ,       -1,       -1, G_FTEXT           ,   /* Object 9 CPXNAME */
  EDITABLE, NORMAL, (LONG)&rs_tedinfo[0],
  0x000C, 0x0001, 0x0612, 0x0001,
         6,       -1,       -1, G_BOXCHAR         ,   /* Object 10 NEXT_CPX */
  NONE, NORMAL, (LONG) ((LONG)'C' << 24)|0x00FF1100L,
  0x041D, 0x0000, 0x0002, 0x0202,
        13,       12,       12, G_BOX             ,   /* Object 11  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0200, 0x0702, 0x041F, 0x0001,
        11,       -1,       -1, G_TEXT            ,   /* Object 12  */
  NONE, NORMAL, (LONG)&rs_tedinfo[1],
  0x000B, 0x0000, 0x000B, 0x0001,
        22,       14, ICOLOR          , G_BOX             ,   /* Object 13  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0200, 0x0A03, 0x041F, 0x0F02,
        15,       -1,       -1, G_TEXT            ,   /* Object 14  */
  NONE, NORMAL, (LONG)&rs_tedinfo[2],
  0x0501, 0x0100, 0x000B, 0x0001,
  TCOLOR          ,       -1,       -1, G_TEXT            ,   /* Object 15  */
  NONE, NORMAL, (LONG)&rs_tedinfo[3],
  0x0301, 0x0701, 0x000B, 0x0001,
  ICOLOR          , PTCOL           , NTCOL           , G_BOXTEXT         ,   /* Object 16 TCOLOR */
  NONE, NORMAL, (LONG)&rs_tedinfo[4],
  0x0612, 0x0500, 0x000A, 0x0001,
  NTCOL           ,       -1,       -1, G_BOXCHAR         ,   /* Object 17 PTCOL */
  NONE, NORMAL, (LONG) ((LONG)'C' << 24)|0x00FF1100L,
  0x0000, 0x0000, 0x0203, 0x0001,
  TCOLOR          ,       -1,       -1, G_BOXCHAR         ,   /* Object 18 NTCOL */
  SELECTABLE|TOUCHEXIT, NORMAL, (LONG) ((LONG)'\0' << 24)|0x00FF1100L,
  0x0007, 0x0000, 0x0203, 0x0001,
        13, PICOL           , NICOL           , G_BOXTEXT         ,   /* Object 19 ICOLOR */
  NONE, NORMAL, (LONG)&rs_tedinfo[5],
  0x0612, 0x0801, 0x000A, 0x0001,
  NICOL           ,       -1,       -1, G_BOXCHAR         ,   /* Object 20 PICOL */
  NONE, NORMAL, (LONG) ((LONG)'C' << 24)|0x00FF1100L,
  0x0000, 0x0000, 0x0203, 0x0001,
  ICOLOR          ,       -1,       -1, G_BOXCHAR         ,   /* Object 21 NICOL */
  NONE, NORMAL, (LONG) ((LONG)'C' << 24)|0x00FF1100L,
  0x0007, 0x0000, 0x0203, 0x0001,
         0,       23, RES_PUP         , G_BOX             ,   /* Object 22  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0200, 0x0A06, 0x041F, 0x0C01,
  RES_PUP         ,       -1,       -1, G_TEXT            ,   /* Object 23  */
  NONE, NORMAL, (LONG)&rs_tedinfo[6],
  0x0301, 0x0800, 0x000D, 0x0001,
        22,       -1,       -1, G_BUTTON          ,   /* Object 24 RES_PUP */
  SELECTABLE|LASTOB|TOUCHEXIT, SHADOWED, (LONG)"No",
  0x0713, 0x0500, 0x0008, 0x0001
};

static OBJECT FAR *rs_trindex[] =
{ &rs_object[0]    /* Tree  0 CPXCONF          */
};
