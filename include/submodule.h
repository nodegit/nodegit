/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITSUBMODULE_H
#define GITSUBMODULE_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitSubmodule : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_submodule *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitSubmodule(git_submodule *raw);
    ~GitSubmodule();

    static NAN_METHOD(New);


    struct AddFinalizeBaton {
      int error_code;
      const git_error* error;
      git_submodule * submodule;
    };
    class AddFinalizeWorker : public NanAsyncWorker {
      public:
        AddFinalizeWorker(
            AddFinalizeBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~AddFinalizeWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        AddFinalizeBaton *baton;
    };
    static NAN_METHOD(AddFinalize);

    struct AddToIndexBaton {
      int error_code;
      const git_error* error;
      git_submodule * submodule;
      int write_index;
    };
    class AddToIndexWorker : public NanAsyncWorker {
      public:
        AddToIndexWorker(
            AddToIndexBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~AddToIndexWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        AddToIndexBaton *baton;
    };
    static NAN_METHOD(AddToIndex);

    struct SaveBaton {
      int error_code;
      const git_error* error;
      git_submodule * submodule;
    };
    class SaveWorker : public NanAsyncWorker {
      public:
        SaveWorker(
            SaveBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~SaveWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        SaveBaton *baton;
    };
    static NAN_METHOD(Save);
    static NAN_METHOD(Name);
    static NAN_METHOD(Path);
    static NAN_METHOD(Url);
    static NAN_METHOD(SetUrl);
    static NAN_METHOD(IndexId);
    static NAN_METHOD(HeadId);

    struct InitBaton {
      int error_code;
      const git_error* error;
      git_submodule * submodule;
      int overwrite;
    };
    class InitWorker : public NanAsyncWorker {
      public:
        InitWorker(
            InitBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~InitWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        InitBaton *baton;
    };
    static NAN_METHOD(Init);

    struct SyncBaton {
      int error_code;
      const git_error* error;
      git_submodule * submodule;
    };
    class SyncWorker : public NanAsyncWorker {
      public:
        SyncWorker(
            SyncBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~SyncWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        SyncBaton *baton;
    };
    static NAN_METHOD(Sync);

    struct OpenBaton {
      int error_code;
      const git_error* error;
      git_repository * repo;
      git_submodule * submodule;
    };
    class OpenWorker : public NanAsyncWorker {
      public:
        OpenWorker(
            OpenBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~OpenWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        OpenBaton *baton;
    };
    static NAN_METHOD(Open);

    struct ReloadBaton {
      int error_code;
      const git_error* error;
      git_submodule * submodule;
    };
    class ReloadWorker : public NanAsyncWorker {
      public:
        ReloadWorker(
            ReloadBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~ReloadWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        ReloadBaton *baton;
    };
    static NAN_METHOD(Reload);

    struct StatusBaton {
      int error_code;
      const git_error* error;
      unsigned int * status;
      git_submodule * submodule;
    };
    class StatusWorker : public NanAsyncWorker {
      public:
        StatusWorker(
            StatusBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~StatusWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        StatusBaton *baton;
    };
    static NAN_METHOD(Status);
    git_submodule *raw;
};

#endif
