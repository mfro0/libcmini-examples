#include <stdio.h>
#include <stdlib.h>

#define MAX_PLANES      8
#define BITS_PER_SHORT  16
#define NUM_COLORS      256

struct planeword
{
    short pattern;
    short mask;
};

/*
 * generate the masks and bit patterns needed for speed optimized hline drawing
 */
void gen_middle_patterns(FILE *fp)
{
    int col;
    unsigned short bitpos;
    unsigned short pixels;
    short pattern;
    int runlength;

    fprintf(fp, "#define MAX_PLANES\t\t8\n");
    fprintf(fp, "#define BITS_PER_SHORT\t%d\n", BITS_PER_SHORT);
    fprintf(fp, "#define NUM_COLORS\t\t%d\n\n", NUM_COLORS);

    fprintf(fp, "\n");
    fprintf(fp, "static short preshifted[BITS_PER_SHORT][NUM_COLORS][2][8] =\n");
    fprintf(fp, "%s", "{\n");

    for (bitpos = 0; bitpos < BITS_PER_SHORT; bitpos++)       /* for all shifts */
    {
        fprintf(fp, "\t/* %d-shifted bit patterns */\n", bitpos);

        fprintf(fp, "%s", "\t{\n");
        for (col = 0; col < NUM_COLORS; col++)
        {
            short set;

            /* pattern */
            fprintf(fp, "%s", "\t\t{\n");
            fprintf(fp, "%s", "\t\t\t{");
            for (pattern = 0; pattern < MAX_PLANES; pattern++)
            {
                set = col & (1 << pattern);
                fprintf(fp, " 0x%04hx,", (unsigned short) (set ? (1U << bitpos) : 0));
            }
            fprintf(fp, "%s", "},\n");

            /* mask */
            fprintf(fp, "%s", "\t\t\t{");
            for (pattern = 0; pattern < MAX_PLANES; pattern++)
            {
                set = col & (1 << pattern);
                fprintf(fp, " 0x%04hx,", (unsigned short) (set ? ~(1U << bitpos) : 0));
            }
            fprintf(fp, "%s", "},\n");
            fprintf(fp, "\t\t},\n");
        }
        fprintf(fp, "%s", "\t},\n");
    }

    fprintf(fp, "};\n");

}

int main(int argc, char *argv[])
{
    FILE *fp;
    char *filename;

    if (argc == 2)
    {
        filename = argv[1];
    }

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "could not open file %s\n", filename);
        exit(1);
    }

    gen_middle_patterns(fp);

    fclose(fp);

    return 0;
}
