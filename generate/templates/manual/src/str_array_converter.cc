#include <nan.h>
#include <node.h>
#include <string>
#include <cstring>

#include "../include/str_array_converter.h"
#include "git2/strarray.h"

using namespace v8;
using namespace node;

git_strarray *StrArrayConverter::Convert(v8::Local<v8::Value> val) {
  if (!Nan::To<bool>(val).FromJust()) {
    return NULL;
  }
  else if (val->IsArray()) {
    return ConvertArray(v8::Local<v8::Array>::Cast(val));
  }
  else if (val->IsString() || val->IsStringObject()) {
    return ConvertString(Nan::To<v8::String>(val).ToLocalChecked());
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

git_strarray *StrArrayConverter::ConvertArray(v8::Local<v8::Array> val) {
  git_strarray *result = AllocStrArray(val->Length());

  for(size_t i = 0; i < result->count; i++) {
    Nan::Utf8String entry(Nan::Get(val, i).ToLocalChecked());
    result->strings[i] = strdup(*entry);
  }

  return result;
}

git_strarray* StrArrayConverter::ConvertString(v8::Local<v8::String> val) {
  char *strings[1];
  Nan::Utf8String utf8String(val);

  strings[0] = *utf8String;

  return ConstructStrArray(1, strings);
}

git_strarray *StrArrayConverter::ConstructStrArray(int argc, char** argv) {
  git_strarray *result = AllocStrArray(argc);

  for(size_t i = 0; i < result->count; i++) {
    result->strings[i] = strdup(argv[i]);
  }

  return result;
}

void StrArrayConverter::ConvertInto(git_strarray *out, v8::Local<v8::Array> val) {
  out->count = val->Length();
  out->strings = new char *[out->count];
  for (uint32_t i = 0; i < out->count; ++i) {
    Nan::Utf8String utf8String(Nan::Get(val, i).ToLocalChecked().As<v8::String>());
    out->strings[i] = strdup(*utf8String);
  }
}

void StrArrayConverter::ConvertInto(git_strarray *out, v8::Local<v8::String> val) {
  Nan::Utf8String utf8String(val);
  out->count = 1;
  out->strings = new char *[1];
  out->strings[0] = strdup(*utf8String);
}
