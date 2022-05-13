#include <stdint.h>

typedef struct
{
    uint16_t magic;                     /* Magic constant = 100   */

    struct
    {
        uint16_t reserved : 13;     /* Reserved                */
        uint16_t resident :  1;     /* RAM-resident if set     */
        uint16_t bootinit :  1;     /* Boot initialize if set  */
        uint16_t setonly  :  1;     /* Set-only CPX if set     */

    } flags;

    int32_t cpx_id;                 /* Unique CPX ID                   */
    uint16_t cpx_version;           /* CPX version number              */
    int8_t i_text[14];              /* Icon text                       */
    uint16_t sm_icon[48];           /* Icon bitmap (32*24 pixel)       */
    uint16_t i_color;               /* Icon colour                     */
    int8_t title_text[18];          /* Name of the CPX (16 chars. max) */
    uint16_t t_color;               /* Text colour                     */
    int8_t buffer[64];              /* Non-volatile buffer             */
    int8_t reserved[306];           /* Reserved                        */

} CPXHEAD;

const CPXHEAD header =
{
    .magic = 100,
    .flags =
    {
        .reserved = 0,
        .resident = 0,
        .bootinit = 1,
        .setonly = 0
    },
    .cpx_id = 0x436f6c72,
    .cpx_version = 0x0100,
    .i_text = "Colors",
    .sm_icon = { 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0800,
                 0x0000,0x1800,0x0000,0x3c00,0x0000,0x7e00,0x0000,0x9f00,
                 0x0001,0x0f80,0x0002,0x07c0,0x0007,0x03e0,0x0009,0x81f0,
                 0x0010,0xc0f8,0x0020,0x6070,0x0040,0x3020,0x0080,0x1840,
                 0x0100,0x0c80,0x0200,0x0700,0x0400,0x0200,0x0800,0x0000,
                 0x1000,0x0000,0x2000,0x0000,0x0000,0x0000,0x0000,0x0000 },
    .i_color = 0x1180,
    .title_text = "Colors",
    .t_color = 0x1180,
    .buffer = { 0 },
    .reserved = { 0 },
};
