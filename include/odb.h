/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITODB_H
#define GITODB_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitOdb : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_odb *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitOdb(git_odb *raw);
    ~GitOdb();

    static NAN_METHOD(New);

    static NAN_METHOD(Create);
    static NAN_METHOD(Open);
    static NAN_METHOD(AddDiskAlternate);
    static NAN_METHOD(Read);

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
    static NAN_METHOD(ReadPrefix);
    static NAN_METHOD(ReadHeader);
    static NAN_METHOD(Exists);
    static NAN_METHOD(Refresh);
    static NAN_METHOD(Write);
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
    static NAN_METHOD(Hash);
    static NAN_METHOD(Hashfile);
    git_odb *raw;
};

#endif
