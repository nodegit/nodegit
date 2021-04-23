#ifndef NODEGIT_V8_HELPERS_H
#define NODEGIT_V8_HELPERS_H

#include <nan.h>

namespace nodegit {
  v8::Local<v8::Value> safeGetField(v8::Local<v8::Object> &containerObj, std::string fieldName);
}

#endif
