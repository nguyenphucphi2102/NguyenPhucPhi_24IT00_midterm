#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include "ls.h"
#include "options.h"
#include "entry.h"
#include "sort.h"
#include "print.h"
#include "util.h"

static int printed;   /* has anything been written to stdout yet? */

/*
 * List one directory and, with -R, every subdirectory below it.
 * Symbolic links to directories are not followed.
 */
static void list_dir(const char *path, const struct options *opt, int header)
{
    struct entlist list;
    size_t i;

    if (printed)
        putchar('\n');
    if (header)
        printf("%s:\n", path);
    printed = 1;

    entlist_init(&list);
    if (read_dir(path, opt, &list) == -1) {
        fflush(stdout);
        report_error(path);
        return;
    }
    sort_entries(&list, opt);
    print_dir(&list, opt);

    if (opt->recursive) {
        for (i = 0; i < list.n; i++) {
            const struct entry *e = &list.v[i];

            if (!S_ISDIR(e->st.st_mode))
                continue;
            if (strcmp(e->name, ".") == 0 || strcmp(e->name, "..") == 0)
                continue;
            list_dir(e->path, opt, 1);
        }
    }
    entlist_free(&list);
}

int main(int argc, char *argv[])
{
    struct options opt;
    struct entlist files, dirs;
    char *dot[] = { ".", NULL };
    char **ops;
    int nops, first, i, follow;

    first = parse_options(argc, argv, &opt);
    ops = argv + first;
    nops = argc - first;
    if (nops == 0) {
        ops = dot;
        nops = 1;
    }

    /* Symlink operands are not indirected through with -d (and, like the
     * real ls, with -l / -n / -F). */
    follow = !(opt.no_recurse || opt.longfmt || opt.classify);

    entlist_init(&files);
    entlist_init(&dirs);

    /* Split operands into non-directories and directories. */
    for (i = 0; i < nops; i++) {
        struct stat st;

        if (entry_stat(ops[i], follow, &st) == -1) {
            report_error(ops[i]);
            continue;
        }
        if (S_ISDIR(st.st_mode) && !opt.no_recurse)
            entlist_push(&dirs, ops[i], ops[i], &st);
        else
            entlist_push(&files, ops[i], ops[i], &st);
    }

    /* Non-directories first, then directories; each group sorted. */
    sort_entries(&files, &opt);
    sort_entries(&dirs, &opt);

    if (files.n > 0) {
        print_entries(&files, &opt);
        printed = 1;
    }
    for (i = 0; i < (int)dirs.n; i++)
        list_dir(dirs.v[i].path, &opt, nops > 1);

    entlist_free(&files);
    entlist_free(&dirs);
    return exit_status;
}
