/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITINDEXENTRY_H
#define GITINDEXENTRY_H

#include <nan.h>
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

    static NAN_METHOD(New);

    static NAN_METHOD(Ctime);
    static NAN_METHOD(Mtime);
    static NAN_METHOD(Dev);
    static NAN_METHOD(Ino);
    static NAN_METHOD(Mode);
    static NAN_METHOD(Uid);
    static NAN_METHOD(gid);
    static NAN_METHOD(FileSize);
    static NAN_METHOD(Oid);
    static NAN_METHOD(Flags);
    static NAN_METHOD(FlagsExtended);
    static NAN_METHOD(Path);
    git_index_entry *raw;
};

#endif
