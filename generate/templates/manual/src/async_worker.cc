#include "../include/async_worker.h"

namespace nodegit {
  AsyncWorker::AsyncWorker(Nan::Callback *callback, const char *resourceName)
    : Nan::AsyncWorker(callback, resourceName)
  {}

  Nan::AsyncResource *AsyncWorker::GetAsyncResource() {
    return async_resource;
  }
}
