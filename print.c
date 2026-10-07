#include <stdio.h>
#include "print.h"

void print_entries(const struct entlist *l, const struct options *opt)
{
    size_t i;

    (void)opt;   /* used from Step 6 on (-F, -i, -s, -l ...) */
    for (i = 0; i < l->n; i++)
        puts(l->v[i].name);
}
