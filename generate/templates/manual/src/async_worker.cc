#include "../include/async_worker.h"

namespace nodegit {
  AsyncWorker::AsyncWorker(Nan::Callback *callback, const char *resourceName)
    : Nan::AsyncWorker(callback, resourceName)
  {}

  void AsyncWorker::Cancel() {
    isCancelled = true;

    // We use Nan::AsyncWorker's ErrorMessage flow
    // to trigger `HandleErrorCallback` for cancellation
    // of AsyncWork
    SetErrorMessage("SHUTTING DOWN");
  }

  Nan::AsyncResource *AsyncWorker::GetAsyncResource() {
    return async_resource;
  }

  bool AsyncWorker::GetIsCancelled() const {
    return isCancelled;
  }

  void AsyncWorker::Destroy() {
    std::for_each(cleanupCalls.begin(), cleanupCalls.end(), [](std::function<void()> cleanupCall) {
      cleanupCall();
    });
    Nan::AsyncWorker::Destroy();
  }

  void AsyncWorker::RegisterCleanupCall(std::function<void()> cleanupCall) {
    cleanupCalls.push_back(cleanupCall);
  }
}
