/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITINDEXENTRY_H
#define GITINDEXENTRY_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitIndexEntry : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_index_entry *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitIndexEntry(git_index_entry *raw);
    ~GitIndexEntry();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Ctime(const Arguments& args);
    static Handle<Value> Mtime(const Arguments& args);
    static Handle<Value> Dev(const Arguments& args);
    static Handle<Value> Ino(const Arguments& args);
    static Handle<Value> Mode(const Arguments& args);
    static Handle<Value> Uid(const Arguments& args);
    static Handle<Value> gid(const Arguments& args);
    static Handle<Value> FileSize(const Arguments& args);
    static Handle<Value> Oid(const Arguments& args);
    static Handle<Value> Flags(const Arguments& args);
    static Handle<Value> FlagsExtended(const Arguments& args);
    static Handle<Value> Path(const Arguments& args);

    git_index_entry *raw;
};

#endif
