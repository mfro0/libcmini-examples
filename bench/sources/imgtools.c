#include "imgtools.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

// #define DEBUG
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


struct image
{
    MFDB mfdb;
    short imgdata[];
};

MFDB *get_mfdb(struct image *img)
{
    return &img->mfdb;
}

short *get_imgdata(struct image *img)
{
    return img->imgdata;
}

struct image *create_image_like(struct image *like_image)
{
    struct image *new_image;
    
    new_image = calloc(1, sizeof(struct image) + 
                       (size_t) like_image->mfdb.fd_wdwidth * like_image->mfdb.fd_h * like_image->mfdb.fd_nplanes * sizeof(short));
    if (new_image != NULL)
    {
        new_image->mfdb = like_image->mfdb;
        new_image->mfdb.fd_addr = new_image->imgdata;
        memcpy(new_image->imgdata, like_image->imgdata, like_image->mfdb.fd_wdwidth * like_image->mfdb.fd_h * 
               like_image->mfdb.fd_nplanes * sizeof(short));
    }

    assert(new_image != NULL);
    return new_image;
}

/*
 * create a struct image in memory (empty) from width, height and number of planes
 * The image is in device specific format (fd_stand = 0)
 */
struct image *create_image_whp(short width, short height, short nplanes)
{
    struct image *new_image;
    short wdwidth = (width + 15) / 16;
    
    dbg("width=%d, height=%d, nplanes=%d\r\n", width, height, nplanes);
    
    new_image = calloc(1, sizeof(struct image) +
                       (size_t) wdwidth * height * nplanes * sizeof(short));
    if (new_image != NULL)
    {
        new_image->mfdb.fd_addr = new_image->imgdata;
        new_image->mfdb.fd_wdwidth = wdwidth;
        new_image->mfdb.fd_nplanes = nplanes;
        new_image->mfdb.fd_h = height;
        new_image->mfdb.fd_w = width;
        new_image->mfdb.fd_stand = 0;
    }
    assert(new_image != NULL);
    
    return new_image;
}

struct image *create_image_mfdb(MFDB *mfdb)
{
    struct image *new_image;
    new_image = calloc(1, sizeof(struct image) +
                       (size_t) mfdb->fd_wdwidth * mfdb->fd_h * mfdb->fd_nplanes * sizeof(short));
    if (new_image != NULL)
    {
        new_image->mfdb = *mfdb;
        new_image->mfdb.fd_addr = new_image->imgdata;
        memcpy(new_image->imgdata, mfdb->fd_addr, mfdb->fd_wdwidth * mfdb->fd_h * mfdb->fd_nplanes * sizeof(short));
    }
    assert(new_image != NULL);
    return new_image;
}

void delete_image(struct image *image)
{
    free(image);
}

/*
 * return an image that is rotated <rotations> * 90 degrees
 */
struct image *integral_rotate_image(struct window *wi, struct image *src, short rotations)
{
    struct image *dst = rotations & 1 ? create_image_whp(src->mfdb.fd_h, src->mfdb.fd_w, src->mfdb.fd_nplanes) :
                                        create_image_like(src);
    short vh = wi->vdi_handle;

