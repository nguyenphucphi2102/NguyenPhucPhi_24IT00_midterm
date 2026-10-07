#include <stdlib.h>
#include <string.h>
#include "sort.h"
#include "format.h"

/* qsort() has no context argument, so the options are kept here. */
static const struct options *g_opt;

/* Lexicographical order by name. */
static int cmp_name(const void *a, const void *b)
{
    const struct entry *ea = a;
    const struct entry *eb = b;

    return strcmp(ea->name, eb->name);
}

/* Largest first; equal sizes fall back to name order. */
static int cmp_size(const void *a, const void *b)
{
    const struct entry *ea = a;
    const struct entry *eb = b;

    if (ea->st.st_size != eb->st.st_size)
        return ea->st.st_size > eb->st.st_size ? -1 : 1;
    return cmp_name(a, b);
}

/* Most recent first (time chosen by -c / -u); ties fall back to name. */
static int cmp_time(const void *a, const void *b)
{
    struct timespec ta = entry_time(a, g_opt);
    struct timespec tb = entry_time(b, g_opt);

    if (ta.tv_sec != tb.tv_sec)
        return ta.tv_sec > tb.tv_sec ? -1 : 1;
    if (ta.tv_nsec != tb.tv_nsec)
        return ta.tv_nsec > tb.tv_nsec ? -1 : 1;
    return cmp_name(a, b);
}

void sort_entries(struct entlist *l, const struct options *opt)
{
    size_t i, j;

    if (opt->no_sort || l->n < 2)
        return;

    g_opt = opt;
    if (opt->sort_size)
        qsort(l->v, l->n, sizeof(struct entry), cmp_size);
    else if (opt->sort_time)
        qsort(l->v, l->n, sizeof(struct entry), cmp_time);
    else
        qsort(l->v, l->n, sizeof(struct entry), cmp_name);

    /* -r: reverse the final order. */
    if (opt->reverse) {
        for (i = 0, j = l->n - 1; i < j; i++, j--) {
            struct entry tmp = l->v[i];
            l->v[i] = l->v[j];
            l->v[j] = tmp;
        }
    }
}
