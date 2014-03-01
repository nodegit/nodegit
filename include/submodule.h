/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITSUBMODULE_H
#define GITSUBMODULE_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitSubmodule : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_submodule *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitSubmodule(git_submodule *raw);
    ~GitSubmodule();

    static NAN_METHOD(New);

    static NAN_METHOD(AddFinalize);
    static void AddFinalizeWork(uv_work_t* req);
    static void AddFinalizeAfterWork(uv_work_t* req);

    struct AddFinalizeBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> submoduleReference;
      git_submodule * submodule;
      Persistent<Function> callback;
    };
    static NAN_METHOD(AddToIndex);
    static void AddToIndexWork(uv_work_t* req);
    static void AddToIndexAfterWork(uv_work_t* req);

    struct AddToIndexBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> submoduleReference;
      git_submodule * submodule;
      Persistent<Value> write_indexReference;
      int write_index;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Save);
    static void SaveWork(uv_work_t* req);
    static void SaveAfterWork(uv_work_t* req);

    struct SaveBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> submoduleReference;
      git_submodule * submodule;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Name);
    static NAN_METHOD(Path);
    static NAN_METHOD(Url);
    static NAN_METHOD(SetUrl);
    static NAN_METHOD(IndexId);
    static NAN_METHOD(HeadId);
    static NAN_METHOD(Init);
    static void InitWork(uv_work_t* req);
    static void InitAfterWork(uv_work_t* req);

    struct InitBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> submoduleReference;
      git_submodule * submodule;
      Persistent<Value> overwriteReference;
      int overwrite;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Sync);
    static void SyncWork(uv_work_t* req);
    static void SyncAfterWork(uv_work_t* req);

    struct SyncBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> submoduleReference;
      git_submodule * submodule;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Open);
    static void OpenWork(uv_work_t* req);
    static void OpenAfterWork(uv_work_t* req);

    struct OpenBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_repository * repo;
      Persistent<Value> submoduleReference;
      git_submodule * submodule;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Reload);
    static void ReloadWork(uv_work_t* req);
    static void ReloadAfterWork(uv_work_t* req);

    struct ReloadBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> submoduleReference;
      git_submodule * submodule;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Status);
    static void StatusWork(uv_work_t* req);
    static void StatusAfterWork(uv_work_t* req);

    struct StatusBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> statusReference;
      unsigned int * status;
      Persistent<Value> submoduleReference;
      git_submodule * submodule;
      Persistent<Function> callback;
    };
    git_submodule *raw;
};

#endif
