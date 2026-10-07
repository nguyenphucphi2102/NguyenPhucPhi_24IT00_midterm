#include "format.h"

struct timespec entry_time(const struct entry *e, const struct options *opt)
{
    if (opt->use_ctime)
        return e->st.st_ctimespec;
    if (opt->use_atime)
        return e->st.st_atimespec;
    return e->st.st_mtimespec;
}
