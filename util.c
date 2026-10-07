#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int exit_status = 0;

void *xmalloc(size_t n)
{
    void *p = malloc(n ? n : 1);
    if (p == NULL) {
        fprintf(stderr, "%s: out of memory\n", PROG);
        exit(2);
    }
    return p;
}

void *xrealloc(void *p, size_t n)
{
    void *q = realloc(p, n ? n : 1);
    if (q == NULL) {
        fprintf(stderr, "%s: out of memory\n", PROG);
        exit(2);
    }
    return q;
}

char *xstrdup(const char *s)
{
    size_t n = strlen(s) + 1;
    char *p = xmalloc(n);
    memcpy(p, s, n);
    return p;
}

char *join_path(const char *dir, const char *name)
{
    size_t dl = strlen(dir), nl = strlen(name);
    int slash = (dl > 0 && dir[dl - 1] != '/');
    char *p = xmalloc(dl + slash + nl + 1);

    memcpy(p, dir, dl);
    if (slash)
        p[dl] = '/';
    memcpy(p + dl + slash, name, nl + 1);
    return p;
}

void report_error(const char *path)
{
    int e = errno;   /* save: fprintf may change errno */

    fprintf(stderr, "%s: %s: %s\n", PROG, path, strerror(e));
    exit_status = 1;
}
