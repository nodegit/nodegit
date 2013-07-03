/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITODB_H
#define GITODB_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitOdb : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_odb *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitOdb(git_odb *raw);
    ~GitOdb();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Create(const Arguments& args);
    static Handle<Value> Open(const Arguments& args);
    static Handle<Value> AddDiskAlternate(const Arguments& args);
    static Handle<Value> Read(const Arguments& args);
    static void ReadWork(uv_work_t* req);
    static void ReadAfterWork(uv_work_t* req);

    struct ReadBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_odb_object * out;
      Persistent<Value> dbReference;
      git_odb * db;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> ReadPrefix(const Arguments& args);
    static Handle<Value> ReadHeader(const Arguments& args);
    static Handle<Value> Exists(const Arguments& args);
    static Handle<Value> Refresh(const Arguments& args);
    static Handle<Value> Write(const Arguments& args);
    static void WriteWork(uv_work_t* req);
    static void WriteAfterWork(uv_work_t* req);

    struct WriteBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * out;
      Persistent<Value> odbReference;
      git_odb * odb;
      Persistent<Value> dataReference;
      const void * data;
      Persistent<Value> lenReference;
      size_t len;
      Persistent<Value> typeReference;
      git_otype type;
      Persistent<Function> callback;
    };
    static Handle<Value> Hash(const Arguments& args);
    static Handle<Value> Hashfile(const Arguments& args);
    git_odb *raw;
};

#endif
