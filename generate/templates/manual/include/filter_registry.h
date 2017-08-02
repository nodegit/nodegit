#ifndef GITFILTERREGISTRY_H
#define GITFILTERREGISTRY_H
#include <nan.h>
#include <string>
#include <queue>
#include <utility>

#include "async_baton.h"
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
    static void InitializeComponent(v8::Local<v8::Object> target);

    static Nan::Persistent<v8::Object> persistentHandle;

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

    class RegisterWorker : public Nan::AsyncWorker {
      public:
        RegisterWorker(FilterRegisterBaton *_baton, Nan::Callback *callback) 
        : Nan::AsyncWorker(callback), baton(_baton) {};
        ~RegisterWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        FilterRegisterBaton *baton;
    };

    class UnregisterWorker : public Nan::AsyncWorker {
      public:
        UnregisterWorker(FilterUnregisterBaton *_baton, Nan::Callback *callback) 
        : Nan::AsyncWorker(callback), baton(_baton) {};
        ~UnregisterWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        FilterUnregisterBaton *baton;
    };
};

#endif
