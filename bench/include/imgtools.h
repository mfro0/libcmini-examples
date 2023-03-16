#ifndef IMGTOOLS_H
#define IMGTOOLS_H

#include <gem.h>
#include "window.h"

struct image;

struct image *integral_rotate_image(struct window *wi, struct image *src, short rotations);
struct image *y_shear(struct window *wi, struct image *src, short shear_y);
struct image *x_shear(struct window *wi, struct image *src, short shear_x);
struct image *shear_rotate_image(struct window *wi, struct image *src[], short angle);
struct image *create_image_mfdb(MFDB *mfdb);
struct image *create_image_like(struct image *);
struct image *create_image_whp(short, short, short);
void delete_image(struct image *img);
MFDB *get_mfdb(struct image *img);
short *get_imgdata(struct image *img);

#endif // IMGTOOLS_H
