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

    static Handle<Value> New(void *raw);

  private:
    GitBlob(git_blob *raw);
    ~GitBlob();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Content(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> CreateFromFile(const Arguments& args);
    static void CreateFromFileWork(uv_work_t* req);
    static void CreateFromFileAfterWork(uv_work_t* req);

    struct CreateFromFileBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * id;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateFromBuffer(const Arguments& args);
    static void CreateFromBufferWork(uv_work_t* req);
    static void CreateFromBufferAfterWork(uv_work_t* req);

    struct CreateFromBufferBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * oid;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> bufferReference;
      const void * buffer;
      Persistent<Value> lenReference;
      size_t len;
      Persistent<Function> callback;
    };
    static Handle<Value> IsBinary(const Arguments& args);
    git_blob *raw;
};

#endif
