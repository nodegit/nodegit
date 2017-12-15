#ifndef STR_ARRAY_H
#define STR_ARRAY_H

#include <v8.h>

#include "nan.h"
#include "git2/buffer.h"

using namespace v8;

class GitBufConverter {
  public:
    static git_buf *Convert(v8::Local<v8::Value> val);
};

#endif
