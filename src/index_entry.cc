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
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("IndexEntry"));

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

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("IndexEntry"), tpl->GetFunction());
}

NAN_METHOD(GitIndexEntry::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_index_entry is required."));
  }

  GitIndexEntry* object = new GitIndexEntry((git_index_entry *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitIndexEntry::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_index_entry *GitIndexEntry::GetValue() {
  return this->raw;
}

NAN_METHOD(GitIndexEntry::Ctime) {
  NanScope();
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
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Mtime) {
  NanScope();
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
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Dev) {
  NanScope();
    Handle<Value> to;

  unsigned int dev =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->dev;

    to = Uint32::New(dev);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Ino) {
  NanScope();
    Handle<Value> to;

  unsigned int ino =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->ino;

    to = Uint32::New(ino);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Mode) {
  NanScope();
    Handle<Value> to;

  uint16_t mode =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->mode;

    to = Integer::New(mode);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Uid) {
  NanScope();
    Handle<Value> to;

  unsigned int uid =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->uid;

    to = Uint32::New(uid);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::gid) {
  NanScope();
    Handle<Value> to;

  unsigned int gid =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->gid;

    to = Uint32::New(gid);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::FileSize) {
  NanScope();
    Handle<Value> to;

  unsigned int file_size =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->file_size;

    to = Uint32::New(file_size);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Oid) {
  NanScope();
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
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Flags) {
  NanScope();
    Handle<Value> to;

  uint16_t flags =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->flags;

    to = Integer::New(flags);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::FlagsExtended) {
  NanScope();
    Handle<Value> to;

  uint16_t flags_extended =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->flags_extended;

    to = Integer::New(flags_extended);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Path) {
  NanScope();
    Handle<Value> to;

  char * path =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->path;

    to = String::New(path);
  NanReturnValue(to);
}

Persistent<FunctionTemplate> GitIndexEntry::constructor_template;
