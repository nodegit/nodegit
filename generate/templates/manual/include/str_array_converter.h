#ifndef STR_ARRAY_H
#define STR_ARRAY_H

#include <v8.h>

#include "nan.h"
#include "git2/strarray.h"

using namespace v8;

class StrArrayConverter {
  public:

    static git_strarray *Convert (v8::Local<v8::Value> val);

  private:
    static git_strarray *ConvertArray(Array *val);
    static git_strarray *ConvertString(v8::Local<String> val);
    static git_strarray *AllocStrArray(const size_t count);
    static git_strarray *ConstructStrArray(int argc, char** argv);
};

#endif
