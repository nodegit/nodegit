/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITBLOB_H
#define GITBLOB_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitBlob : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_blob *GetValue();

  private:
    GitBlob(git_blob *raw);
    ~GitBlob();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t* req);
    static void LookupAfterWork(uv_work_t* req);

    struct LookupBaton {
      uv_work_t request;
      const git_error* error;
      git_blob *out;
      git_repository * repo;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Content(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> CreateFromFile(const Arguments& args);
    static void CreateFromFileWork(uv_work_t* req);
    static void CreateFromFileAfterWork(uv_work_t* req);

    struct CreateFromFileBaton {
      uv_work_t request;
      const git_error* error;
      git_oid * id;
      git_repository * repo;
      const char * path;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateFromBuffer(const Arguments& args);
    static void CreateFromBufferWork(uv_work_t* req);
    static void CreateFromBufferAfterWork(uv_work_t* req);

    struct CreateFromBufferBaton {
      uv_work_t request;
      const git_error* error;
      git_oid * oid;
      git_repository * repo;
      const void * buffer;
      size_t len;
      Persistent<Function> callback;
    };
    static Handle<Value> IsBinary(const Arguments& args);
    git_blob *raw;
};

#endif
