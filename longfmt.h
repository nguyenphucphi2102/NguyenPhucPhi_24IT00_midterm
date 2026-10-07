#ifndef LONGFMT_H
#define LONGFMT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <time.h>

/* Ten-character mode string such as "drwxr-xr-x"; buf needs 11 bytes. */
void format_mode(mode_t m, char *buf);

/* Date column: "Oct  7 01:48" or, for old/future files, "Oct  7  2025". */
void format_date(time_t t, char *buf, size_t len);

/* Owner / group name of an id; the number if unknown or numeric != 0. */
void user_name(uid_t uid, int numeric, char *buf, size_t len);
void group_name(gid_t gid, int numeric, char *buf, size_t len);

#endif
