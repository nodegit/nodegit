#include <string>
#include <cstring>

#include "git2.h"

const git_error *git_error_dup(const git_error *arg) {
  git_error *result = (git_error *)malloc(sizeof(git_error));
  result->klass = arg->klass;
  result->message = strdup(arg->message);
  return result;
}
