/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/index_entry.h"
#include "../include/index_time.h"
#include "../include/oid.h"

using namespace v8;
using namespace node;

GitIndexEntry::GitIndexEntry(git_index_entry *raw) {
  this->raw = raw;
}

GitIndexEntry::~GitIndexEntry() {
  free(this->raw);
}

void GitIndexEntry::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("IndexEntry"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "ctime", Ctime);
  NODE_SET_PROTOTYPE_METHOD(tpl, "mtime", Mtime);
  NODE_SET_PROTOTYPE_METHOD(tpl, "dev", Dev);
  NODE_SET_PROTOTYPE_METHOD(tpl, "ino", Ino);
  NODE_SET_PROTOTYPE_METHOD(tpl, "mode", Mode);
  NODE_SET_PROTOTYPE_METHOD(tpl, "uid", Uid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "gid", gid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "file_size", FileSize);
  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "flags", Flags);
  NODE_SET_PROTOTYPE_METHOD(tpl, "flags_extended", FlagsExtended);
  NODE_SET_PROTOTYPE_METHOD(tpl, "path", Path);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("IndexEntry"), constructor_template);
}

Handle<Value> GitIndexEntry::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_index_entry is required.")));
  }

  GitIndexEntry* object = new GitIndexEntry((git_index_entry *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitIndexEntry::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitIndexEntry::constructor_template->NewInstance(1, argv));
}

git_index_entry *GitIndexEntry::GetValue() {
  return this->raw;
}


Handle<Value> GitIndexEntry::Ctime(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_index_time *ctime =
    &ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->ctime;

    if (ctime != NULL) {
    ctime = (git_index_time *)git_index_time_dup(ctime);
  }
  if (ctime != NULL) {
    to = GitIndexTime::New((void *)ctime);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Mtime(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_index_time *mtime =
    &ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->mtime;

    if (mtime != NULL) {
    mtime = (git_index_time *)git_index_time_dup(mtime);
  }
  if (mtime != NULL) {
    to = GitIndexTime::New((void *)mtime);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Dev(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  unsigned int dev =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->dev;

    to = Uint32::New(dev);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Ino(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  unsigned int ino =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->ino;

    to = Uint32::New(ino);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Mode(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  uint16_t mode =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->mode;

    to = Integer::New(mode);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Uid(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  unsigned int uid =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->uid;

    to = Uint32::New(uid);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::gid(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  unsigned int gid =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->gid;

    to = Uint32::New(gid);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::FileSize(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  unsigned int file_size =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->file_size;

    to = Uint32::New(file_size);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Oid(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_oid *oid =
    &ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->oid;

    if (oid != NULL) {
    oid = (git_oid *)git_oid_dup(oid);
  }
  if (oid != NULL) {
    to = GitOid::New((void *)oid);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Flags(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  uint16_t flags =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->flags;

    to = Integer::New(flags);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::FlagsExtended(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  uint16_t flags_extended =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->flags_extended;

    to = Integer::New(flags_extended);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Path(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  char * path =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->path;

    to = String::New(path);
  return scope.Close(to);
}

Persistent<Function> GitIndexEntry::constructor_template;
