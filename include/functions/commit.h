#include <v8.h>
#include <node.h>
#include <string>
#include <vector>

#include "cvv8/v8-convert.hpp"

#include "../../vendor/libgit2/include/git2.h"

#include "../../include/functions/string.h"

#ifndef COMMIT_FUNCTIONS
#define COMMIT_FUNCTIONS

struct GitCommitDetails {
  const git_oid *oid;
  char sha[GIT_OID_HEXSZ + 1];
  const char* message;
  time_t time;
  int timeOffset;
  const git_signature* committer;
  const git_signature* author;
  unsigned int parentCount;
  std::vector<std::string> parentShas;
};

v8::Local<v8::Object> createCommitDetailsObject(GitCommitDetails* rawDetails);

#endif
