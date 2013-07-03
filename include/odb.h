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
    static Handle<Value> ReadPrefix(const Arguments& args);
    static Handle<Value> ReadHeader(const Arguments& args);
    static Handle<Value> Exists(const Arguments& args);
    static Handle<Value> Refresh(const Arguments& args);
    static Handle<Value> Write(const Arguments& args);
    static Handle<Value> Hash(const Arguments& args);
    static Handle<Value> Hashfile(const Arguments& args);
    git_odb *raw;
};

#endif
