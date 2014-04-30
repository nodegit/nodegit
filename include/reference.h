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

    static Handle<Value> New(void *raw);

  private:
    GitReference(git_reference *raw);
    ~GitReference();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> OidForName(const Arguments& args);
    static void OidForNameWork(uv_work_t* req);
    static void OidForNameAfterWork(uv_work_t* req);

    struct OidForNameBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> nameReference;
      const char * name;
      Persistent<Function> callback;
    };
    static Handle<Value> Target(const Arguments& args);
    static Handle<Value> SymbolicTarget(const Arguments& args);
    static Handle<Value> Type(const Arguments& args);
    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Resolve(const Arguments& args);
    static void ResolveWork(uv_work_t* req);
    static void ResolveAfterWork(uv_work_t* req);

    struct ResolveBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_reference * out;
      Persistent<Value> refReference;
      const git_reference * ref;
      Persistent<Function> callback;
    };
    static Handle<Value> SetSymbolicTarget(const Arguments& args);
    static Handle<Value> setTarget(const Arguments& args);
    static Handle<Value> Rename(const Arguments& args);
    static void RenameWork(uv_work_t* req);
    static void RenameAfterWork(uv_work_t* req);

    struct RenameBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_reference * out;
      Persistent<Value> refReference;
      git_reference * ref;
      Persistent<Value> new_nameReference;
      const char * new_name;
      Persistent<Value> forceReference;
      int force;
      Persistent<Function> callback;
    };
    static Handle<Value> Delete(const Arguments& args);
    static void DeleteWork(uv_work_t* req);
    static void DeleteAfterWork(uv_work_t* req);

    struct DeleteBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> refReference;
      git_reference * ref;
      Persistent<Function> callback;
    };
    static Handle<Value> IsBranch(const Arguments& args);
    static Handle<Value> IsRemote(const Arguments& args);
    static Handle<Value> Peel(const Arguments& args);
    static Handle<Value> IsValidName(const Arguments& args);
    git_reference *raw;
};

#endif
