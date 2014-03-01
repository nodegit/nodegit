/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/diff_file.h"
#include "../include/oid.h"

using namespace v8;
using namespace node;

GitDiffFile::GitDiffFile(git_diff_file *raw) {
  this->raw = raw;
}

GitDiffFile::~GitDiffFile() {
  free(this->raw);
}

void GitDiffFile::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("DiffFile"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "path", Path);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "flags", Flags);
  NODE_SET_PROTOTYPE_METHOD(tpl, "mode", Mode);

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("DiffFile"), tpl->GetFunction());
}

NAN_METHOD(GitDiffFile::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_diff_file is required."));
  }

  GitDiffFile* object = new GitDiffFile((git_diff_file *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitDiffFile::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_diff_file *GitDiffFile::GetValue() {
  return this->raw;
}

NAN_METHOD(GitDiffFile::Oid) {
  NanScope();
    Handle<Value> to;

  git_oid *oid =
    &ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->oid;

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

NAN_METHOD(GitDiffFile::Path) {
  NanScope();
    Handle<Value> to;

  const char * path =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->path;

    to = String::New(path);
  NanReturnValue(to);
}

NAN_METHOD(GitDiffFile::Size) {
  NanScope();
    Handle<Value> to;

  git_off_t size =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->size;

    to = Integer::New(size);
  NanReturnValue(to);
}

NAN_METHOD(GitDiffFile::Flags) {
  NanScope();
    Handle<Value> to;

  uint32_t flags =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->flags;

    to = Integer::New(flags);
  NanReturnValue(to);
}

NAN_METHOD(GitDiffFile::Mode) {
  NanScope();
    Handle<Value> to;

  uint16_t mode =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->mode;

    to = Integer::New(mode);
  NanReturnValue(to);
}

Persistent<FunctionTemplate> GitDiffFile::constructor_template;
