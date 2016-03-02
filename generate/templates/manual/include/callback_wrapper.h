#ifndef CALLBACK_WRAPPER_H
#define CALLBACK_WRAPPER_H

#include <v8.h>
#include <node.h>

#include "nan.h"

using namespace v8;
using namespace node;

class CallbackWrapper {
  Nan::Callback* jsCallback;

public:
  CallbackWrapper() {
    jsCallback = NULL;
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

  void SetCallback(Nan::Callback* callback) {
    if(jsCallback) {
      delete jsCallback;
    }
    jsCallback = callback;
  }
};

#endif
