#include <iostream>
#include "../include/promise_completion.h"

// initializes the persistent handles for NAN_METHODs
void PromiseCompletion::InitializeComponent(nodegit::Context *nodegitContext) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> nodegitExternal = Nan::New<v8::External>(nodegitContext);
  v8::Local<v8::FunctionTemplate> newTemplate = Nan::New<v8::FunctionTemplate>(New, nodegitExternal);
  newTemplate->InstanceTemplate()->SetInternalFieldCount(2);

  nodegitContext->SaveToPersistent(
    "PromiseCompletion::Template",
    Nan::GetFunction(newTemplate).ToLocalChecked()
  );

  v8::Local<v8::Value> promiseFulfilled = Nan::GetFunction(
    Nan::New<v8::FunctionTemplate>(PromiseFulfilled, nodegitExternal)
  ).ToLocalChecked();
  nodegitContext->SaveToPersistent("PromiseCompletion::PromiseFulfilled", promiseFulfilled);

  v8::Local<v8::Value> promiseRejected = Nan::GetFunction(
    Nan::New<v8::FunctionTemplate>(PromiseRejected, nodegitExternal)
  ).ToLocalChecked();
  nodegitContext->SaveToPersistent("PromiseCompletion::PromiseRejected", promiseRejected);
}

bool PromiseCompletion::ForwardIfPromise(v8::Local<v8::Value> result, nodegit::AsyncBaton *baton, Callback callback)
{
  Nan::HandleScope scope;

  // check if the result is a promise
  if (!result.IsEmpty() && result->IsObject()) {
    Nan::MaybeLocal<v8::Value> maybeThenProp = Nan::Get(Nan::To<v8::Object>(result).ToLocalChecked(), Nan::New("then").ToLocalChecked());
    if (!maybeThenProp.IsEmpty()) {
      v8::Local<v8::Value> thenProp = maybeThenProp.ToLocalChecked();
      if(thenProp->IsFunction()) {
        // we can be reasonably certain that the result is a promise

        // create a new v8 instance of PromiseCompletion
        nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
        v8::Local<v8::Function> constructor_template = nodegitContext->GetFromPersistent("PromiseCompletion::Template")
          .As<v8::Function>();
        v8::Local<v8::Object> object = Nan::NewInstance(constructor_template).ToLocalChecked();

        // set up the native PromiseCompletion object
        PromiseCompletion *promiseCompletion = ObjectWrap::Unwrap<PromiseCompletion>(object);
        promiseCompletion->Setup(thenProp.As<v8::Function>(), result, baton, callback);

        return true;
      }
    }
  }

  return false;
}

// creates a new instance of PromiseCompletion, wrapped in a v8 object
NAN_METHOD(PromiseCompletion::New) {
  PromiseCompletion *promiseCompletion = new PromiseCompletion();
  promiseCompletion->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

// sets up a Promise to forward the promise result via the baton and callback
void PromiseCompletion::Setup(v8::Local<v8::Function> thenFn, v8::Local<v8::Value> result, nodegit::AsyncBaton *baton, Callback callback) {
  this->callback = callback;
  this->baton = baton;

  v8::Local<v8::Object> promise = Nan::To<v8::Object>(result).ToLocalChecked();

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  v8::Local<v8::Object> thisHandle = handle();
  v8::Local<v8::Function> promiseFulfilled = nodegitContext->GetFromPersistent("PromiseCompletion::PromiseFulfilled")
    .As<v8::Function>();
  v8::Local<v8::Function> promiseRejected = nodegitContext->GetFromPersistent("PromiseCompletion::PromiseRejected")
    .As<v8::Function>();

  v8::Local<v8::Value> argv[2] = {
    Bind(promiseFulfilled, thisHandle),
    Bind(promiseRejected, thisHandle)
  };

  // call the promise's .then method with resolve and reject callbacks
  Nan::Call(Nan::Callback(thenFn), promise, 2, argv);
}

// binds an object to be the context of the function.
// there might be a better way to do this than calling Function.bind...
v8::Local<v8::Value> PromiseCompletion::Bind(v8::Local<v8::Function> function, v8::Local<v8::Object> object) {
  Nan::EscapableHandleScope scope;

  v8::Local<v8::Function> bind =
    Nan::Get(function, Nan::New("bind").ToLocalChecked())
      .ToLocalChecked().As<v8::Function>();

  v8::Local<v8::Value> argv[1] = { object };

  return scope.Escape(Nan::Call(bind, Nan::To<v8::Object>(function).ToLocalChecked(), 1, argv).ToLocalChecked());
}

// calls the callback stored in the PromiseCompletion, passing the baton that
// was provided in construction
void PromiseCompletion::CallCallback(bool isFulfilled, const Nan::FunctionCallbackInfo<v8::Value> &info) {
  v8::Local<v8::Value> resultOfPromise;

  if (info.Length() > 0) {
    resultOfPromise = info[0];
  }

  PromiseCompletion *promiseCompletion = ObjectWrap::Unwrap<PromiseCompletion>(Nan::To<v8::Object>(info.This()).ToLocalChecked());

  (*promiseCompletion->callback)(isFulfilled, promiseCompletion->baton, resultOfPromise);
}

NAN_METHOD(PromiseCompletion::PromiseFulfilled) {
  CallCallback(true, info);
}

NAN_METHOD(PromiseCompletion::PromiseRejected) {
  CallCallback(false, info);
}
