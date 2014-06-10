/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTREEENTRY_H
#define GITTREEENTRY_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitTreeEntry : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_tree_entry *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTreeEntry(git_tree_entry *raw);
    ~GitTreeEntry();

    static NAN_METHOD(New);

    static NAN_METHOD(Name);
    static NAN_METHOD(Oid);
    static NAN_METHOD(Type);
    static NAN_METHOD(filemode);

    struct GetObjectBaton {
      int error_code;
      const git_error* error;
      git_object * object_out;
      git_repository * repo;
      const git_tree_entry * entry;
    };
    class GetObjectWorker : public NanAsyncWorker {
      public:
        GetObjectWorker(
            GetObjectBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetObjectWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetObjectBaton *baton;
    };
    static NAN_METHOD(GetObject);
    git_tree_entry *raw;
};

#endif
