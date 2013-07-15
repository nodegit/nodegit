/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTAG_H
#define GITTAG_H

#include <v8.h>
#include <node.h>
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

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> GetTarget(const Arguments& args);
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
    static Handle<Value> TargetId(const Arguments& args);
    static Handle<Value> TargetType(const Arguments& args);
    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Tagger(const Arguments& args);
    static Handle<Value> Message(const Arguments& args);
    static Handle<Value> Peel(const Arguments& args);
    git_tag *raw;
};

#endif
