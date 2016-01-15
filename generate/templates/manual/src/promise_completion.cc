#include "../include/promise_completion.h"

Nan::Persistent<v8::Function> PromiseCompletion::newFn;
Nan::Persistent<v8::Function> PromiseCompletion::promiseFulfilled;
Nan::Persistent<v8::Function> PromiseCompletion::promiseRejected;

// initializes the persistent handles for NAN_METHODs
void PromiseCompletion::InitializeComponent() {
  v8::Local<v8::FunctionTemplate> newTemplate = Nan::New<v8::FunctionTemplate>(New);
  newTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  newFn.Reset(newTemplate->GetFunction());

  promiseFulfilled.Reset(Nan::New<v8::FunctionTemplate>(PromiseFulfilled)->GetFunction());
  promiseRejected.Reset(Nan::New<v8::FunctionTemplate>(PromiseRejected)->GetFunction());
}

bool PromiseCompletion::ForwardIfPromise(v8::Local<v8::Value> result, AsyncBaton *baton, Callback callback)
{
  Nan::HandleScope scope;

  // check if the result is a promise
  if (result->IsObject()) {
    Nan::MaybeLocal<v8::Value> maybeThenProp = Nan::Get(result->ToObject(), Nan::New("then").ToLocalChecked());
    if (!maybeThenProp.IsEmpty()) {
      v8::Local<v8::Value> thenProp = maybeThenProp.ToLocalChecked();
      if(thenProp->IsFunction()) {
        // we can be reasonably certain that the result is a promise

        // create a new v8 instance of PromiseCompletion
        v8::Local<v8::Object> object = Nan::NewInstance(Nan::New(newFn)).ToLocalChecked();

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
void PromiseCompletion::Setup(v8::Local<v8::Function> thenFn, v8::Local<v8::Value> result, AsyncBaton *baton, Callback callback) {
  this->callback = callback;
  this->baton = baton;

  v8::Local<v8::Object> promise = result->ToObject();

  v8::Local<v8::Object> thisHandle = handle();

  v8::Local<v8::Value> argv[2] = {
    Bind(promiseFulfilled, thisHandle),
    Bind(promiseRejected, thisHandle)
  };

  // call the promise's .then method with resolve and reject callbacks
  Nan::Callback(thenFn).Call(promise, 2, argv);
}

// binds an object to be the context of the function.
// there might be a better way to do this than calling Function.bind...
v8::Local<v8::Value> PromiseCompletion::Bind(Nan::Persistent<v8::Function> &function, v8::Local<v8::Object> object) {
  Nan::EscapableHandleScope scope;

  v8::Local<v8::Function> bind =
    Nan::Get(Nan::New(function), Nan::New("bind").ToLocalChecked())
      .ToLocalChecked().As<v8::Function>();

  v8::Local<v8::Value> argv[1] = { object };

  return scope.Escape(bind->Call(Nan::New(function), 1, argv));
}

// calls the callback stored in the PromiseCompletion, passing the baton that
// was provided in construction
void PromiseCompletion::CallCallback(bool isFulfilled, const Nan::FunctionCallbackInfo<v8::Value> &info) {
  v8::Local<v8::Value> resultOfPromise;

  if (info.Length() > 0) {
    resultOfPromise = info[0];
  }

  PromiseCompletion *promiseCompletion = ObjectWrap::Unwrap<PromiseCompletion>(info.This()->ToObject());

  (*promiseCompletion->callback)(isFulfilled, promiseCompletion->baton, resultOfPromise);
}

NAN_METHOD(PromiseCompletion::PromiseFulfilled) {
  CallCallback(true, info);
}

NAN_METHOD(PromiseCompletion::PromiseRejected) {
  CallCallback(false, info);
}
