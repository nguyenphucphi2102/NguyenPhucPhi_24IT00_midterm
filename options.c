#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "options.h"

/* Print usage to stderr and exit with an error status. */
static void usage(void)
{
    fprintf(stderr, "usage: ls [-AacdFfhiklnqRrSstuw] [file ...]\n");
    exit(2);
}

/*
 * Parse command-line options into *opt.
 * For options that override each other (-c/-u, -l/-n, -q/-w, -R/-d,
 * -h/-k) the last one given on the command line wins.
 * Returns the index of the first operand in argv.
 */
int parse_options(int argc, char *argv[], struct options *opt)
{
    int ch;
    int q_mode = -1;   /* -1 = neither -q nor -w, 1 = -q, 0 = -w */

    memset(opt, 0, sizeof(*opt));

    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
        case 'A': opt->almost_all = 1; break;
        case 'a': opt->all = 1; break;
        case 'c': opt->use_ctime = 1; opt->use_atime = 0; break;
        case 'u': opt->use_atime = 1; opt->use_ctime = 0; break;
        case 'd': opt->no_recurse = 1; opt->recursive = 0; break;
        case 'R': opt->recursive = 1; opt->no_recurse = 0; break;
        case 'h': opt->human = 1; opt->kilobytes = 0; break;
        case 'k': opt->kilobytes = 1; opt->human = 0; break;
        case 'l': opt->longfmt = 1; opt->numeric = 0; break;
        case 'n': opt->longfmt = 1; opt->numeric = 1; break;
        case 'q': q_mode = 1; break;
        case 'w': q_mode = 0; break;
        case 'F': opt->classify = 1; break;
        case 'f': opt->no_sort = 1; opt->all = 1; break;
        case 'i': opt->inode = 1; break;
        case 'r': opt->reverse = 1; break;
        case 'S': opt->sort_size = 1; opt->sort_time = 0; break;
        case 's': opt->blocks = 1; break;
        case 't': opt->sort_time = 1; opt->sort_size = 0; break;
        default:  usage();
        }
    }

    /* -A is always set for the super-user. */
    if (geteuid() == 0)
        opt->almost_all = 1;

    /* Default: print '?' for non-printable chars only on a terminal. */
    if (q_mode == -1)
        opt->nonprint_q = isatty(STDOUT_FILENO);
    else
        opt->nonprint_q = q_mode;

    return optind;
}
