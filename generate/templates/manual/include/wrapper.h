/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef WRAPPER_H
#define WRAPPER_H

#include <napi.h>

#include "context.h"

class Wrapper : public Napi::ObjectWrap<Wrapper> {
  public:
    static Napi::FunctionReference constructor;

    static void InitializeComponent(Napi::Env env, Napi::Object exports, nodegit::Context *nodegitContext);

    // Factory used by the generated classes (and the base Wrapper) to wrap a raw
    // libgit2 pointer in a JS object. Mirrors the old `Wrapper::New(raw)`.
    static Napi::Object New(Napi::Env env, const void *raw);

    void *GetValue();

    // Constructed via `constructor.New({ External(raw) })`; extracts the raw pointer.
    // Must be public: Napi::ObjectWrap's ConstructorCallbackWrapper news the type.
    Wrapper(const Napi::CallbackInfo &info);

  private:
    Napi::Value ToBuffer(const Napi::CallbackInfo &info);

    void *raw;
};

#endif
