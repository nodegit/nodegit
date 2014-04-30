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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("DiffFile"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "path", Path);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "flags", Flags);
  NODE_SET_PROTOTYPE_METHOD(tpl, "mode", Mode);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("DiffFile"), constructor_template);
}

Handle<Value> GitDiffFile::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_diff_file is required.")));
  }

  GitDiffFile* object = new GitDiffFile((git_diff_file *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitDiffFile::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitDiffFile::constructor_template->NewInstance(1, argv));
}

git_diff_file *GitDiffFile::GetValue() {
  return this->raw;
}


Handle<Value> GitDiffFile::Oid(const Arguments& args) {
  HandleScope scope;
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
  return scope.Close(to);
}

Handle<Value> GitDiffFile::Path(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  const char * path =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->path;

    to = String::New(path);
  return scope.Close(to);
}

Handle<Value> GitDiffFile::Size(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_off_t size =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->size;

    to = Integer::New(size);
  return scope.Close(to);
}

Handle<Value> GitDiffFile::Flags(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  uint32_t flags =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->flags;

    to = Integer::New(flags);
  return scope.Close(to);
}

Handle<Value> GitDiffFile::Mode(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  uint16_t mode =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->mode;

    to = Integer::New(mode);
  return scope.Close(to);
}

Persistent<Function> GitDiffFile::constructor_template;
