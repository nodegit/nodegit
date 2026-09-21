#include <napi.h>
#include <string>
#include <cstring>

#include "../include/str_array_converter.h"
#include "git2/strarray.h"

git_strarray *StrArrayConverter::Convert(Napi::Value val) {
  if (!val.ToBoolean().Value()) {
    return NULL;
  }
  else if (val.IsArray()) {
    return ConvertArray(val.As<Napi::Array>());
  }
  else if (val.IsString()) {
    return ConvertString(val.As<Napi::String>());
  }
  else {
    return NULL;
  }
}

git_strarray * StrArrayConverter::AllocStrArray(const size_t count) {
  const size_t size = sizeof(git_strarray) + (sizeof(char*) * count);
  uint8_t* memory = reinterpret_cast<uint8_t*>(malloc(size));
  git_strarray *result = reinterpret_cast<git_strarray *>(memory);
  result->count = count;
  result->strings = reinterpret_cast<char**>(memory + sizeof(git_strarray));
  return result;
}

git_strarray *StrArrayConverter::ConvertArray(Napi::Array val) {
  git_strarray *result = AllocStrArray(val.Length());

  for(size_t i = 0; i < result->count; i++) {
    std::string entry = val.Get(i).As<Napi::String>().Utf8Value();
    result->strings[i] = strdup(entry.c_str());
  }

  return result;
}

git_strarray* StrArrayConverter::ConvertString(Napi::String val) {
  char *strings[1];
  std::string utf8String = val.Utf8Value();

  strings[0] = const_cast<char *>(utf8String.c_str());

  return ConstructStrArray(1, strings);
}

git_strarray *StrArrayConverter::ConstructStrArray(int argc, char** argv) {
  git_strarray *result = AllocStrArray(argc);

  for(size_t i = 0; i < result->count; i++) {
    result->strings[i] = strdup(argv[i]);
  }

  return result;
}

void StrArrayConverter::ConvertInto(git_strarray *out, Napi::Array val) {
  out->count = val.Length();
  out->strings = (char**) malloc(out->count * sizeof(char*));
  for (uint32_t i = 0; i < out->count; ++i) {
    std::string utf8String = val.Get(i).As<Napi::String>().Utf8Value();
    out->strings[i] = strdup(utf8String.c_str());
  }
}

void StrArrayConverter::ConvertInto(git_strarray *out, Napi::String val) {
  std::string utf8String = val.Utf8Value();
  out->count = 1;
  out->strings = (char**) malloc(out->count * sizeof(char*));
  out->strings[0] = strdup(utf8String.c_str());
}
