/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREFERENCE_H
#define GITREFERENCE_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitReference : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_reference *GetValue();

  private:
    GitReference(git_reference *raw);
    ~GitReference();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t* req);
    static void LookupAfterWork(uv_work_t* req);

    struct LookupBaton {
      uv_work_t request;
      const git_error* error;
      git_reference * out;
      git_repository * repo;
      const char * name;
      Persistent<Function> callback;
    };
    static Handle<Value> OidForName(const Arguments& args);
    static void OidForNameWork(uv_work_t* req);
    static void OidForNameAfterWork(uv_work_t* req);

    struct OidForNameBaton {
      uv_work_t request;
      const git_error* error;
      git_oid * out;
      git_repository * repo;
      const char * name;
      Persistent<Function> callback;
    };
    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Type(const Arguments& args);
    static Handle<Value> Resolve(const Arguments& args);
    static void ResolveWork(uv_work_t* req);
    static void ResolveAfterWork(uv_work_t* req);

    struct ResolveBaton {
      uv_work_t request;
      const git_error* error;
      git_reference * out;
      const git_reference * ref;
      Persistent<Function> callback;
    };
    git_reference *raw;
};

#endif
