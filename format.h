#ifndef FORMAT_H
#define FORMAT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "entry.h"
#include "ls.h"

/* Time selected by -c / -u for an entry (default: modification time). */
struct timespec entry_time(const struct entry *e, const struct options *opt);

/* Block size for -s and "total": 1024 with -k, else $BLOCKSIZE, else 512. */
long long block_size(const struct options *opt);

/* st_blocks (512-byte units) converted to units of bs, rounded up. */
unsigned long long blocks_in(const struct stat *st, long long bs);

/* Format a byte count like 512B, 5.0K, 36K, 2.0M (for -h). */
void humanize(unsigned long long bytes, char *buf, size_t len);

/* Character appended by -F, or 0 if none. */
int classify_char(const struct stat *st);

#endif
