#ifndef FORMAT_H
#define FORMAT_H

#include <time.h>
#include "entry.h"
#include "ls.h"

/* Time selected by -c / -u for an entry (default: modification time). */
struct timespec entry_time(const struct entry *e, const struct options *opt);

#endif
