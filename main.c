#include <stdio.h>
#include "options.h"

int main(int argc, char *argv[])
{
    struct options opt;
    int first = parse_options(argc, argv, &opt);

    printf("first=%d a=%d A=%d l=%d n=%d c=%d u=%d R=%d d=%d h=%d k=%d q=%d\n",
        first, opt.all, opt.almost_all, opt.longfmt, opt.numeric,
        opt.use_ctime, opt.use_atime, opt.recursive, opt.no_recurse,
        opt.human, opt.kilobytes, opt.nonprint_q);
    return 0;
}
