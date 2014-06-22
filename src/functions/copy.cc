#include <string>
#include <cstring>

#include "git2.h"

const git_error *git_error_dup(const git_error *arg) {
  git_error *result = (git_error *)malloc(sizeof(git_error));
  result->klass = arg->klass;
  result->message = strdup(arg->message);
  return result;
}

const git_oid *git_oid_dup(const git_oid *arg) {
  git_oid *result = (git_oid *)malloc(sizeof(git_oid));
  git_oid_cpy(result, arg);
  return result;
}

const git_index_entry *git_index_entry_dup(const git_index_entry *arg) {
  git_index_entry *result = (git_index_entry *)malloc(sizeof(git_index_entry));
  *result = *arg;
  return result;
}

const git_index_time *git_index_time_dup(const git_index_time *arg) {
  git_index_time *result = (git_index_time *)malloc(sizeof(git_index_time));
  *result = (const git_index_time) *arg;
  return result;
}

const git_time *git_time_dup(const git_time *arg) {
  git_time *result = (git_time *)malloc(sizeof(git_time));
  *result = *arg;
  return result;
}

const git_diff_delta *git_diff_delta_dup(const git_diff_delta *arg) {
  git_diff_delta *result = (git_diff_delta *)malloc(sizeof(git_diff_delta));
  *result = *arg;
  return result;
}

const git_diff_file *git_diff_file_dup(const git_diff_file *arg) {
  git_diff_file *result = (git_diff_file *)malloc(sizeof(git_diff_file));
  *result = *arg;
  return result;
}

const git_diff_range *git_diff_range_dup(const git_diff_range *arg) {
  git_diff_range *result = (git_diff_range *)malloc(sizeof(git_diff_range));
  *result = *arg;
  return result;
}
