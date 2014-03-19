/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTAG_H
#define GITTAG_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitTag : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_tag *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTag(git_tag *raw);
    ~GitTag();

    static NAN_METHOD(New);

    static NAN_METHOD(Oid);
    static NAN_METHOD(GetTarget);
    static void GetTargetWork(uv_work_t* req);
    static void GetTargetAfterWork(uv_work_t* req);

    struct GetTargetBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_object * target_out;
      Persistent<Value> tagReference;
      const git_tag * tag;
      Persistent<Function> callback;
    };
    static NAN_METHOD(TargetId);
    static NAN_METHOD(TargetType);
    static NAN_METHOD(Name);
    static NAN_METHOD(Tagger);
    static NAN_METHOD(Message);
    static NAN_METHOD(Peel);
    git_tag *raw;
};

#endif
