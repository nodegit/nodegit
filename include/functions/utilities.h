#ifndef UTILITY_FUNCTIONS
#define UTILITY_FUNCTIONS

#include <v8.h>
#include <node.h>

#include "../../vendor/libgit2/include/git2.h"

#include "../../include/error.h"

using namespace v8;

bool success(const git_error* error, Persistent<Function> callback);

#endif
