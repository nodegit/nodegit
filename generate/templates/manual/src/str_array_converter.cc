#include <nan.h>
#include <node.h>
#include <string>
#include <cstring>

#include "../include/str_array_converter.h"
#include "git2/strarray.h"

using namespace v8;
using namespace node;

git_strarray *StrArrayConverter::Convert(Handle<v8::Value> val) {
  if (!val->BooleanValue()) {
    return NULL;
  }
  else if (val->IsArray()) {
    return ConvertArray(Array::Cast(*val));
  }
  else if (val->IsString() || val->IsStringObject()) {
    return ConvertString(val->ToString());
  }
  else {
    return NULL;
  }
}

git_strarray *StrArrayConverter::ConvertArray(Array *val) {
  git_strarray *result = (git_strarray *)malloc(sizeof(git_strarray*));
  result->count = val->Length();
  result->strings = (char **)malloc(sizeof(char*) * result->count);

  for(size_t i = 0; i < result->count; i++) {
    NanUtf8String entry(val->Get(i));
    result->strings[i] = strdup(*entry);
  }

  return result;
}

git_strarray* StrArrayConverter::ConvertString(Handle<String> val) {
  char *strings[1];
  NanUtf8String utf8String(val);

  strings[0] = *utf8String;

  return ConstructStrArray(1, strings);
}

git_strarray *StrArrayConverter::ConstructStrArray(int argc, char** argv) {
  git_strarray *result = (git_strarray *)malloc(sizeof(git_strarray*));
  result->count = argc;
  result->strings = (char **)malloc(sizeof(char*) * result->count);

  for(size_t i = 0; i < result->count; i++) {
    result->strings[i] = strdup(argv[i]);
  }

  return result;
}
