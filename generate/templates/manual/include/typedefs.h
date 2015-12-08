#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include "git2/submodule.h"

typedef int (*git_submodule_cb)(git_submodule *sm, const char *name, void *payload);

#endif
