#ifndef PRINT_H
#define PRINT_H

#include "entry.h"
#include "ls.h"

/* Print every entry of the list, one per line. */
void print_entries(const struct entlist *l, const struct options *opt);

/* Print a directory listing: optional "total" line, then the entries. */
void print_dir(const struct entlist *l, const struct options *opt);

#endif
