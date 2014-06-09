/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREFERENCE_H
#define GITREFERENCE_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitReference : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_reference *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitReference(git_reference *raw);
    ~GitReference();

    static NAN_METHOD(New);


    struct OidForNameBaton {
      int error_code;
      const git_error* error;
      git_oid * out;
      git_repository * repo;
      const char * name;
    };
    class OidForNameWorker : public NanAsyncWorker {
      public:
        OidForNameWorker(
            OidForNameBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~OidForNameWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        OidForNameBaton *baton;
    };
    static NAN_METHOD(OidForName);
    static NAN_METHOD(Target);
    static NAN_METHOD(SymbolicTarget);
    static NAN_METHOD(Type);
    static NAN_METHOD(Name);

    struct ResolveBaton {
      int error_code;
      const git_error* error;
      git_reference * out;
      const git_reference * ref;
    };
    class ResolveWorker : public NanAsyncWorker {
      public:
        ResolveWorker(
            ResolveBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~ResolveWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        ResolveBaton *baton;
    };
    static NAN_METHOD(Resolve);
    static NAN_METHOD(SetSymbolicTarget);
    static NAN_METHOD(setTarget);

    struct RenameBaton {
      int error_code;
      const git_error* error;
      git_reference * out;
      git_reference * ref;
      const char * new_name;
      int force;
    };
    class RenameWorker : public NanAsyncWorker {
      public:
        RenameWorker(
            RenameBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~RenameWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        RenameBaton *baton;
    };
    static NAN_METHOD(Rename);

    struct DeleteBaton {
      int error_code;
      const git_error* error;
      git_reference * ref;
    };
    class DeleteWorker : public NanAsyncWorker {
      public:
        DeleteWorker(
            DeleteBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~DeleteWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        DeleteBaton *baton;
    };
    static NAN_METHOD(Delete);
    static NAN_METHOD(IsBranch);
    static NAN_METHOD(IsRemote);
    static NAN_METHOD(Peel);
    static NAN_METHOD(IsValidName);
    git_reference *raw;
};

#endif
