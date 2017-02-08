#include <cstring>

#include "git2.h"

void git_remote_head_free(git_remote_head *remote_head) {
  free(remote_head->name);
  free(remote_head->symref_target);
  free(remote_head);
}
