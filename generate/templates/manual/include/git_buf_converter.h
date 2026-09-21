#ifndef STR_ARRAY_H
#define STR_ARRAY_H

#include <napi.h>

#include "git2/buffer.h"

class GitBufConverter {
  public:
    static git_buf *Convert(Napi::Value val);
};

#endif
