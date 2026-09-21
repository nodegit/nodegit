#ifndef PROMISE_COMPLETION
#define PROMISE_COMPLETION

#include <napi.h>

#include "async_baton.h"
#include "context.h"

// PromiseCompletion forwards either the resolved result or the rejection reason
// to the native layer, once the promise completes
//
// inherits ObjectWrap so it can be used in v8 and managed by the garbage collector
// it isn't wired up to be instantiated or accessed from the JS layer other than
// for the purpose of promise result forwarding
class PromiseCompletion : public Napi::ObjectWrap<PromiseCompletion>
{
  // callback type called when a promise completes
  typedef void (*Callback) (bool isFulfilled, nodegit::AsyncBaton *baton, Napi::Value resultOfPromise);

  static Napi::FunctionReference constructor;

  static Napi::Value New(const Napi::CallbackInfo& info);
  static Napi::Value PromiseFulfilled(const Napi::CallbackInfo& info);
  static Napi::Value PromiseRejected(const Napi::CallbackInfo& info);

  static Napi::Value Bind(Napi::Function method, Napi::Object object);
  static void CallCallback(bool isFulfilled, const Napi::CallbackInfo& info);

  // callback and baton stored for the promise that this PromiseCompletion is
  // attached to.  when the promise completes, the callback will be called with
  // the result, and the stored baton.
  Callback callback;
  nodegit::AsyncBaton *baton;

  void Setup(Napi::Function thenFn, Napi::Value result, nodegit::AsyncBaton *baton, Callback callback);
public:
  PromiseCompletion(const Napi::CallbackInfo& info);

  // If result is a promise, this will instantiate a new PromiseCompletion
  // and have it forward the promise result / reason via the baton and callback
  static bool ForwardIfPromise(Napi::Value result, nodegit::AsyncBaton *baton, Callback callback);

  static void InitializeComponent(Napi::Env env, nodegit::Context *nodegitContext);
};

#endif
