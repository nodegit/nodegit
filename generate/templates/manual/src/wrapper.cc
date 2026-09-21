/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <napi.h>
#include <node.h>
#include <string>
#include <cstring>

#include "../include/wrapper.h"
#include "node_buffer.h"

using namespace Napi;

Napi::FunctionReference Wrapper::constructor;

Wrapper::Wrapper(const Napi::CallbackInfo &info)
  : Napi::ObjectWrap<Wrapper>(info) {
  if (info.Length() > 0 && info[0].IsExternal()) {
    raw = info[0].As<Napi::External<void>>().Data();
  } else {
    raw = nullptr;
  }
}

void Wrapper::InitializeComponent(Napi::Env env, Napi::Object exports, nodegit::Context *nodegitContext) {
  Napi::HandleScope scope(env);

  Napi::External<nodegit::Context> nodegitExternal = Napi::External<nodegit::Context>::New(env, nodegitContext);

  Napi::Function func = DefineClass(env, "Wrapper",
    {
      InstanceMethod("toBuffer", &Wrapper::ToBuffer, napi_default, nodegitExternal),
    });

  constructor = Napi::Persistent(func);
  nodegitContext->SaveToPersistent("Wrapper::Template", func);
  exports.Set("Wrapper", func);
}

Napi::Object Wrapper::New(Napi::Env env, const void *raw) {
  return constructor.New({ Napi::External<void>::New(env, (void *)raw) });
}

void *Wrapper::GetValue() {
  return this->raw;
}

Napi::Value Wrapper::ToBuffer(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() == 0 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Number is required.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  int len = info[0].As<Number>().Int32Value();

  Napi::Object nodeBuffer = Napi::Buffer<char>::New(env, len);
  std::memcpy(nodeBuffer.As<Napi::Buffer<char>>().Data(), raw, len);

  return nodeBuffer;
}
