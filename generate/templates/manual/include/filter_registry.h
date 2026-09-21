#ifndef FILTER_REGISTRY_H
#define FILTER_REGISTRY_H

#include <napi.h>
#include <string>

#include "async_baton.h"
#include "async_worker.h"
#include "lock_master.h"
#include "promise_completion.h"
#include "cleanup_handle.h"

extern "C" {
  #include <git2.h>
  #include <git2/sys/filter.h>
}

#include <map>
#include <vector>

namespace nodegit {
  class GitFilterRegistry : public Napi::ObjectWrap<GitFilterRegistry> {
    public:
      GitFilterRegistry(const Napi::CallbackInfo& info);
      static void InitializeComponent (Napi::Object target, nodegit::Context *nodegitContext);

      static Napi::Value New(void *raw);

      git_filter *GetValue();
      void Reference();
      void Unreference();

      void AddReferenceCallbacks(size_t fieldIndex, std::function<void()> refCb, std::function<void()> unrefCb);

      ~GitFilterRegistry();

    private:
      GitFilterRegistry(const GitFilterRegistry &) = delete;
      GitFilterRegistry(GitFilterRegistry &&) = delete;
      GitFilterRegistry &operator=(const GitFilterRegistry &) = delete;
      GitFilterRegistry &operator=(GitFilterRegistry &&) = delete;

      git_filter *raw;

      static Napi::Value JSNewFunction(const Napi::CallbackInfo& info);
      static Napi::Value Register(const Napi::CallbackInfo& info);
      static Napi::Value Unregister(const Napi::CallbackInfo& info);

      struct RegisterBaton : public nodegit::AsyncBaton {
        const char *name;
        git_filter *filter;
        const git_error *error;
        int priority;
        int error_code;
        bool failedRequest;
      };
      class RegisterWorker : public nodegit::AsyncWorker {
        public:
          RegisterWorker(
              RegisterBaton *_baton,
              Napi::FunctionReference *callback,
              std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> &_cleanupHandles
          ) : nodegit::AsyncWorker(callback, "nodegit:AsyncWorker:GitFilterRegistry:Register", _cleanupHandles)
            , baton(_baton) {};
          RegisterWorker(const RegisterWorker &) = delete;
          RegisterWorker(RegisterWorker &&) = delete;
          RegisterWorker &operator=(const RegisterWorker &) = delete;
          RegisterWorker &operator=(RegisterWorker &&) = delete;
          ~RegisterWorker(){};
          void Execute();
          void HandleErrorCallback();
          void HandleOKCallback();
          nodegit::LockMaster AcquireLocks();

        private:
          RegisterBaton *baton;
      };

      struct UnregisterBaton : public nodegit::AsyncBaton {
        const char *name;
        const git_error *error;
        int error_code;
        bool failedRequest;
      };
      class UnregisterWorker : public nodegit::AsyncWorker {
        public:
          UnregisterWorker(
              UnregisterBaton *_baton,
              Napi::FunctionReference *callback
          ) : nodegit::AsyncWorker(callback, "nodegit:AsyncWorker:GitFilterRegistry:Unregister")
            , baton(_baton) {};
          UnregisterWorker(const UnregisterWorker &) = delete;
          UnregisterWorker(UnregisterWorker &&) = delete;
          UnregisterWorker &operator=(const UnregisterWorker &) = delete;
          UnregisterWorker &operator=(UnregisterWorker &&) = delete;
          ~UnregisterWorker(){};
          void Execute();
          void HandleErrorCallback();
          void HandleOKCallback();
          nodegit::LockMaster AcquireLocks();

        private:
          UnregisterBaton *baton;
      };
  };
}

#endif
