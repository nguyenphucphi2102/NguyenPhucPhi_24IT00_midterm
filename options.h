#ifndef OPTIONS_H
#define OPTIONS_H

#include "ls.h"

/* Parse argv into *opt. Returns the index of the first operand. */
int parse_options(int argc, char *argv[], struct options *opt);

#endif
