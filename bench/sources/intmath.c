#include "intmath.h"

//#define DEBUG
#ifdef DEBUG
#include "natfeats.h"
#define dbg(format, arg...) do { nf_printf("DEBUG: (%s):" format, __FUNCTION__, ##arg); } while (0)
#define out(format, arg...) do { nf_printf("" format, ##arg); } while (0)
#else
#define dbg(format, arg...) do { ; } while (0)
#endif /* DEBUG */


/*
 * Sines of angles 1 - 90 degrees normalized between 0-32767.
 *
 * Courtesy of the EmuTOS development team
 */
static const short sin_tbl[92] =
{
        0,   572, 1144,   1716,  2286,  2856,  3425,  3993,
     4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
     9032,  9580, 10126, 10668, 11207, 11743, 12275, 12803,
    13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621,
    21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841,
    27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982,
    31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722,
    32747, 32762, 32767, 32767
};


#define HALFPI      900
#define PI          1800
#define TWOPI       3600

/*
 * ISin - Returns integer sin between -32767 - 32767.
 *
 * Uses integer lookup table sintable^[]. Expects angle in tenths of
 * degree 0 - 3600. Assumes positive angles only.
 */
short isin(short angle)
{
    short index;
    short remainder;
    short tmpsin;                           /* holder for sin. */
    short quadrant;                         /* 0-3 = 1st, 2nd, 3rd, 4th.        */

    while (angle > 3600)
        angle -= 3600;

    quadrant = angle / HALFPI;

    switch (quadrant)
    {
    case 0:
        break;

    case 1:
        angle = PI - angle;
        break;

    case 2:
        angle -= PI;
        break;

    case 3:
        angle = TWOPI - angle;
        break;

    case 4:
        angle -= TWOPI;
        break;
    };
    index = angle / 10;
    remainder = angle % 10;

    tmpsin = sin_tbl[index];

    if (remainder != 0)         /* add interpolation. */
        tmpsin += ((sin_tbl[index + 1] - tmpsin) * remainder) / 10;

    if (quadrant > 1)
        tmpsin = -tmpsin;
    return (tmpsin);
}



/*
 * Icos - Return integer cos between -32767 and 32767.
 */
short icos(short angle)
{
    angle = angle + HALFPI;

    return isin(angle);
}

/*
 * Itan - Return integer tangens between -32767 and 32767
 */
short itan(short angle)
{
    return (short) (32767L * isin(angle) / icos(angle));
}
