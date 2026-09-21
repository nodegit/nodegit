#include <iostream>
#include "../include/promise_completion.h"

Napi::FunctionReference PromiseCompletion::constructor;

// initializes the persistent handles for Napi methods
void PromiseCompletion::InitializeComponent(Napi::Env env, nodegit::Context *nodegitContext) {
  Napi::HandleScope scope(env);

  Napi::Function constructorFunc = DefineClass(env, "PromiseCompletion", {});
  constructor = Napi::Persistent(constructorFunc);
  nodegitContext->SaveToPersistent("PromiseCompletion::Template", constructorFunc);

  Napi::Function promiseFulfilled = Napi::Function::New(env, PromiseFulfilled, "PromiseFulfilled");
  nodegitContext->SaveToPersistent("PromiseCompletion::PromiseFulfilled", promiseFulfilled);

  Napi::Function promiseRejected = Napi::Function::New(env, PromiseRejected, "PromiseRejected");
  nodegitContext->SaveToPersistent("PromiseCompletion::PromiseRejected", promiseRejected);
}

PromiseCompletion::PromiseCompletion(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<PromiseCompletion>(info) {
  callback = nullptr;
  baton = nullptr;
}

bool PromiseCompletion::ForwardIfPromise(Napi::Value result, nodegit::AsyncBaton *baton, Callback callback)
{
  if (result.IsEmpty() || !result.IsObject() || result.Env().IsExceptionPending()) {
    return false;
  }

  // check if the result is a promise
  Napi::Value thenProp = result.As<Napi::Object>().Get("then");
  if (!thenProp.IsEmpty() && !thenProp.IsUndefined() && !result.Env().IsExceptionPending() && thenProp.IsFunction()) {
    // we can be reasonably certain that the result is a promise

    // create a new instance of PromiseCompletion
    nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
    Napi::Object object = constructor.New({});
    PromiseCompletion *promiseCompletion = PromiseCompletion::Unwrap(object);
    promiseCompletion->Setup(thenProp.As<Napi::Function>(), result, baton, callback);

    return true;
  }

  return false;
}

// creates a new instance of PromiseCompletion, wrapped in a v8 object
Napi::Value PromiseCompletion::New(const Napi::CallbackInfo& info) {
  return info.This().As<Napi::Object>();
}

// sets up a Promise to forward the promise result via the baton and callback
void PromiseCompletion::Setup(Napi::Function thenFn, Napi::Value result, nodegit::AsyncBaton *baton, Callback callback) {
  this->callback = callback;
  this->baton = baton;

  Napi::Object promise = result.As<Napi::Object>();

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  Napi::Object thisHandle = this->Value();
  Napi::Function promiseFulfilled = nodegitContext->GetFromPersistent("PromiseCompletion::PromiseFulfilled").As<Napi::Function>();
  Napi::Function promiseRejected = nodegitContext->GetFromPersistent("PromiseCompletion::PromiseRejected").As<Napi::Function>();

  Napi::Value argv[2] = {
    Bind(promiseFulfilled, thisHandle),
    Bind(promiseRejected, thisHandle)
  };

  // call the promise's .then method with resolve and reject callbacks
  thenFn.Call(promise, { argv[0], argv[1] });
}

// binds an object to be the context of the function.
// there might be a better way to do this than calling Function.bind...
Napi::Value PromiseCompletion::Bind(Napi::Function function, Napi::Object object) {
  Napi::Function bind = function.Get("bind").As<Napi::Function>();
  return bind.Call(function, { object });
}

// calls the callback stored in the PromiseCompletion, passing the baton that
// was provided in construction
void PromiseCompletion::CallCallback(bool isFulfilled, const Napi::CallbackInfo& info) {
  Napi::Value resultOfPromise;

  if (info.Length() > 0) {
    resultOfPromise = info[0];
  }

  PromiseCompletion *promiseCompletion = PromiseCompletion::Unwrap(info.This().As<Napi::Object>());

  (*promiseCompletion->callback)(isFulfilled, promiseCompletion->baton, resultOfPromise);
}

Napi::Value PromiseCompletion::PromiseFulfilled(const Napi::CallbackInfo& info) {
  CallCallback(true, info);
  return info.Env().Undefined();
}

Napi::Value PromiseCompletion::PromiseRejected(const Napi::CallbackInfo& info) {
  CallCallback(false, info);
  return info.Env().Undefined();
}
