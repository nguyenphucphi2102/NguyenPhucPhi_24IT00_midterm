#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#define PROG "ls"

/* Set to 1 whenever an error is reported; used as the exit status. */
extern int exit_status;

void *xmalloc(size_t n);
void *xrealloc(void *p, size_t n);
char *xstrdup(const char *s);

/* Build "dir/name" (no doubled slash). Caller must free the result. */
char *join_path(const char *dir, const char *name);

/* Print "ls: path: <strerror(errno)>" to stderr and set exit_status. */
void report_error(const char *path);

#endif
