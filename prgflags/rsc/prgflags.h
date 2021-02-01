/*
 * resource set indices for prgflags
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        15
 * Number of Bitblks:        1
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       3
 * Number of Free Strings:   0
 * Number of Free Images:    1
 * Number of Objects:        13
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         1
 * Total file size:          708
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "prgflags"
#endif
#undef RSC_ID
#ifdef prgflags
#define RSC_ID prgflags
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 15
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 1
#define NUM_BB 1
#define NUM_FRIMG 1
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 3
#define NUM_OBS 13
#define NUM_TREE 1
#endif



#define PRGFLAGS           0 /* form/dialog */
#define B_OK               2 /* BUTTON in tree PRGFLAGS */
#define B_CANCEL           3 /* BUTTON in tree PRGFLAGS */
#define B_SAVE             5 /* BUTTON in tree PRGFLAGS */
#define PROGLOAD           6 /* BUTTON in tree PRGFLAGS */
#define MEMALLOC           7 /* BUTTON in tree PRGFLAGS */
#define FASTLOAD           8 /* BUTTON in tree PRGFLAGS */
#define FILENAME           9 /* BOXTEXT in tree PRGFLAGS */

#define ICON               0 /* Free image */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD prgflags_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD prgflags_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD prgflags_rsc_free(void);
#endif
