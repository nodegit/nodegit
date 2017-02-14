#ifndef CALLBACK_WRAPPER_H
#define CALLBACK_WRAPPER_H

#include <nan.h>
#include <uv.h>

using namespace v8;
using namespace node;

class CallbackWrapper {
  Nan::Callback* jsCallback;

  // throttling data, used for callbacks that need to be throttled
  int throttle; // in milliseconds - if > 0, calls to the JS callback will be throttled
  uint64_t lastCallTime;

public:
  CallbackWrapper() {
    jsCallback = NULL;
    lastCallTime = 0;
    throttle = 0;
  }

  ~CallbackWrapper() {
    SetCallback(NULL);
  }

  bool HasCallback() {
    return jsCallback != NULL;
  }

  Nan::Callback* GetCallback() {
    return jsCallback;
  }

  void SetCallback(Nan::Callback* callback, int throttle = 0) {
    if(jsCallback) {
      delete jsCallback;
    }
    jsCallback = callback;
    this->throttle = throttle;
  }

  bool WillBeThrottled() {
    if(!throttle) {
      return false;
    }
    // throttle if needed
    uint64_t now = uv_hrtime();
    if(lastCallTime > 0 && now < lastCallTime + throttle * (uint64_t)1000000) {
      // throttled
      return true;
    } else {
      lastCallTime = now;
      return false;
    }
  }
};

#endif
