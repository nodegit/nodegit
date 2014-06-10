/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTAG_H
#define GITTAG_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitTag : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_tag *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTag(git_tag *raw);
    ~GitTag();

    static NAN_METHOD(New);

    static NAN_METHOD(Oid);

    struct GetTargetBaton {
      int error_code;
      const git_error* error;
      git_object * target_out;
      const git_tag * tag;
    };
    class GetTargetWorker : public NanAsyncWorker {
      public:
        GetTargetWorker(
            GetTargetBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetTargetWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetTargetBaton *baton;
    };
    static NAN_METHOD(GetTarget);
    static NAN_METHOD(TargetId);
    static NAN_METHOD(TargetType);
    static NAN_METHOD(Name);
    static NAN_METHOD(Tagger);
    static NAN_METHOD(Message);
    static NAN_METHOD(Peel);
    git_tag *raw;
};

#endif
