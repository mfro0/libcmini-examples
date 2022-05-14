typedef struct
{
    unsigned short magic;               /* Magic constant = 100   */

    struct
    {
        unsigned short reserved : 13;   /* Reserved                */
        unsigned short resident :  1;   /* RAM-resident if set     */
        unsigned short bootinit :  1;   /* Boot initialize if set  */
        unsigned short setonly  :  1;   /* Set-only CPX if set     */

    } flags;

    long cpx_id;                 /* Unique CPX ID                   */
    unsigned short cpx_version;  /* CPX version number              */
    char i_text[14];             /* Icon text                       */
    unsigned short sm_icon[48];  /* Icon bitmap (32*24 pixel)       */
    unsigned short i_color;      /* Icon colour                     */
    char title_text[18];         /* Name of the CPX (16 chars. max) */
    unsigned short t_color;      /* Text colour                     */
    char buffer[64];             /* Non-volatile buffer             */
    char reserved[306];          /* Reserved                        */

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
    .cpx_id = 0x436f6c72,   /* "Colr" */
    .cpx_version = 0x0100,
    .i_text = "Colors",
    .sm_icon = {  0x0000, 0x0000, 0x0078, 0x0000,
                  0x0387, 0x8000, 0x0400, 0x600C,
                  0x0800, 0x1838, 0x1800, 0x0670,
                  0x1C00, 0x01E0, 0x0E00, 0x03C0,
                  0x0600, 0x07C0, 0x1C00, 0x1F20,
                  0x2000, 0x3610, 0x61C0, 0x3C08,
                  0x63E0, 0x5808, 0x63E3, 0xA004,
                  0x71C7, 0xC004, 0x3807, 0x8004,
                  0x1C04, 0x000C, 0x0F00, 0x000C,
                  0x07C0, 0x001C, 0x01F8, 0x0038,
                  0x007F, 0x80F0, 0x001F, 0xFFE0,
                  0x0001, 0xFF80, 0x0000, 0x0000
    },
    .i_color = 0x1180,
    .title_text = "Colors",
    .t_color = 0x1180,
    .buffer = { 0 },
    .reserved = { 0 },
};
