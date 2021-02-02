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

TEDINFO FAR rs_tedinfo[] =
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

WORD FAR RSBB0DATA[] =
{ 0xFFFF, 0xFFFF, 0x8000, 0x0001, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0xBFFF, 0xFFFD, 0xBFFF, 0xFFFD, 
  0x8000, 0x0001, 0xFFFF, 0xFFFF
};

BITBLK FAR rs_bitblk[] =
{ RSBB0DATA,   4,  24,   0,   0, 0x0001
};

OBJECT FAR rs_object[] =
{ 
  /******** Tree 0 CPXCONF ****************************************************/
        -1,        1,       22, G_BOX             ,   /* Object 0  */
  NONE, NORMAL, (LONG)0x00FF1141L,
  0x0000, 0x0000, 0x0020, 0x000B,
         3, BSAVE           , BSAVE           , G_BOX             ,   /* Object 1  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0000, 0x0808, 0x000A, 0x0802,
         1,       -1,       -1, G_BUTTON          ,   /* Object 2 BSAVE */
  SELECTABLE|EXIT, NORMAL, (LONG)"Save",
  0x0001, 0x0001, 0x0008, 0x0001,
         6, BOK             , BCANCEL         , G_BOX             ,   /* Object 3  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x000A, 0x0808, 0x0016, 0x0802,
  BCANCEL         ,       -1,       -1, G_BUTTON          ,   /* Object 4 BOK */
  SELECTABLE|DEFAULT|EXIT, NORMAL, (LONG)"  OK  ",
  0x0601, 0x0001, 0x0008, 0x0001,
         3,       -1,       -1, G_BUTTON          ,   /* Object 5 BCANCEL */
  SELECTABLE|EXIT, NORMAL, (LONG)"Cancel",
  0x000C, 0x0001, 0x0008, 0x0001,
        11, CPXICON         , NCPX            , G_BOX             ,   /* Object 6  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0200, 0x0200, 0x041F, 0x0202,
  PCPX            ,       -1,       -1, G_IMAGE           ,   /* Object 7 CPXICON */
  NONE, NORMAL, (LONG)&rs_bitblk[0],
  0x0003, 0x0500, 0x0004, 0x0801,
  CPXNAME         ,       -1,       -1, G_BOXCHAR         ,   /* Object 8 PCPX */
  SELECTABLE|EXIT, NORMAL, (LONG) ((LONG)'\004' << 24)|0x00FF1100L,
  0x0000, 0x0000, 0x0002, 0x0202,
  NCPX            ,       -1,       -1, G_FTEXT           ,   /* Object 9 CPXNAME */
  EDITABLE, NORMAL, (LONG)&rs_tedinfo[0],
  0x000A, 0x0900, 0x0612, 0x0001,
         6,       -1,       -1, G_BOXCHAR         ,   /* Object 10 NCPX */
  SELECTABLE|EXIT, NORMAL, (LONG) ((LONG)'\003' << 24)|0x00FF1100L,
  0x041D, 0x0000, 0x0002, 0x0202,
        13,       12,       12, G_BOX             ,   /* Object 11  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0200, 0x0702, 0x041F, 0x0001,
        11,       -1,       -1, G_TEXT            ,   /* Object 12  */
  NONE, NORMAL, (LONG)&rs_tedinfo[1],
  0x000B, 0x0000, 0x000B, 0x0001,
        22,       14, ICNCOL          , G_BOX             ,   /* Object 13  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0200, 0x0A03, 0x041F, 0x0D02,
        15,       -1,       -1, G_TEXT            ,   /* Object 14  */
  NONE, NORMAL, (LONG)&rs_tedinfo[2],
  0x0301, 0x0300, 0x000B, 0x0001,
  TXTCOL          ,       -1,       -1, G_TEXT            ,   /* Object 15  */
  NONE, NORMAL, (LONG)&rs_tedinfo[3],
  0x0301, 0x0801, 0x000B, 0x0001,
  ICNCOL          , PTXTCOL         , NTXTCOL         , G_BOXTEXT         ,   /* Object 16 TXTCOL */
  NONE, NORMAL, (LONG)&rs_tedinfo[4],
  0x0612, 0x0500, 0x000A, 0x0001,
  NTXTCOL         ,       -1,       -1, G_BOXCHAR         ,   /* Object 17 PTXTCOL */
  SELECTABLE|EXIT, NORMAL, (LONG) ((LONG)'\004' << 24)|0x00FF1100L,
  0x0000, 0x0000, 0x0203, 0x0001,
  TXTCOL          ,       -1,       -1, G_BOXCHAR         ,   /* Object 18 NTXTCOL */
  SELECTABLE|EXIT, NORMAL, (LONG) ((LONG)'\003' << 24)|0x00FF1100L,
  0x0007, 0x0000, 0x0203, 0x0001,
        13, PICNCOL         , NICNCOL         , G_BOXTEXT         ,   /* Object 19 ICNCOL */
  NONE, NORMAL, (LONG)&rs_tedinfo[5],
  0x0612, 0x0801, 0x000A, 0x0001,
  NICNCOL         ,       -1,       -1, G_BOXCHAR         ,   /* Object 20 PICNCOL */
  SELECTABLE|EXIT, NORMAL, (LONG) ((LONG)'\004' << 24)|0x00FF1100L,
  0x0000, 0x0000, 0x0203, 0x0001,
  ICNCOL          ,       -1,       -1, G_BOXCHAR         ,   /* Object 21 NICNCOL */
  SELECTABLE|EXIT, NORMAL, (LONG) ((LONG)'\003' << 24)|0x00FF1100L,
  0x0007, 0x0000, 0x0203, 0x0001,
         0,       23, RESPUP          , G_BOX             ,   /* Object 22  */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0200, 0x0A06, 0x041F, 0x0C01,
  RESPUP          ,       -1,       -1, G_TEXT            ,   /* Object 23  */
  NONE, NORMAL, (LONG)&rs_tedinfo[6],
  0x0501, 0x0600, 0x000D, 0x0001,
        22,       -1,       -1, G_BUTTON          ,   /* Object 24 RESPUP */
  SELECTABLE|EXIT|LASTOB, SHADOWED, (LONG)"No",
  0x0713, 0x0600, 0x0008, 0x0001
};

OBJECT FAR *rs_trindex[] =
{ &rs_object[0]    /* Tree  0 CPXCONF          */
};
