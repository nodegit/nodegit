#include <v8.h>

#include <string.h>

#include "git2.h"

#ifndef COPY_FUNCTIONS
#define COPY_FUNCTIONS

const git_error *git_error_dup(const git_error *arg);
const git_oid *git_oid_dup(const git_oid *arg);
const git_index_entry *git_index_entry_dup(const git_index_entry *arg);
const git_index_time *git_index_time_dup(const git_index_time *arg);
const git_time *git_time_dup(const git_time *arg);
const git_diff_delta *git_diff_delta_dup(const git_diff_delta *arg);
const git_diff_file *git_diff_file_dup(const git_diff_file *arg);
const git_diff_range *git_diff_range_dup(const git_diff_range *arg);

#endif
