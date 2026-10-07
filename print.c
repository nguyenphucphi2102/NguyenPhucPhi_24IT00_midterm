#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "print.h"
#include "format.h"
#include "longfmt.h"
#include "util.h"

/* Column widths of one listing, so that the output lines up. */
struct cols {
    int iw;     /* inode number (-i)          */
    int sw;     /* block count (-s)           */
    int lw;     /* link count (-l)            */
    int uw;     /* owner                      */
    int gw;     /* group                      */
    int zw;     /* size, or "major, minor"    */
    int minw;   /* digits of the minor number */
};

static int max_int(int a, int b)
{
    return a > b ? a : b;
}

/* Number of decimal digits of v. */
static int ndigits(unsigned long long v)
{
    int n = 1;

    while (v >= 10) {
        v /= 10;
        n++;
    }
    return n;
}

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

/* Size in bytes for the long format; human readable with -h. */
static void byte_text(const struct stat *st, const struct options *opt,
    char *buf, size_t len)
{
    if (opt->human)
        humanize((unsigned long long)st->st_size, buf, len);
    else
        snprintf(buf, len, "%llu", (unsigned long long)st->st_size);
}

/* Compute the widest value of every column over the whole listing. */
static void widths(const struct entlist *l, const struct options *opt,
    long long bs, struct cols *c)
{
    size_t i;
    int majw = 0, anydev = 0;
    char buf[64];

    memset(c, 0, sizeof(*c));
    for (i = 0; i < l->n; i++) {
        const struct entry *e = &l->v[i];
        const struct stat *st = &e->st;

        if (opt->inode)
            c->iw = max_int(c->iw, ndigits(st->st_ino));
        if (opt->blocks) {
            size_text(e, opt, bs, buf, sizeof(buf));
            c->sw = max_int(c->sw, (int)strlen(buf));
        }
        if (!opt->longfmt)
            continue;
        c->lw = max_int(c->lw, ndigits(st->st_nlink));
        user_name(st->st_uid, opt->numeric, buf, sizeof(buf));
        c->uw = max_int(c->uw, (int)strlen(buf));
        group_name(st->st_gid, opt->numeric, buf, sizeof(buf));
        c->gw = max_int(c->gw, (int)strlen(buf));
        if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode)) {
            anydev = 1;
            majw = max_int(majw, ndigits(major(st->st_rdev)));
            c->minw = max_int(c->minw, ndigits(minor(st->st_rdev)));
        } else {
            byte_text(st, opt, buf, sizeof(buf));
            c->zw = max_int(c->zw, (int)strlen(buf));
        }
    }
    /* A "major, minor" pair must fit in the size column. */
    if (anydev && c->zw < majw + 2 + c->minw)
        c->zw = majw + 2 + c->minw;
}

/* Long-format fields between the -s column and the file name. */
static void print_long_part(const struct entry *e, const struct options *opt,
    const struct cols *c)
{
    const struct stat *st = &e->st;
    struct timespec ts = entry_time(e, opt);
    char mode[16], user[64], group[64], date[32], size[32];

    format_mode(st->st_mode, mode);
    user_name(st->st_uid, opt->numeric, user, sizeof(user));
    group_name(st->st_gid, opt->numeric, group, sizeof(group));
    format_date(ts.tv_sec, date, sizeof(date));
    printf("%s  %*lu %-*s  %-*s  ", mode, c->lw,
        (unsigned long)st->st_nlink, c->uw, user, c->gw, group);

    if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode)) {
        printf("%*lu, %*lu ", c->zw - 2 - c->minw,
            (unsigned long)major(st->st_rdev), c->minw,
            (unsigned long)minor(st->st_rdev));
    } else {
        byte_text(st, opt, size, sizeof(size));
        printf("%*s ", c->zw, size);
    }
    printf("%s ", date);
}

/* For a symbolic link print " -> target". */
static void print_link_target(const struct entry *e,
    const struct options *opt)
{
    char target[PATH_MAX + 1];
    ssize_t n = readlink(e->path, target, PATH_MAX);

    if (n < 0) {
        report_error(e->path);
        return;
    }
    target[n] = '\0';
    fputs(" -> ", stdout);
    print_name(target, opt);
}

void print_entries(const struct entlist *l, const struct options *opt)
{
    size_t i;
    long long bs = block_size(opt);
    struct cols c;
    char buf[64];

    widths(l, opt, bs, &c);
    for (i = 0; i < l->n; i++) {
        const struct entry *e = &l->v[i];

        if (opt->inode)
            printf("%*llu ", c.iw, (unsigned long long)e->st.st_ino);
        if (opt->blocks) {
            size_text(e, opt, bs, buf, sizeof(buf));
            printf("%*s ", c.sw, buf);
        }
        if (opt->longfmt)
            print_long_part(e, opt, &c);
        print_name(e->name, opt);
        if (opt->classify) {
            int ch = classify_char(&e->st);

            if (ch != 0)
                putchar(ch);
        }
        if (opt->longfmt && S_ISLNK(e->st.st_mode))
            print_link_target(e, opt);
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
    /* -s: total only on a terminal; -l: always. */
    if (l->n > 0 && (opt->longfmt || (opt->blocks && isatty(STDOUT_FILENO))))
        print_total(l, opt);
    print_entries(l, opt);
}
