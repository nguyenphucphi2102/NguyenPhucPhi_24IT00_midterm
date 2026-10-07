#ifndef SORT_H
#define SORT_H

#include "entry.h"
#include "ls.h"

/* Sort the list according to the options. */
void sort_entries(struct entlist *l, const struct options *opt);

#endif
