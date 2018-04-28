#ifndef _UTIL_H_
#define _UTIL_H_

#define RANDOM_MAX 0xffffffUL
/*
 * return a random number in range min...max inclusively
 *
 * TODO: this belongs elsewhere, but not here
 */
static inline int random(const int min, const int max)
{
    return (Random() >> 8) * ((long) max - min) / (RANDOM_MAX >> 8) + min;
}

/*
 * return the smaller of two values
 */
static inline int min(int a, int b)
{
    return (a < b ? a : b);
}

/*
 * return the larger of two values
 */
static inline int max(int a, int b)
{
    return (a > b ? a : b);
}

extern short init_util(void);
extern short free_util(void);
extern short open_vwork(short work_out[]);
extern void set_clipping(short handle, short x, short y, short w, short h, short on);
extern void fix_menu(OBJECT *tree);

#endif /* _UTIL_H_ */

