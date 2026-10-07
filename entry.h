#ifndef ENTRY_H
#define ENTRY_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include "ls.h"

/* One file to be listed. */
struct entry {
    char *name;         /* name as displayed            */
    char *path;         /* path used for stat()         */
    struct stat st;     /* file status                  */
};

/* Growable array of entries. */
struct entlist {
    struct entry *v;
    size_t n;
    size_t cap;
};

void entlist_init(struct entlist *l);
void entlist_free(struct entlist *l);

/* Append a copy of name/path/st to the list. */
void entlist_push(struct entlist *l, const char *name, const char *path,
    const struct stat *st);

/* stat() (follow != 0) or lstat() a path. Returns 0 or -1. */
int entry_stat(const char *path, int follow, struct stat *st);

/*
 * Read the entries of directory dir into out, honouring -a / -A.
 * Returns 0 on success, -1 if the directory cannot be opened.
 */
int read_dir(const char *dir, const struct options *opt, struct entlist *out);

#endif