    if (dst)
    {
        short pxy[8];
        
        assert(dst != NULL);
        
        switch (rotations)
        {
            case 0:
            default:
                pxy[0] = pxy[1] = pxy[4] = pxy[5] = 0;
                pxy[2] = pxy[6] = src->mfdb.fd_w - 1;
                pxy[3] = pxy[7] = src->mfdb.fd_h - 1;
                vro_cpyfm(vh, S_ONLY, pxy, &src->mfdb, &dst->mfdb);
                break;
                
            case 1: /* 90 degrees counter clockwise */
                /* swap rows and reverse columns */
                //dst->mfdb.fd_w = src->mfdb.fd_h;
                //dst->mfdb.fd_h = src->mfdb.fd_w;
                //dst->mfdb.fd_wdwidth = (dst->mfdb.fd_w + 15) / 16;
                for (short y = 0; y < src->mfdb.fd_h; y++)   /* rows */
                {
                    for (short x = 0; x < src->mfdb.fd_w; x++) /* columns */
                    {
                        pxy[0] = x;
                        pxy[1] = y;
                        pxy[2] = x;
                        pxy[3] = y;
                        
                        pxy[4] = y;
                        pxy[5] = dst->mfdb.fd_h - x - 1;
                        pxy[6] = y;
                        pxy[7] = dst->mfdb.fd_h - x - 1;
                        
                        vro_cpyfm(vh, S_ONLY, pxy, &src->mfdb, &dst->mfdb);
                    }
                }
                break;
                
            case 2:
                /* 180 degrees counter clockwise (aka upside-down) */
                for (short y = 0; y < src->mfdb.fd_h; y++)
                {
                    for (short x = 0; x < src->mfdb.fd_w; x++)
                    {
                        pxy[0] = x;
                        pxy[1] = y;
                        pxy[2] = x;
                        pxy[3] = y;
                        
                        pxy[4] = dst->mfdb.fd_w - x - 1;
                        pxy[5] = dst->mfdb.fd_h - y - 1;
                        pxy[6] = dst->mfdb.fd_w - x - 1;
                        pxy[7] = dst->mfdb.fd_h - y - 1;
                        
                        vro_cpyfm(vh, S_ONLY, pxy, &src->mfdb, &dst->mfdb);
                    }
                }
                break;
                
            case 3:
                /* 270 degrees counter clockwise */
                //dst->mfdb.fd_w = src->mfdb.fd_h;
                //dst->mfdb.fd_h = src->mfdb.fd_w;
                //dst->mfdb.fd_wdwidth = (dst->mfdb.fd_w + 15) / 16;
                for (short y = 0; y < src->mfdb.fd_h; y++)
                {
                    for (short x = 0; x < src->mfdb.fd_w; x++)
                    {
                        pxy[0] = x;
                        pxy[1] = y;
                        pxy[2] = x;
                        pxy[3] = y;
                        
                        pxy[4] = dst->mfdb.fd_w - 1 - y;
                        pxy[5] = x;
                        pxy[6] = dst->mfdb.fd_w - 1 - y;
                        pxy[7] = x;
                        
                        vro_cpyfm(vh, S_ONLY, pxy, &src->mfdb, &dst->mfdb);
                    }
                }
                break;
        }
        return dst;
    }
    return NULL;
}

struct image *shear_rotate_image(struct window *wi, struct image *src[], short angle)
{
    short n90rot;
    short rest_angle;
    struct image *integral_img;
    struct image *x_sheared_img, *y_sheared_img;

    /*
     * adjust rotation angle so that we need to shear-rotate by a maximum amount of +/- 45°
     */
    rest_angle = angle % 3600;
    n90rot = rest_angle / 900;
    rest_angle = rest_angle % 900;
    if (rest_angle > 450)
    {
        rest_angle = rest_angle - 900;
        n90rot = (n90rot + 1) % 4;
    }
    
    integral_img = src[n90rot];

    dbg("angle=%d, n_90rot=%d, rest_angle=%d\r\n", angle, n_90rot, rest_angle);

    /*
     * doesn't work for negative angles (which we need)
     */
    //short shear_x = rest_angle < 0 ? -itan(-rest_angle / 2) : -itan(rest_angle / 2);
    //short shear_y = rest_angle < 0 ? isin(-rest_angle) : isin(rest_angle);
    
    short shear_x = (short) (-tan(rest_angle / 10.0 / 2.0 * M_PI / 180.0) * SHRT_MAX);
    short shear_y = (short) (sin(rest_angle / 10.0 * M_PI / 180.0) * SHRT_MAX);
    
    if (shear_x == 0 && shear_y == 0)
        return create_image_like(integral_img);

    x_sheared_img = x_shear(wi, integral_img, shear_x);
    
    y_sheared_img = y_shear(wi, x_sheared_img, shear_y);
    delete_image(x_sheared_img);
                
