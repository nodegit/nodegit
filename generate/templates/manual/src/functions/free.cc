#include <cstring>

#include "git2.h"

void git_remote_head_free(git_remote_head *remote_head) {
  free(remote_head->name);
  free(remote_head->symref_target);
  free(remote_head);
}

void git_diff_line_free(const git_diff_line *diff_line) {
  free((void*)(diff_line->content));
  free((void*) diff_line);
}

void git_index_entry_free(git_index_entry *index_entry) {
  free((void*)(index_entry->path));
  free((void*) index_entry);
}
