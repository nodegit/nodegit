/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("IndexEntry"));


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

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("IndexEntry"), _constructor_template);
}

NAN_METHOD(GitIndexEntry::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_index_entry is required.");
  }
  GitIndexEntry* object = new GitIndexEntry(static_cast<git_index_entry *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitIndexEntry::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitIndexEntry::constructor_template)->NewInstance(1, argv));
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
    to = NanNull();
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
    to = NanNull();
  }
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Dev) {
  NanScope();
    Handle<Value> to;

  unsigned int dev =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->dev;

    to = NanNew<Uint32>((uint32_t)dev);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Ino) {
  NanScope();
    Handle<Value> to;

  unsigned int ino =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->ino;

    to = NanNew<Uint32>((uint32_t)ino);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Mode) {
  NanScope();
    Handle<Value> to;

  uint16_t mode =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->mode;

    to = NanNew<Integer>(mode);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Uid) {
  NanScope();
    Handle<Value> to;

  unsigned int uid =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->uid;

    to = NanNew<Uint32>((uint32_t)uid);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::gid) {
  NanScope();
    Handle<Value> to;

  unsigned int gid =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->gid;

    to = NanNew<Uint32>((uint32_t)gid);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::FileSize) {
  NanScope();
    Handle<Value> to;

  unsigned int file_size =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->file_size;

    to = NanNew<Uint32>((uint32_t)file_size);
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
    to = NanNull();
  }
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Flags) {
  NanScope();
    Handle<Value> to;

  uint16_t flags =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->flags;

    to = NanNew<Integer>(flags);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::FlagsExtended) {
  NanScope();
    Handle<Value> to;

  uint16_t flags_extended =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->flags_extended;

    to = NanNew<Integer>(flags_extended);
  NanReturnValue(to);
}

NAN_METHOD(GitIndexEntry::Path) {
  NanScope();
    Handle<Value> to;

  char * path =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->path;

    to = NanNew<String>(path);
  NanReturnValue(to);
}

Persistent<Function> GitIndexEntry::constructor_template;
