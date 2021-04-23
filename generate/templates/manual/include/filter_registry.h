#ifndef GITFILTERREGISTRY_H
#define GITFILTERREGISTRY_H
#include <nan.h>
#include <string>
#include <utility>

#include "async_baton.h"
#include "async_worker.h"
#include "cleanup_handle.h"
#include "context.h"
#include "lock_master.h"
#include "nodegit_wrapper.h"
#include "promise_completion.h"

extern "C" {
#include <git2.h>
}

#include "../include/typedefs.h"

#include "../include/filter.h"

using namespace node;
using namespace v8;


class GitFilterRegistry : public Nan::ObjectWrap {
   public:
    static void InitializeComponent(v8::Local<v8::Object> target, nodegit::Context *nodegitContext);

  private:

    static NAN_METHOD(GitFilterRegister);

    static NAN_METHOD(GitFilterUnregister);

    struct FilterRegisterBaton {
      const git_error *error;
      git_filter *filter;
      char *filter_name;
      int filter_priority;
      int error_code;
    };

    struct FilterUnregisterBaton {
      const git_error *error;
      char *filter_name;
      int error_code;
    };

    class RegisterWorker : public nodegit::AsyncWorker {
      public:
        RegisterWorker(FilterRegisterBaton *_baton, Nan::Callback *callback, std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> &cleanupHandles)
        : nodegit::AsyncWorker(callback, "nodegit:AsyncWorker:FilterRegistry:Register", cleanupHandles), baton(_baton) {};
        RegisterWorker(const RegisterWorker &) = delete;
        RegisterWorker(RegisterWorker &&) = delete;
        RegisterWorker &operator=(const RegisterWorker &) = delete;
        RegisterWorker &operator=(RegisterWorker &&) = delete;
        ~RegisterWorker() {};
        void Execute();
        void HandleErrorCallback();
        void HandleOKCallback();
        nodegit::LockMaster AcquireLocks();

      private:
        FilterRegisterBaton *baton;
    };

    class UnregisterWorker : public nodegit::AsyncWorker {
      public:
        UnregisterWorker(FilterUnregisterBaton *_baton, Nan::Callback *callback)
        : nodegit::AsyncWorker(callback, "nodegit:AsyncWorker:FilterRegistry:Unregister"), baton(_baton) {};
        UnregisterWorker(const UnregisterWorker &) = delete;
        UnregisterWorker(UnregisterWorker &&) = delete;
        UnregisterWorker &operator=(const UnregisterWorker &) = delete;
        UnregisterWorker &operator=(UnregisterWorker &&) = delete;
        ~UnregisterWorker() {};
        void Execute();
        void HandleErrorCallback();
        void HandleOKCallback();
        nodegit::LockMaster AcquireLocks();

      private:
        FilterUnregisterBaton *baton;
    };
};

#endif
