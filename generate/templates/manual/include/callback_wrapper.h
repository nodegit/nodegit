#ifndef CALLBACK_WRAPPER_H
#define CALLBACK_WRAPPER_H

#include <nan.h>
#include <uv.h>
#include <memory>

using namespace v8;
using namespace node;

class CallbackWrapper {
  std::unique_ptr<Nan::Callback> jsCallback;

  // throttling data, used for callbacks that need to be throttled
  uint32_t throttle; // in milliseconds - if > 0, calls to the JS callback will be throttled
  uint64_t lastCallTime;

  // false will trigger the callback and not wait for the callback to finish
  // in this case, the underlying libgit2 function will immediately be given
  // the default result
  bool waitForResult;

public:
  CallbackWrapper(): jsCallback(nullptr), throttle(0), lastCallTime(0) {}

  CallbackWrapper(const CallbackWrapper &) = delete;
  CallbackWrapper(CallbackWrapper &&) = delete;
  CallbackWrapper &operator=(const CallbackWrapper &) = delete;
  CallbackWrapper &operator=(CallbackWrapper &&) = delete;

  bool HasCallback() {
    return jsCallback != nullptr;
  }

  Nan::Callback* GetCallback() {
    return jsCallback.get();
  }

  void SetCallback(std::unique_ptr<Nan::Callback> callback, uint32_t throttle = 0, bool waitForResult = true) {
    jsCallback = std::move(callback);
    this->throttle = throttle;
    this->waitForResult = waitForResult;
  }

  bool ShouldWaitForResult() {
    return waitForResult;
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
