#include "../include/async_worker.h"

namespace nodegit {
  AsyncWorker::AsyncWorker(Napi::FunctionReference *callback, const char *resourceName, std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> &_cleanupHandles)
    : cleanupHandles(_cleanupHandles),
      callback(callback),
      asyncResource(new Napi::AsyncContext(callback->Env(), resourceName))
  {}

  AsyncWorker::AsyncWorker(Napi::FunctionReference *callback, const char *resourceName)
    : callback(callback),
      asyncResource(new Napi::AsyncContext(callback->Env(), resourceName))
  {}

  AsyncWorker::~AsyncWorker() {
    delete callback;
    delete asyncResource;
  }

  void AsyncWorker::Cancel() {
    isCancelled = true;

    // We use the errorMessage flow (mirroring Nan::AsyncWorker) to trigger
    // `HandleErrorCallback` for cancellation of AsyncWork.
    SetErrorMessage("SHUTTING DOWN");
  }

  void AsyncWorker::SetErrorMessage(const std::string &msg) {
    errorMessage = msg;
  }

  Napi::AsyncContext *AsyncWorker::GetAsyncResource() {
    return asyncResource;
  }

  Napi::Reference<Napi::Value> *AsyncWorker::GetCallbackErrorHandle() {
    return &callbackErrorHandle;
  }

  bool AsyncWorker::GetIsCancelled() const {
    return isCancelled;
  }

  void AsyncWorker::HandleOKCallback() {
    Napi::Env env = GetAsyncResource()->Env();
    Napi::HandleScope scope(env);

    Napi::Value argv[2] = {
      env.Null(),
      env.Undefined()
    };
    CallCallback(argv, 2);
  }

  void AsyncWorker::HandleErrorCallback() {
    Napi::Env env = GetAsyncResource()->Env();
    Napi::HandleScope scope(env);

    Napi::Object err = Napi::Error::New(env, ErrorMessage()).Value();
    err.Set("errorFunction", Napi::String::New(env, "AsyncWorker"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
  }

  void AsyncWorker::WorkComplete() {
    if (!errorMessage.empty()) {
      HandleErrorCallback();
    } else {
      HandleOKCallback();
    }
  }

  void AsyncWorker::Destroy() {
    std::for_each(cleanupCalls.begin(), cleanupCalls.end(), [](std::function<void()> cleanupCall) {
      cleanupCall();
    });
    delete this;
  }

  void AsyncWorker::RegisterCleanupCall(std::function<void()> cleanupCall) {
    cleanupCalls.push_back(cleanupCall);
  }
}
