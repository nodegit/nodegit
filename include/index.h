/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITINDEX_H
#define GITINDEX_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitIndex : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_index *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitIndex(git_index *raw);
    ~GitIndex();

    static NAN_METHOD(New);

    static NAN_METHOD(Open);
    static void OpenWork(uv_work_t* req);
    static void OpenAfterWork(uv_work_t* req);

    struct OpenBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_index * out;
      Persistent<Value> index_pathReference;
      const char * index_path;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Read);
    static void ReadWork(uv_work_t* req);
    static void ReadAfterWork(uv_work_t* req);

    struct ReadBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Write);
    static void WriteWork(uv_work_t* req);
    static void WriteAfterWork(uv_work_t* req);

    struct WriteBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Function> callback;
    };
    static NAN_METHOD(ReadTree);
    static void ReadTreeWork(uv_work_t* req);
    static void ReadTreeAfterWork(uv_work_t* req);

    struct ReadTreeBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Value> treeReference;
      const git_tree * tree;
      Persistent<Function> callback;
    };
    static NAN_METHOD(WriteTree);
    static void WriteTreeWork(uv_work_t* req);
    static void WriteTreeAfterWork(uv_work_t* req);

    struct WriteTreeBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * out;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Size);
    static NAN_METHOD(Clear);
    static NAN_METHOD(Entry);
    static NAN_METHOD(Remove);
    static NAN_METHOD(RemoveDirectory);
    static NAN_METHOD(AddBypath);
    static void AddBypathWork(uv_work_t* req);
    static void AddBypathAfterWork(uv_work_t* req);

    struct AddBypathBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Function> callback;
    };
    static NAN_METHOD(RemoveBypath);
    static NAN_METHOD(Find);
    static NAN_METHOD(ConflictRemove);
    static NAN_METHOD(ConflictCleanup);
    static NAN_METHOD(HasConflicts);
    static NAN_METHOD(IndexToWorkdir);
    static void IndexToWorkdirWork(uv_work_t* req);
    static void IndexToWorkdirAfterWork(uv_work_t* req);

    struct IndexToWorkdirBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_diff_list * diff;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Value> optsReference;
      const git_diff_options * opts;
      Persistent<Function> callback;
    };
    git_index *raw;
};

#endif
