#ifndef NODEGIT_ASYNC_WORKER
#define NODEGIT_ASYNC_WORKER

#include <napi.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <type_traits>
#include "lock_master.h"
#include "cleanup_handle.h"
#include "tracker_wrap.h"

namespace nodegit {
  // Standalone async worker (no longer derives from Nan::AsyncWorker). It owns a
  // Napi::FunctionReference for the JS callback, a Napi::AsyncContext for correct
  // async-hook propagation, and persistent storage for objects that must stay alive
  // across the async call (formerly Nan::AsyncWorker::SaveToPersistent).
  class AsyncWorker {
  public:
    AsyncWorker(Napi::FunctionReference *callback, const char *resourceName, std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> &cleanupHandles);
    AsyncWorker(Napi::FunctionReference *callback, const char *resourceName);
    AsyncWorker(const AsyncWorker &) = delete;
    AsyncWorker(AsyncWorker &&) = delete;
    AsyncWorker &operator=(const AsyncWorker &) = delete;
    AsyncWorker &operator=(AsyncWorker &&) = delete;
    virtual ~AsyncWorker();

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
    Napi::AsyncContext *GetAsyncResource();

    Napi::Reference<Napi::Value> *GetCallbackErrorHandle();

    bool GetIsCancelled() const;

    void SetErrorMessage(const std::string &msg);

    virtual const char *ErrorMessage() const { return errorMessage.c_str(); }

    // Invoked by the thread pool.
    virtual void Destroy();
    virtual void Execute() = 0;
    virtual void HandleOKCallback();
    virtual void HandleErrorCallback();
    void WorkComplete();

  public:
    // Invoke the JS callback (owned by the base) with the given arguments,
    // propagating the async context. Generated workers use this from
    // HandleOKCallback / HandleErrorCallback.
    Napi::Value CallCallback(const Napi::Value argv[], size_t argc) {
      napi_value recv = callback->Env().Global();
      std::vector<napi_value> args;
      args.reserve(argc);
      for (size_t i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
      }
      return callback->MakeCallback(recv, args, *asyncResource);
    }

    void RegisterCleanupCall(std::function<void()> cleanupCall);

    template<class NodeGitWrapperT>
    void Reference(Napi::Value item) {
      if (item.IsFunction() || item.IsString() || item.IsNull() || item.IsUndefined()) {
        return;
      }

      NodeGitWrapperT *objectWrapPointer;
      if constexpr (std::is_base_of_v<nodegit::TrackerWrap, NodeGitWrapperT>) {
        objectWrapPointer = static_cast<NodeGitWrapperT *>(
          Napi::ObjectWrap<nodegit::TrackerWrap>::Unwrap(item.As<Napi::Object>()));
      } else {
        objectWrapPointer = NodeGitWrapperT::Unwrap(item.As<Napi::Object>());
      }
      objectWrapPointer->Reference();
      RegisterCleanupCall([objectWrapPointer]() {
        objectWrapPointer->Unreference();
      });
    }

    template<class NodeGitWrapperT>
    inline void Reference(const char *label, Napi::Value item) {
      SaveToPersistent(label, item);
      Reference<NodeGitWrapperT>(item);
    }

    template<class NodeGitWrapperT>
    inline void Reference(const char *label, Napi::Object item) {
      SaveToPersistent(label, item);
      Reference<NodeGitWrapperT>(item);
    }

    template<class NodeGitWrapperT>
    inline void Reference(const char *label, Napi::Array array) {
      SaveToPersistent(label, array);
      for (uint32_t i = 0; i < array.Length(); ++i) {
        Reference<NodeGitWrapperT>(array.Get(i));
      }
    }

    inline void Reference(const char *label, Napi::Value item) {
      SaveToPersistent(label, item);
    }

    void SaveToPersistent(const char *label, const Napi::Value &value) {
      if (value.IsObject() || value.IsFunction() || value.IsExternal()) {
        persistentStorage[label] = Napi::Persistent(value);
        return;
      }

      if (primitives.IsEmpty()) {
        primitives = Napi::Persistent(Napi::Object::New(value.Env()));
      }

      primitives.Value().Set(label, value);
    }

    Napi::Value GetFromPersistent(const char *label) {
      if (!primitives.IsEmpty()) {
        Napi::Object bag = primitives.Value();
        if (bag.Has(label)) {
          return bag.Get(label);
        }
      }

      auto it = persistentStorage.find(label);
      if (it != persistentStorage.end()) {
        return it->second.Value();
      }
      return callback->Env().Undefined();
    }

  protected:
    std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
    Napi::Reference<Napi::Value> callbackErrorHandle;

  private:
    Napi::FunctionReference *callback;
    Napi::AsyncContext *asyncResource;
    std::vector<std::function<void()>> cleanupCalls;
    bool isCancelled = false;
    std::string errorMessage;
    std::map<std::string, Napi::Reference<Napi::Value>> persistentStorage;
    Napi::ObjectReference primitives;
  };
}

#endif
