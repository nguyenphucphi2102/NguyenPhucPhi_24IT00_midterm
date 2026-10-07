#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "print.h"
#include "format.h"

/* Print a file name; with -q, non-printable characters become '?'. */
static void print_name(const char *name, const struct options *opt)
{
    const unsigned char *p;

    for (p = (const unsigned char *)name; *p != '\0'; p++) {
        if (opt->nonprint_q && !isprint(*p))
            putchar('?');
        else
            putchar(*p);
    }
}

/* Text of the -s column: bytes in human form with -h, else blocks. */
static void size_text(const struct entry *e, const struct options *opt,
    long long bs, char *buf, size_t len)
{
    if (opt->human)
        humanize((unsigned long long)e->st.st_size, buf, len);
    else
        snprintf(buf, len, "%llu", blocks_in(&e->st, bs));
}

void print_entries(const struct entlist *l, const struct options *opt)
{
    size_t i;
    int iw = 0, sw = 0, n;
    long long bs = block_size(opt);
    char buf[32];

    /* Pass 1: widest inode / size field, so the columns line up. */
    for (i = 0; i < l->n; i++) {
        const struct entry *e = &l->v[i];

        if (opt->inode) {
            n = snprintf(buf, sizeof(buf), "%llu",
                (unsigned long long)e->st.st_ino);
            if (n > iw)
                iw = n;
        }
        if (opt->blocks) {
            size_text(e, opt, bs, buf, sizeof(buf));
            n = (int)strlen(buf);
            if (n > sw)
                sw = n;
        }
    }

    /* Pass 2: print "[inode] [size] name[indicator]". */
    for (i = 0; i < l->n; i++) {
        const struct entry *e = &l->v[i];

        if (opt->inode)
            printf("%*llu ", iw, (unsigned long long)e->st.st_ino);
        if (opt->blocks) {
            size_text(e, opt, bs, buf, sizeof(buf));
            printf("%*s ", sw, buf);
        }
        print_name(e->name, opt);
        if (opt->classify) {
            int c = classify_char(&e->st);

            if (c != 0)
                putchar(c);
        }
        putchar('\n');
    }
}

/* "total N": all blocks of the directory, summed in 512-byte units first. */
static void print_total(const struct entlist *l, const struct options *opt)
{
    unsigned long long sum = 0, bs = (unsigned long long)block_size(opt);
    char buf[32];
    size_t i;

    for (i = 0; i < l->n; i++)
        sum += (unsigned long long)l->v[i].st.st_blocks;

    if (opt->human) {
        humanize(sum * 512ULL, buf, sizeof(buf));
        printf("total %s\n", buf);
    } else {
        printf("total %llu\n", (sum * 512ULL + bs - 1) / bs);
    }
}

void print_dir(const struct entlist *l, const struct options *opt)
{
    /* -s: total only on a terminal; -l: always (long format: Step 7). */
    if (l->n > 0 && (opt->longfmt || (opt->blocks && isatty(STDOUT_FILENO))))
        print_total(l, opt);
    print_entries(l, opt);
}
