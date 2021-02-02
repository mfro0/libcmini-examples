#ifndef CPXHEAD_H
#define CPXHEAD_H

typedef struct
{
    short magic;                     /* Magic constant = 100   */

    struct
    {
        short reserved : 13;     /* Reserved                */
        short resident :  1;     /* RAM-resident if set     */
        short bootinit :  1;     /* Boot initialize if set  */
        short setonly  :  1;     /* Set-only CPX if set     */

    } flags;

    long cpx_id;                    /* Unique CPX ID                   */
    unsigned short cpx_version;     /* CPX version number              */
    char i_text[14];                /* Icon text                       */
    unsigned short sm_icon[48];     /* Icon bitmap (32*24 pixel)       */
    unsigned short i_color;         /* Icon colour                     */
    char title_text[18];            /* Name of the CPX (16 chars. max) */
    unsigned short t_color;         /* Text colour                     */
    char buffer[64];                /* Non-volatile buffer             */
    char reserved[306];             /* Reserved                        */
} CPXHEAD;

#endif // CPXHEAD_H
