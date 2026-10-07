#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "entry.h"
#include "util.h"

void entlist_init(struct entlist *l)
{
    l->v = NULL;
    l->n = 0;
    l->cap = 0;
}

void entlist_free(struct entlist *l)
{
    size_t i;

    for (i = 0; i < l->n; i++) {
        free(l->v[i].name);
        free(l->v[i].path);
    }
    free(l->v);
    entlist_init(l);
}

void entlist_push(struct entlist *l, const char *name, const char *path,
    const struct stat *st)
{
    struct entry *e;

    if (l->n == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 16;
        l->v = xrealloc(l->v, l->cap * sizeof(*l->v));
    }
    e = &l->v[l->n++];
    e->name = xstrdup(name);
    e->path = xstrdup(path);
    e->st = *st;
}

int entry_stat(const char *path, int follow, struct stat *st)
{
    /* A dangling symlink fails stat(); fall back to lstat() for it. */
    if (follow && stat(path, st) == 0)
        return 0;
    return lstat(path, st);
}

int read_dir(const char *dir, const struct options *opt, struct entlist *out)
{
    DIR *dp;
    struct dirent *de;

    dp = opendir(dir);
    if (dp == NULL)
        return -1;

    while ((de = readdir(dp)) != NULL) {
        const char *nm = de->d_name;
        struct stat st;
        char *path;

        if (nm[0] == '.') {
            /* Hidden entries need -a or -A. */
            if (!(opt->all || opt->almost_all))
                continue;
            /* "." and ".." only appear with -a. */
            if (!opt->all && (strcmp(nm, ".") == 0 || strcmp(nm, "..") == 0))
                continue;
        }

        path = join_path(dir, nm);
        if (entry_stat(path, 0, &st) == -1) {
            report_error(path);
            free(path);
            continue;
        }
        entlist_push(out, nm, path, &st);
        free(path);
    }
    closedir(dp);
    return 0;
}
