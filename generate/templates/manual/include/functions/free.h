#include <v8.h>

#include <string.h>

#include "git2.h"

#ifndef NODEGIT_FREE_FUNCTIONS
#define NODEGIT_FREE_FUNCTIONS

void git_remote_head_free(git_remote_head *remote_head);

#endif
