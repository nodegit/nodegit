#ifndef STR_ARRAY_H
#define STR_ARRAY_H

#include <napi.h>

#include "git2/strarray.h"

class StrArrayConverter {
  public:

    static git_strarray *Convert (Napi::Value val);
    static void ConvertInto(git_strarray *out, Napi::Array val);
    static void ConvertInto(git_strarray *out, Napi::String val);

  private:
    static git_strarray *ConvertArray(Napi::Array val);
    static git_strarray *ConvertString(Napi::String val);
    static git_strarray *AllocStrArray(const size_t count);
    static git_strarray *ConstructStrArray(int argc, char** argv);
};

#endif
