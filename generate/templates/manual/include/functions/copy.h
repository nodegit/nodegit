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
git_remote_head *git_remote_head_dup(const git_remote_head *src);


void git_time_dup(git_time **out, const git_time *arg);
void git_transfer_progress_dup(git_transfer_progress **out, const git_transfer_progress *arg);

#endif
