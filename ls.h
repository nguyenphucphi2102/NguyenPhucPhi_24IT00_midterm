#ifndef LS_H
#define LS_H

/* All command-line settings, filled in by parse_options(). */
struct options {
    int all;         /* -a : include dot files           */
    int almost_all;  /* -A : all except '.' and '..'     */
    int use_ctime;   /* -c : status-change time          */
    int use_atime;   /* -u : access time                 */
    int no_recurse;  /* -d : list directories as files   */
    int classify;    /* -F : append / * @ % = |          */
    int no_sort;     /* -f : do not sort                 */
    int human;       /* -h : human-readable sizes        */
    int kilobytes;   /* -k : sizes in kilobytes          */
    int inode;       /* -i : print inode number          */
    int longfmt;     /* -l or -n : long format           */
    int numeric;     /* -n : numeric uid/gid             */
    int recursive;   /* -R : recurse into subdirectories */
    int reverse;     /* -r : reverse sort order          */
    int sort_size;   /* -S : sort by size                */
    int sort_time;   /* -t : sort by time                */
    int blocks;      /* -s : show block counts           */
    int nonprint_q;  /* 1 = print '?', 0 = raw output    */
};

#endif
