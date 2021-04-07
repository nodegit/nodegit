#ifndef NODEGIT_ASYNC_WORKER
#define NODEGIT_ASYNC_WORKER

#include <nan.h>
#include <functional>
#include "lock_master.h"

namespace nodegit {
  class AsyncWorker : public Nan::AsyncWorker {
  public:
    AsyncWorker(Nan::Callback *callback, const char *resourceName);

    // This must be implemented by every async worker
    // so that the thread pool can lock separately
    // from the execute method in the AsyncWorker
    virtual nodegit::LockMaster AcquireLocks() = 0;

    // Ensure that the `HandleErrorCallback` will be called
    // when the AsyncWork is complete
    void Cancel();

    // Retrieves the async resource attached to this AsyncWorker
    // This is used to inform libgit2 callbacks what asyncResource
    // they should use when working with any javascript
    Nan::AsyncResource *GetAsyncResource();

    bool GetIsCancelled() const;

    void Destroy() override;

    void RegisterCleanupCall(std::function<void()> cleanupCall);

    template<class NodeGitWrapperT>
    void Reference(v8::Local<v8::Value> item) {
      if (item->IsString() || item->IsNull() || item->IsUndefined()) {
        return;
      }

      auto objectWrapPointer = Nan::ObjectWrap::Unwrap<NodeGitWrapperT>(item.As<v8::Object>());
      objectWrapPointer->Reference();
      RegisterCleanupCall([objectWrapPointer]() {
        objectWrapPointer->Unreference();
      });
    }

    template<class NodeGitWrapperT>
    inline void Reference(const char *label, v8::Local<v8::Value> item) {
      SaveToPersistent(label, item);
      Reference<NodeGitWrapperT>(item);
    }

    template<class NodeGitWrapperT>
    inline void Reference(const char *label, v8::Local<v8::Object> item) {
      SaveToPersistent(label, item);
      Reference<NodeGitWrapperT>(item);
    }

    template<class NodeGitWrapperT>
    inline void Reference(const char *label, v8::Local<v8::Array> array) {
      SaveToPersistent(label, array);
      for (uint32_t i = 0; i < array->Length(); ++i) {
        Reference<NodeGitWrapperT>(Nan::Get(array, i).ToLocalChecked());
      }
    }

    inline void Reference(const char *label, v8::Local<v8::Value> item) {
      SaveToPersistent(label, item);
    }

  private:
    std::vector<std::function<void()>> cleanupCalls;
    bool isCancelled = false;
  };
}

#endif
