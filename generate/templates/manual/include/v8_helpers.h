#ifndef NODEGIT_V8_HELPERS_H
#define NODEGIT_V8_HELPERS_H

#include <napi.h>
#include <string>
#include <type_traits>

namespace nodegit {
  Napi::Value safeGetField(Napi::Object containerObj, std::string fieldName);
  inline bool IsError(Napi::Env env, Napi::Value value) {
    if (value.IsEmpty() || !value.IsObject()) return false;
    bool isErr = false;
    napi_is_error(env, value, &isErr);
    return isErr;
  }

  inline Napi::Value CallJSFunction(Napi::Env env, Napi::FunctionReference *callback, napi_value recv, const std::vector<napi_value>& args) {
    napi_value resultVal = nullptr;
    napi_status status = napi_call_function(env, recv, callback->Value(), args.size(), args.data(), &resultVal);
    if (status != napi_ok || !resultVal) {
      return env.Undefined();
    }
    return Napi::Value(env, resultVal);
  }
  inline Napi::Value ToV8(Napi::Env env, int value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, unsigned int value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, int64_t value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, uint64_t value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, double value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, float value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, bool value) { return Napi::Boolean::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, const char *value) { return value ? Napi::String::New(env, value) : env.Null(); }
  inline Napi::Value ToV8(Napi::Env env, const std::string &value) { return Napi::String::New(env, value); }
}

#endif
