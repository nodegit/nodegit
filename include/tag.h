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


    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t* req);
    static void LookupAfterWork(uv_work_t* req);

    struct LookupBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_tag * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Target(const Arguments& args);
    static void TargetWork(uv_work_t* req);
    static void TargetAfterWork(uv_work_t* req);

    struct TargetBaton {
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
    static Handle<Value> Create(const Arguments& args);
    static void CreateWork(uv_work_t* req);
    static void CreateAfterWork(uv_work_t* req);

    struct CreateBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * oid;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> tag_nameReference;
      const char * tag_name;
      Persistent<Value> targetReference;
      const git_object * target;
      Persistent<Value> taggerReference;
      const git_signature * tagger;
      Persistent<Value> messageReference;
      const char * message;
      Persistent<Value> forceReference;
      int force;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateLightweight(const Arguments& args);
    static void CreateLightweightWork(uv_work_t* req);
    static void CreateLightweightAfterWork(uv_work_t* req);

    struct CreateLightweightBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * oid;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> tag_nameReference;
      const char * tag_name;
      Persistent<Value> targetReference;
      const git_object * target;
      Persistent<Value> forceReference;
      int force;
      Persistent<Function> callback;
    };
    static Handle<Value> Delete(const Arguments& args);
    static Handle<Value> Peel(const Arguments& args);
    git_tag *raw;
};

#endif
