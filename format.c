#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "format.h"

#define MIN_BSIZE 512LL
#define MAX_BSIZE (1024LL * 1024 * 1024)

struct timespec entry_time(const struct entry *e, const struct options *opt)
{
    if (opt->use_ctime)
        return e->st.st_ctimespec;
    if (opt->use_atime)
        return e->st.st_atimespec;
    return e->st.st_mtimespec;
}

/*
 * Block size in bytes. -k forces 1024; otherwise BLOCKSIZE is parsed
 * as a number with an optional unit (b, k, m, g); anything invalid
 * falls back to 512. The result is clamped to [512, 1G].
 */
long long block_size(const struct options *opt)
{
    const char *s;
    char *end;
    long long n;

    if (opt->kilobytes)
        return 1024;
    s = getenv("BLOCKSIZE");
    if (s == NULL || *s == '\0')
        return MIN_BSIZE;
    n = strtoll(s, &end, 10);
    if (end == s || n <= 0)
        return MIN_BSIZE;
    if (n > MAX_BSIZE)          /* avoid overflow when multiplying */
        n = MAX_BSIZE;
    switch (*end) {
    case '\0':
        break;
    case 'b': case 'B':
        n *= 512; end++; break;
    case 'k': case 'K':
        n *= 1024; end++; break;
    case 'm': case 'M':
        n *= 1024LL * 1024; end++; break;
    case 'g': case 'G':
        n *= 1024LL * 1024 * 1024; end++; break;
    default:
        return MIN_BSIZE;
    }
    if (*end != '\0')
        return MIN_BSIZE;
    if (n < MIN_BSIZE)
        n = MIN_BSIZE;
    if (n > MAX_BSIZE)
        n = MAX_BSIZE;
    return n;
}

unsigned long long blocks_in(const struct stat *st, long long bs)
{
    unsigned long long bytes = (unsigned long long)st->st_blocks * 512ULL;

    return (bytes + (unsigned long long)bs - 1) / (unsigned long long)bs;
}

/*
 * Human-readable size, at most 4 characters: 0B, 512B, 5.0K, 36K, 2.0M.
 * Values are scaled by 1024 until they fit in 3 digits; below 10 one
 * decimal is shown, otherwise the value is rounded to an integer.
 */
void humanize(unsigned long long bytes, char *buf, size_t len)
{
    static const char units[] = "BKMGTPE";
    double v = (double)bytes;
    int u = 0;
    unsigned long long t;

    while (v >= 999.5 && u < 6) {
        v /= 1024.0;
        u++;
    }
    if (u == 0) {
        snprintf(buf, len, "%lluB", bytes);
    } else if (v < 9.95) {
        t = (unsigned long long)(v * 10.0 + 0.5);   /* tenths */
        snprintf(buf, len, "%llu.%llu%c", t / 10, t % 10, units[u]);
    } else {
        t = (unsigned long long)(v + 0.5);
        snprintf(buf, len, "%llu%c", t, units[u]);
    }
}

int classify_char(const struct stat *st)
{
    mode_t m = st->st_mode;

    if (S_ISDIR(m))
        return '/';
    if (S_ISLNK(m))
        return '@';
    if (S_ISSOCK(m))
        return '=';
    if (S_ISFIFO(m))
        return '|';
#ifdef S_ISWHT
    if (S_ISWHT(m))
        return '%';
#endif
    if (m & (S_IXUSR | S_IXGRP | S_IXOTH))
        return '*';
    return 0;
}
