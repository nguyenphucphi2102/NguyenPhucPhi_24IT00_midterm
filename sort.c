#include <stdlib.h>
#include <string.h>
#include "sort.h"

/* Lexicographical order by name. */
static int cmp_name(const void *a, const void *b)
{
    const struct entry *ea = a;
    const struct entry *eb = b;

    return strcmp(ea->name, eb->name);
}

void sort_entries(struct entlist *l, const struct options *opt)
{
    size_t i, j;

    if (opt->no_sort || l->n < 2)
        return;

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
