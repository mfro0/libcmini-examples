/*
 * natfeat.c - NatFeats API examples
 *
 * Copyright (c) 2014 by Eero Tamminen
 *
 * NF initialization & calling is based on EmuTOS code,
 * Copyright (c) 2001-2003 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#if __GNUC__
# include <mint/osbind.h>
#else	/* VBCC/AHCC/Pure-C */
# include <tos.h>
#endif
#include "natfeats.h"
#include <stdarg.h>
#include <stdlib.h>

/* NatFeats available & initialized */
static int nf_ok;

/* handles for NF features that may be used more frequently */
static long nfid_print;
static long nfid_debugger;
static long nfid_fastforward;


/* API documentation is in natfeats.h header */

int nf_init(void)
{
    void *sup = (void*)Super(0);
    nf_ok = detect_nf();
    Super(sup);

    if (nf_ok)
    {
        /* initialize commonly used handles */
        nfid_print = nf_id("NF_STDERR");
        nfid_debugger = nf_id("NF_DEBUGGER");
        nfid_fastforward = nf_id("NF_FASTFORWARD");
    }
    else
    {
        (void) Cconws("Native Features initialization failed!\r\n");
    }
    return nf_ok;
}

long nf_print(const char *text)
{
    if (nfid_print)
    {
        return nf_call(nfid_print, text);
    }
    else
    {
        (void) Cconws("NF_STDERR unavailable!\r\n");
        return 0;
    }
}

long nf_debugger(void)
{
    if (nfid_debugger) {
        return nf_call(nfid_debugger);
    } else {
        (void) Cconws("NF_DEBUGGER unavailable!\r\n");
        return 0;
    }
}

long nf_fastforward(long enabled)
{
    if (nfid_fastforward) {
        return nf_call(nfid_fastforward, enabled);
    } else {
        (void) Cconws("NF_FASTFORWARD unavailable!\r\n");
        return 0;
    }
}

void nf_shutdown(void)
{
    long id;
    if(nf_ok && (id = nf_id("NF_SHUTDOWN"))) {
        void *sup = (void*)Super(0);
        /* needs to be called in supervisor mode */
        nf_call(id);
        Super(sup);
    } else {
        (void) Cconws("NF_SHUTDOWN unavailable!\r\n");
    }
}

void nf_exit(long exitval)
{
    long id;
    if(nf_ok && (id = nf_id("NF_EXIT"))) {
        nf_call(id, exitval);
    } else {
        /* NF_EXIT is Hatari specific, NF_SHUTDOWN isn't */
        (void) Cconws("NF_EXIT unavailable, trying NF_SHUTDOWN...\r\n");
        nf_shutdown();
    }
}

void nf_putchar(int c)
{
    char s[2];

    s[0] = (char) c;
    s[1] = '\0';
    nf_print(s);
}

extern int doprnt(int (*)(int, void *), char *, const char *, va_list);

int nf_printf(const char *fmt, ...)
{
    int ret;

    va_list va;
    va_start(va, fmt);

    if (! nf_ok)
        nf_init();

    ret = doprnt((int (*)(int, void *)) nf_putchar, NULL, fmt, va);
    va_end(va);

    return ret;
}


#ifdef TEST

/* show emulator name */
static void nf_showname(void)
{
    long id;
    if (nf_ok && (id = nf_id("NF_NAME"))) {
        long chars;
        char buffer[64];
        id |= 0x0001;  /* name + version */
        chars = nf_call(id, buffer, sizeof(buffer)-2);
        buffer[chars++] = '\n';
        buffer[chars++] = '\0';
        nf_print(buffer);
    } else {
        (void) Cconws("NF_NAME unavailable!\r\n");
    }
}

static int wait_key(void)
{
    while (Cconis()) {
        Cconin();
    }
    (void) Cconws("\r\n<press key>\r\n");
    return Cconin();
}

int main()
{
    long old_ff;
    if (!nf_init()) {
        wait_key();
        return 1;
    }
    old_ff = nf_fastforward(1);
    nf_print("Emulator name:\n");
    nf_showname();
    nf_print("Shutting down...\n");
    nf_fastforward(old_ff);
    nf_exit(0);
    wait_key();
    return 0;
}

#endif
