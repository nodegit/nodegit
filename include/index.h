/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef INDEX_H
#define INDEX_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class Index : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_index *GetValue();

  private:
    Index(git_index *raw);
    ~Index();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Open(const Arguments& args);
    static void OpenWork(uv_work_t* req);
    static void OpenAfterWork(uv_work_t* req);

    struct OpenBaton {
      uv_work_t request;
      const git_error* error;
      git_index * out;
      Persistent<Value> index_pathReference;
      const char * index_path;
      Persistent<Function> callback;
    };
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Owner(const Arguments& args);
    static Handle<Value> Read(const Arguments& args);
    static void ReadWork(uv_work_t* req);
    static void ReadAfterWork(uv_work_t* req);

    struct ReadBaton {
      uv_work_t request;
      const git_error* error;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Function> callback;
    };
    static Handle<Value> Write(const Arguments& args);
    static void WriteWork(uv_work_t* req);
    static void WriteAfterWork(uv_work_t* req);

    struct WriteBaton {
      uv_work_t request;
      const git_error* error;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Function> callback;
    };
    static Handle<Value> ReadTree(const Arguments& args);
    static void ReadTreeWork(uv_work_t* req);
    static void ReadTreeAfterWork(uv_work_t* req);

    struct ReadTreeBaton {
      uv_work_t request;
      const git_error* error;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Value> treeReference;
      const git_tree * tree;
      Persistent<Function> callback;
    };
    static Handle<Value> WriteTree(const Arguments& args);
    static void WriteTreeWork(uv_work_t* req);
    static void WriteTreeAfterWork(uv_work_t* req);

    struct WriteTreeBaton {
      uv_work_t request;
      const git_error* error;
      git_oid * out;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Function> callback;
    };
    static Handle<Value> Entrycount(const Arguments& args);
    static Handle<Value> Clear(const Arguments& args);
    static Handle<Value> Remove(const Arguments& args);
    static Handle<Value> RemoveDirectory(const Arguments& args);
    static Handle<Value> AddBypath(const Arguments& args);
    static void AddBypathWork(uv_work_t* req);
    static void AddBypathAfterWork(uv_work_t* req);

    struct AddBypathBaton {
      uv_work_t request;
      const git_error* error;
      Persistent<Value> indexReference;
      git_index * index;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Function> callback;
    };
    static Handle<Value> RemoveBypath(const Arguments& args);
    static Handle<Value> Find(const Arguments& args);
    static Handle<Value> ConflictRemove(const Arguments& args);
    static Handle<Value> ConflictCleanup(const Arguments& args);
    static Handle<Value> HasConflicts(const Arguments& args);
    git_index *raw;
};

#endif
