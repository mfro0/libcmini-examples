/* GEM Resource C Source */

#include <portab.h>
#include <aes.h>
#include "USA.H"

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

static TEDINFO rs_tedinfo[] =
{ "Color",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 6, 1,
  "\001",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 2, 1,
  "000",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 4, 1,
  "\002",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 2, 1,
  "\001",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 2, 1,
  "\001",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 2, 1,
  "\001",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 2, 1,
  "0000",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 5, 1,
  "0000",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 5, 1,
  "0000",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 5, 1,
  "\002",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 2, 1,
  "\002",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 2, 1,
  "\002",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 2, 1,
  "Reload",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 7, 1,
  "Save",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 5, 1,
  "Ok",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 3, 1,
  "Cancel",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 7, 1,
  "000",
  "___",
  "999",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 4, 4,
  "0000",
  "____",
  "9999",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 5, 5
};

static OBJECT rs_object[] =
{ 
  /******** Tree 0 COLOR ****************************************************/
        -1, RGBBOX          , BASE1           , G_BOX             ,   /* Object 0  */
  NONE, NORMAL, (LONG)0x00001121L,
  0x0000, 0x0000, 0x0020, 0x000B,
  BASE1           , BASE0           , RGBSLIDE        , G_BOX             ,   /* Object 1 RGBBOX */
  NONE, NORMAL, (LONG)0x00FF1101L,
  0x0100, 0x0000, 0x061F, 0x0609,
  RGBSLIDE        , CTITLE          , BASE4           , G_BOX             ,   /* Object 2 BASE0 */
  NONE, NORMAL, (LONG)0x00FF1100L,
  0x0000, 0x0100, 0x0611, 0x0509,
  BASE16          ,       -1,       -1, G_BOXTEXT         ,   /* Object 3 CTITLE */
  NONE, NORMAL, (LONG)&rs_tedinfo[0],
  0x0000, 0x0000, 0x0611, 0x0001,
  CUP             , BOX0            , BOX15           , G_BOX             ,   /* Object 4 BASE16 */
  HIDETREE, NORMAL, (LONG)0x00FF1100L,
  0x0403, 0x0101, 0x000E, 0x0408,
  BOX1            ,       -1,       -1, G_BOX             ,   /* Object 5 BOX0 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1170L,
  0x0500, 0x0700, 0x0402, 0x0801,
  BOX2            ,       -1,       -1, G_BOX             ,   /* Object 6 BOX1 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1171L,
  0x0004, 0x0700, 0x0402, 0x0801,
  BOX3            ,       -1,       -1, G_BOX             ,   /* Object 7 BOX2 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1172L,
  0x0307, 0x0700, 0x0402, 0x0801,
  BOX4            ,       -1,       -1, G_BOX             ,   /* Object 8 BOX3 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1173L,
  0x060A, 0x0700, 0x0402, 0x0801,
  BOX5            ,       -1,       -1, G_BOX             ,   /* Object 9 BOX4 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1174L,
  0x0500, 0x0602, 0x0402, 0x0801,
  BOX6            ,       -1,       -1, G_BOX             ,   /* Object 10 BOX5 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1175L,
  0x0004, 0x0602, 0x0402, 0x0801,
  BOX7            ,       -1,       -1, G_BOX             ,   /* Object 11 BOX6 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1176L,
  0x0307, 0x0602, 0x0402, 0x0801,
  BOX8            ,       -1,       -1, G_BOX             ,   /* Object 12 BOX7 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1177L,
  0x060A, 0x0602, 0x0402, 0x0801,
  BOX9            ,       -1,       -1, G_BOX             ,   /* Object 13 BOX8 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1178L,
  0x0500, 0x0504, 0x0402, 0x0801,
  BOX10           ,       -1,       -1, G_BOX             ,   /* Object 14 BOX9 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1179L,
  0x0004, 0x0504, 0x0402, 0x0801,
  BOX11           ,       -1,       -1, G_BOX             ,   /* Object 15 BOX10 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF117AL,
  0x0307, 0x0504, 0x0402, 0x0801,
  BOX12           ,       -1,       -1, G_BOX             ,   /* Object 16 BOX11 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF117BL,
  0x060A, 0x0504, 0x0402, 0x0801,
  BOX13           ,       -1,       -1, G_BOX             ,   /* Object 17 BOX12 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF117CL,
  0x0500, 0x0406, 0x0402, 0x0801,
  BOX14           ,       -1,       -1, G_BOX             ,   /* Object 18 BOX13 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF117DL,
  0x0004, 0x0406, 0x0402, 0x0801,
  BOX15           ,       -1,       -1, G_BOX             ,   /* Object 19 BOX14 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF117EL,
  0x0307, 0x0406, 0x0402, 0x0801,
  BASE16          ,       -1,       -1, G_BOX             ,   /* Object 20 BOX15 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF117FL,
  0x060A, 0x0406, 0x0402, 0x0801,
  CBASE           ,       -1,       -1, G_BOXTEXT         ,   /* Object 21 CUP */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[1],
  0x0000, 0x0101, 0x0303, 0x0101,
  CDOWN           , CSLIDE          , CSLIDE          , G_BOX             ,   /* Object 22 CBASE */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1111L,
  0x0000, 0x0202, 0x0303, 0x0106,
  CBASE           ,       -1,       -1, G_BOXTEXT         ,   /* Object 23 CSLIDE */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[2],
  0x0000, 0x0000, 0x0303, 0x0001,
  BASE2           ,       -1,       -1, G_BOXTEXT         ,   /* Object 24 CDOWN */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[3],
  0x0000, 0x0408, 0x0303, 0x0001,
  BASE4           , TWO01           , TWO02           , G_BOX             ,   /* Object 25 BASE2 */
  HIDETREE, NORMAL, (LONG)0x00FF1100L,
  0x0403, 0x0101, 0x020E, 0x0408,
  TWO02           ,       -1,       -1, G_BOX             ,   /* Object 26 TWO01 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1170L,
  0x0001, 0x0602, 0x0105, 0x0703,
  BASE2           ,       -1,       -1, G_BOX             ,   /* Object 27 TWO02 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1171L,
  0x0607, 0x0602, 0x0105, 0x0703,
  BASE0           , FOUR01          , FOUR04          , G_BOX             ,   /* Object 28 BASE4 */
  HIDETREE, NORMAL, (LONG)0x00FF1100L,
  0x0403, 0x0101, 0x020E, 0x0408,
  FOUR02          ,       -1,       -1, G_BOX             ,   /* Object 29 FOUR01 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1170L,
  0x0301, 0x0601, 0x0005, 0x0402,
  FOUR03          ,       -1,       -1, G_BOX             ,   /* Object 30 FOUR02 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1171L,
  0x0707, 0x0601, 0x0005, 0x0402,
  FOUR04          ,       -1,       -1, G_BOX             ,   /* Object 31 FOUR03 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1172L,
  0x0301, 0x0A04, 0x0005, 0x0402,
  BASE4           ,       -1,       -1, G_BOX             ,   /* Object 32 FOUR04 */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1173L,
  0x0707, 0x0A04, 0x0005, 0x0402,
  RGBBOX          , RTITLE          , BDOWN           , G_BOX             ,   /* Object 33 RGBSLIDE */
  NONE, NORMAL, (LONG)0x00FF1100L,
  0x0511, 0x0100, 0x010E, 0x0509,
  GTITLE          ,       -1,       -1, G_BOXCHAR         ,   /* Object 34 RTITLE */
  NONE, NORMAL, (LONG) ((LONG)'R' << 24)|0x00FF1100L,
  0x0100, 0x0000, 0x0404, 0x0001,
  BTITLE          ,       -1,       -1, G_BOXCHAR         ,   /* Object 35 GTITLE */
  NONE, NORMAL, (LONG) ((LONG)'G' << 24)|0x00FF1100L,
  0x0704, 0x0000, 0x0404, 0x0001,
  RUP             ,       -1,       -1, G_BOXCHAR         ,   /* Object 36 BTITLE */
  NONE, NORMAL, (LONG) ((LONG)'B' << 24)|0x00FF1100L,
  0x0509, 0x0000, 0x0404, 0x0001,
  GUP             ,       -1,       -1, G_BOXTEXT         ,   /* Object 37 RUP */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[4],
  0x0100, 0x0101, 0x0404, 0x0001,
  BUP             ,       -1,       -1, G_BOXTEXT         ,   /* Object 38 GUP */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[5],
  0x0704, 0x0101, 0x0404, 0x0001,
  RBASE           ,       -1,       -1, G_BOXTEXT         ,   /* Object 39 BUP */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[6],
  0x0509, 0x0101, 0x0404, 0x0001,
  GBASE           , RSLIDE          , RSLIDE          , G_BOX             ,   /* Object 40 RBASE */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1111L,
  0x0100, 0x0202, 0x0404, 0x0106,
  RBASE           ,       -1,       -1, G_BOXTEXT         ,   /* Object 41 RSLIDE */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[7],
  0x0000, 0x0000, 0x0404, 0x0001,
  BBASE           , GSLIDE          , GSLIDE          , G_BOX             ,   /* Object 42 GBASE */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1111L,
  0x0704, 0x0202, 0x0404, 0x0106,
  GBASE           ,       -1,       -1, G_BOXTEXT         ,   /* Object 43 GSLIDE */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[8],
  0x0000, 0x0000, 0x0404, 0x0001,
  RDOWN           , BSLIDE          , BSLIDE          , G_BOX             ,   /* Object 44 BBASE */
  TOUCHEXIT, NORMAL, (LONG)0x00FF1111L,
  0x0509, 0x0202, 0x0404, 0x0106,
  BBASE           ,       -1,       -1, G_BOXTEXT         ,   /* Object 45 BSLIDE */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[9],
  0x0000, 0x0000, 0x0404, 0x0001,
  GDOWN           ,       -1,       -1, G_BOXTEXT         ,   /* Object 46 RDOWN */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[10],
  0x0100, 0x0408, 0x0404, 0x0001,
  BDOWN           ,       -1,       -1, G_BOXTEXT         ,   /* Object 47 GDOWN */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[11],
  0x0704, 0x0408, 0x0404, 0x0001,
  RGBSLIDE        ,       -1,       -1, G_BOXTEXT         ,   /* Object 48 BDOWN */
  TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[12],
  0x0509, 0x0408, 0x0404, 0x0001,
         0, BASER           , CCANCEL         , G_BOX             ,   /* Object 49 BASE1 */
  NONE, NORMAL, (LONG)0x00FF1100L,
  0x0100, 0x0609, 0x061F, 0x0A01,
  CSAVE           , CRELOAD         , CRELOAD         , G_BOX             ,   /* Object 50 BASER */
  NONE, NORMAL, (LONG)0x00FF1100L,
  0x0218, 0x0000, 0x0407, 0x0A01,
  BASER           ,       -1,       -1, G_BOXTEXT         ,   /* Object 51 CRELOAD */
  SELECTABLE|EXIT, NORMAL, (LONG)&rs_tedinfo[13],
  0x0300, 0x0500, 0x0606, 0x0001,
  COK             ,       -1,       -1, G_BOXTEXT         ,   /* Object 52 CSAVE */
  SELECTABLE|EXIT, NORMAL, (LONG)&rs_tedinfo[14],
  0x0500, 0x0500, 0x0106, 0x0001,
  CCANCEL         ,       -1,       -1, G_BOXTEXT         ,   /* Object 53 COK */
  SELECTABLE|DEFAULT|EXIT, NORMAL, (LONG)&rs_tedinfo[15],
  0x0607, 0x0500, 0x0307, 0x0001,
  BASE1           ,       -1,       -1, G_BOXTEXT         ,   /* Object 54 CCANCEL */
  SELECTABLE|EXIT|LASTOB, NORMAL, (LONG)&rs_tedinfo[16],
  0x0110, 0x0500, 0x0307, 0x0001,
  
  /******** Tree 1 SLIDE1 ****************************************************/
        -1, CTSLIDE         , EXIT1           , G_BOX             ,   /* Object 0  */
  NONE, NORMAL, (LONG)0x00011100L,
  0x0000, 0x0000, 0x0109, 0x0A03,
  EXIT1           ,       -1,       -1, G_FBOXTEXT        ,   /* Object 1 CTSLIDE */
  EDITABLE|TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[17],
  0x0704, 0x0800, 0x0303, 0x0001,
         0,       -1,       -1, G_BUTTON          ,   /* Object 2 EXIT1 */
  SELECTABLE|DEFAULT|EXIT|LASTOB, NORMAL, (LONG)"\0",
  0x0203, 0x0A02, 0x0101, 0x0800,
  
  /******** Tree 2 SLIDE2 ****************************************************/
        -1, CSLIDERS        , EXIT2           , G_BOX             ,   /* Object 0  */
  NONE, NORMAL, (LONG)0x00011100L,
  0x0000, 0x0000, 0x020F, 0x0205,
  EXIT2           ,       -1,       -1, G_FBOXTEXT        ,   /* Object 1 CSLIDERS */
  EDITABLE|TOUCHEXIT, NORMAL, (LONG)&rs_tedinfo[18],
  0x0005, 0x0001, 0x0404, 0x0001,
         0,       -1,       -1, G_BUTTON          ,   /* Object 2 EXIT2 */
  SELECTABLE|DEFAULT|EXIT|LASTOB, NORMAL, (LONG)"\0",
  0x0003, 0x0603, 0x0401, 0x0900
};

static OBJECT *rs_trindex[] =
{ &rs_object[0],   /* Tree  0 COLOR            */
  &rs_object[55],   /* Tree  1 SLIDE1           */
  &rs_object[58]    /* Tree  2 SLIDE2           */
};