    x_sheared_img = x_shear(wi, y_sheared_img, shear_x);
    delete_image(y_sheared_img);
    
    return x_sheared_img;
}

/*
 * shear image in x direction.
 * shear_x (the tangent of the shear angle) is given in units of 1/SHRT_MAX.
 */
struct image *x_shear(struct window *wi, struct image *src, short shear_x)
{
    short winc = (short) abs((long) src->mfdb.fd_h * shear_x / SHRT_MAX);
    dbg("winc=%d\r\n", winc);
    
    short new_width = src->mfdb.fd_w + winc;
    short new_height = src->mfdb.fd_h;
    struct image *sheared = create_image_whp(new_width, new_height, src->mfdb.fd_nplanes);
    short pxy[8] = {0, 0, new_width - 1, new_height - 1, 0, 0, new_width - 1, new_height - 1 };
    
    assert(sheared != NULL);
    
    vro_cpyfm(wi->vdi_handle, sheared->mfdb.fd_nplanes <= 8 ? ALL_WHITE : ALL_BLACK, pxy, &src->mfdb, &sheared->mfdb);
    
    if (sheared != NULL)
    {
        for (int i = 0; i < src->mfdb.fd_h; i++)
        {
            
            short skew = shear_x > 0 ?
                             (short)((long) i * (long) shear_x / SHRT_MAX) :
                             -(short)((long) (src->mfdb.fd_h - i - 1) * (long) shear_x / SHRT_MAX);
            short left = 0;
            short right = src->mfdb.fd_w - 1;
            
            pxy[0] = left;
            pxy[1] = i;
            pxy[2] = right;
            pxy[3] = i;
            
            pxy[4] = winc - skew;
            pxy[5] = i;
            pxy[6] = pxy[4] + src->mfdb.fd_w - 1;
            pxy[7] = i;
            vro_cpyfm(wi->vdi_handle, S_ONLY, pxy, &src->mfdb, &sheared->mfdb);
        }
    }
    return sheared;
}

struct image *y_shear(struct window *wi, struct image *src, short shear_y)
{    
    short hinc = (short) abs((long) src->mfdb.fd_w * shear_y / SHRT_MAX);
    dbg("hinc=%d\r\n", hinc);
    
    short new_width = src->mfdb.fd_w;
    short new_height = src->mfdb.fd_h + hinc;
    struct image *sheared = create_image_whp(new_width, new_height, src->mfdb.fd_nplanes);
    short pxy[8] = {0, 0, new_width - 1, new_height - 1, 0, 0, new_width - 1, new_height - 1 };
    
    assert(sheared != NULL);
    
    vro_cpyfm(wi->vdi_handle, sheared->mfdb.fd_nplanes <= 8 ? ALL_WHITE : ALL_BLACK, pxy, &src->mfdb, &sheared->mfdb);
    
    if (sheared != NULL)
    {
        for (int i = 0; i < src->mfdb.fd_w; i++)
        {
            
            short skew = shear_y > 0 ? (short)((long) i * (long) shear_y / SHRT_MAX) :
                                       -(short)((long) (src->mfdb.fd_w - i - 1) * (long) shear_y / SHRT_MAX);
            short top = 0;
            short bottom = src->mfdb.fd_h - 1;
            
            pxy[0] = i;
            pxy[1] = top;
            pxy[2] = i;
            pxy[3] = bottom;
            
            
            pxy[4] = i;
            pxy[5] = hinc - skew; 
            pxy[6] = i;
            pxy[7] = pxy[5] + src->mfdb.fd_h - 1;
            dbg("pxy[] = {%d, %d, %d, %d, %d, %d, %d, %d}\r\n",
                pxy[0], pxy[1], pxy[2], pxy[3], pxy[4], pxy[5], pxy[6], pxy[7]);
            vro_cpyfm(wi->vdi_handle, S_ONLY, pxy, &src->mfdb, &sheared->mfdb);
        }
    }
    return sheared;
}

