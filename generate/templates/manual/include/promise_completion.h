#ifndef PROMISE_COMPLETION
#define PROMISE_COMPLETION

#include <nan.h>

#include "async_baton.h"

// PromiseCompletion forwards either the resolved result or the rejection reason
// to the native layer, once the promise completes
//
// inherits ObjectWrap so it can be used in v8 and managed by the garbage collector
// it isn't wired up to be instantiated or accessed from the JS layer other than
// for the purpose of promise result forwarding
class PromiseCompletion : public Nan::ObjectWrap
{
  // callback type called when a promise completes
  typedef void (*Callback) (bool isFulfilled, AsyncBaton *baton, v8::Local<v8::Value> resultOfPromise);

  static NAN_METHOD(New);
  static NAN_METHOD(PromiseFulfilled);
  static NAN_METHOD(PromiseRejected);

  // persistent handles for NAN_METHODs
  static Nan::Persistent<v8::Function> newFn;
  static Nan::Persistent<v8::Function> promiseFulfilled;
  static Nan::Persistent<v8::Function> promiseRejected;

  static v8::Local<v8::Value> Bind(Nan::Persistent<v8::Function> &method, v8::Local<v8::Object> object);
  static void CallCallback(bool isFulfilled, const Nan::FunctionCallbackInfo<v8::Value> &info);

  // callback and baton stored for the promise that this PromiseCompletion is
  // attached to.  when the promise completes, the callback will be called with
  // the result, and the stored baton.
  Callback callback;
  AsyncBaton *baton;

  void Setup(v8::Local<v8::Function> thenFn, v8::Local<v8::Value> result, AsyncBaton *baton, Callback callback);
public:
  // If result is a promise, this will instantiate a new PromiseCompletion
  // and have it forward the promise result / reason via the baton and callback
  static bool ForwardIfPromise(v8::Local<v8::Value> result, AsyncBaton *baton, Callback callback);

  static void InitializeComponent();
};

#endif
